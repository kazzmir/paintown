/* from http://www.bmarquis.com/wiki/index.php5?title=Elecbyte_Spritesheet_File_Format_Version_2 */
#include <iostream>
#include <cmath>
#include <cstring>
#include "sff.h"
 
unsigned long RLE8_Packet::SetPacket(ifstream &In, uint8_t* Sprite, unsigned long &Ofs)
{
    //bytecount allows the program to count all bytes processed, to be used by SFF2_FileHeader::GetSprite to determine the end of the data
    unsigned long bytecount = 0;
    RunMarker = 0;
    uint8_t Raw = 0;
    In.read((char*)&Raw,sizeof(uint8_t)); //Read raw byte.
    bytecount+=sizeof(uint8_t);           //increment byte counter
    RunMarker = Raw & 0xC0;
    RunLength = Raw & 0x3F;
    if(RunMarker == 0x40)                //If the run marker is set...
    {
        In.read((char*)&Raw,sizeof(uint8_t));//Read another byte for color.
        bytecount+=sizeof(uint8_t);          //and increment.
        for(int a=0; a<RunLength; a++)
        {
            Sprite[Ofs] = Raw;
            Ofs++;
        }
    }
    else                                //Or else this is the color you are searching for
    {
        Sprite[Ofs] = Raw;
        Ofs++;
    }
    return bytecount;
}
 
unsigned long RLE5_Packet::Set3R5V(ifstream &In, uint8_t* Sprite, unsigned long &Ofs)
{
    unsigned long bytecount = 0;
    In.read((char*)&P3R5V,sizeof(uint8_t));  //Read the packet.
    bytecount+=sizeof(uint8_t);
    uint8_t P5V = P3R5V & 0x1F;              //Break it down. P5V is the Packet's 5-bit color value.
    uint8_t P3R = (P3R5V >>5)+1;             //P3R is the packets 3-bit run length plus 1, so increment it, as well.
    for(int a=0; a<P3R; a++)
    {
        Sprite[Ofs] = P5V;
        Ofs++;
    }
    return bytecount;
}
 
unsigned long RLE5_Packet::SetPacket(ifstream &In, uint8_t* Sprite, unsigned long &Ofs)
{
    unsigned long bytecount = 0;
    uint8_t Raw;
    In.read((char*)&RunLength,sizeof(uint8_t));  //Read the run length.
    RunLength++;                                 //Remember to increment it after getting it out of the stream
    bytecount+=sizeof(uint8_t);
    In.read((char*)&Raw,sizeof(uint8_t));        //Read raw byte
    bytecount+=sizeof(uint8_t);
    ColorBit = Raw & 0x80;                       //ColorBit = Raw>>7
    DataLength = Raw & 0x7F;
    if(ColorBit)                                 //If the color bit is set
    {
        In.read((char*)&Raw,sizeof(uint8_t));    //Read the color
        bytecount+=sizeof(uint8_t);
    }
    else
    {
        Raw = 0;                                  //Or else the color is 0.
    }
    for(int a=0; a<RunLength; a++)
    {
        Sprite[Ofs] = Raw;
        Ofs++;
    }
    for(int a=0; a<DataLength; a++)
    {
        bytecount+=Set3R5V(In, Sprite, Ofs);      //Now it will process all the 3R5V packets in the main packet
    }
    return bytecount;
}
 
/*Instead of giving LZ5::SetPacket a return value, I thought it'll be feasible to have a countdown argument.
The purpose of this is because even though the control byte is 8-bits, all 8 bits are not used in the last packet*/
void LZ5_Packet::SetPacket(ifstream &In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
{
    In.read((char*)&CtrlByte, sizeof(uint8_t));//Read the control byte
	countdown-=sizeof(uint8_t);                //Decrement everytime you do use In.read
    for(uint8_t a=0; a<8 ; a++)
    {
        uint8_t b = uint8_t(pow(2.0,(int)a )); //This allows me to AND the control byte to determine which packet is next in line
        if(countdown)                          //This allows me to stop when all bytes have been processed
        {
            if(CtrlByte & b)DtrmnLZ(In, Sprite, Ofs, countdown); //If the bit is set, then it's an LZ packet
            else DtrmnRLE(In, Sprite, Ofs, countdown);           //If not, then it's an RLE packet
        }
    }
}
 
void LZ5_Packet::DtrmnLZ(ifstream &In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
{
    uint8_t Raw = (uint8_t)In.peek(); //Just need to peak.
    if(Raw &0x3F)                           //If it's still nonzero...
    {
        ProcessSLZ(In, Sprite, Ofs, countdown); //then the next packet is a Short LZ
    }
    else
    {
        ProcessLLZ(In, Sprite, Ofs, countdown); //those 5 bits are nothing, away. let's go long!
    }
}
 
void LZ5_Packet::ProcessSLZ(ifstream &In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
{
    uint8_t CopyLength;
    RecyBin++;                                  //The recycle bin, because this packet is eco-friendly.
    In.read((char*)&ShrtLZ[0],sizeof(uint8_t));
	countdown-=sizeof(uint8_t);
    if(RecyBin != 4)                            //we need another byte if the recycle bin isn't full
    {
        In.read((char*)&ShrtLZ[1],sizeof(uint8_t));
		countdown-=sizeof(uint8_t);
    }
    CopyLength = ShrtLZ[0] & 0x3f;
    CopyLength++;
    RecyByte = RecyByte << 2;
    RecyByte = RecyByte | (ShrtLZ[0]>>6) ;
    if(RecyBin == 4)
    {
        uint16_t Off = RecyByte+1;
        LZ_Copy(Sprite+Ofs,&Off,CopyLength);
        Ofs+=CopyLength;
        RecyByte = 0;                             //Clear the recycle byte.
        RecyBin = 0;                              //Empty the recycle bin
    }
    else
    {
        uint16_t Off = ShrtLZ[1]+1;
        LZ_Copy(Sprite+Ofs,&Off,CopyLength);
        Ofs+=CopyLength;
    }
}
 
void LZ5_Packet::ProcessLLZ(ifstream &In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
{
    uint32_t Off;                            //Same philosophy goes with offsets.
    In.read((char*)&LongLZ,sizeof(uint8_t)*3);
	countdown-=(sizeof(uint8_t))*3;
    Off =  LongLZ[0]<<2| LongLZ[1];     //ORing is okay, seeing is that almost everything is 0's, anyway.
    Off++;
    uint32_t CopyLength = LongLZ[2]+3;
    LZ_Copy(Sprite+Ofs,&Off,CopyLength);
    Ofs+=CopyLength;
}
 
void LZ5_Packet::LZ_Copy(uint8_t *dst, const uint16_t *ofs, unsigned int num)
{
 
    naivememcpy(dst,dst-*ofs, num);
}
 
void LZ5_Packet::LZ_Copy(uint8_t *dst, const uint32_t *ofs, unsigned int num)
{
 
    naivememcpy(dst, dst-*ofs, num);
}
 
void LZ5_Packet::naivememcpy(uint8_t *dst, const uint8_t *src, unsigned int num)
{
    uint8_t mybyte;
    while(num--)
    {
        mybyte=*src++;
        *dst++=mybyte;
    }
}
 
void LZ5_Packet::DtrmnRLE(ifstream &In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
{
    uint8_t Raw = (uint8_t)In.peek();
    if(Raw& 0xE0)                               //If taking out the bits still gives you something...
    {
        ProcessSRLE(In, Sprite, Ofs, countdown);//Then it's a Short RLE
    }
    else
    {
        ProcessLRLE(In, Sprite, Ofs, countdown);
    }
}
 
void LZ5_Packet::ProcessSRLE(ifstream &In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
{
    uint8_t Raw, Color, RunLength;
    In.read((char*)&Raw, sizeof(uint8_t));
	countdown-=sizeof(uint8_t);
    Color = Raw & 0x1F;
    RunLength = Raw >> 5;
    for(int a=0; a<RunLength;a++)
    {
        Sprite[Ofs] = Color;
        Ofs++;
    }
}
 
void LZ5_Packet::ProcessLRLE(ifstream &In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
{
    uint8_t Raw, Color,RunLength;
    In.read((char*)&Raw, sizeof(uint8_t));
	countdown-=sizeof(uint8_t);
    Color = Raw & 0x1F;
    In.read((char*)&RunLength, sizeof(uint8_t));
	countdown-=sizeof(uint8_t);
    for(int a=0; a<(uint16_t)RunLength+8;a++)
    {
        Sprite[Ofs] = Raw;
        Ofs++;
    }
}
 
LZ5_Packet::LZ5_Packet()
{
    RecyBin = 0; //You always need to start this off at 0.
    RecyByte = 0;
}
 
SFF2_FileHeader::SFF2_FileHeader()
{
 
}
 
SFF2_FileHeader::SFF2_FileHeader(ifstream &In)
{
    In.read(signature,12);
    In.read((char*)&verlo3,sizeof(uint8_t));
    In.read((char*)&verlo2,sizeof(uint8_t));
    In.read((char*)&verlo1,sizeof(uint8_t));
    In.read((char*)&verhi,sizeof(uint8_t));
    In.read((char*)&reserved0,sizeof(long));
    In.read((char*)&reserved1,sizeof(long));
    In.read((char*)&compatverlo3,sizeof(uint8_t));
    In.read((char*)&compatverlo2,sizeof(uint8_t));
    In.read((char*)&compatverlo1,sizeof(uint8_t));
    In.read((char*)&compatverhi,sizeof(uint8_t));
    In.read((char*)&reserved2,sizeof(long));
    In.read((char*)&reserved3,sizeof(long));
    In.read((char*)&sproff,sizeof(unsigned long));
    In.read((char*)&sprcount,sizeof(unsigned long));
    In.read((char*)&paloff,sizeof(unsigned long));
    In.read((char*)&palcount,sizeof(unsigned long));
    In.read((char*)&ldataoff,sizeof(unsigned long));
    In.read((char*)&ldatalen,sizeof(unsigned long));
    In.read((char*)&tdataoff,sizeof(unsigned long));
    In.read((char*)&tdatalen,sizeof(unsigned long));
    In.read((char*)&reserved4,sizeof(long));
    In.read((char*)&reserved5,sizeof(long));
    In.read(unused,436);
}
 
bool SFF2_FileHeader::isGenuine()
{
    if(!strcmp(signature, "ElecbyteSpr"))
        return true;
    return false;
}
 
unsigned long SFF2_FileHeader::GetlData()
{
    return ldataoff;
}
 
SprNode* SFF2_FileHeader::GetSprNode(ifstream &In, unsigned long index)
{
    if(index>=sprcount)
        return NULL;
    In.seekg(sproff+(28*index),ios::beg);
    SprNode *temp = new SprNode(In);
    return temp;
 
}
 
PalNode* SFF2_FileHeader::GetPalNode(ifstream &In, unsigned long index)
{
    if(index>=palcount)
        return NULL;
    In.seekg(paloff+(16*index),ios::beg);
    PalNode *temp = new PalNode(In);
    return temp;
}
 
unsigned long SFF2_FileHeader::GetNumSpr()
{
    return sprcount;
}
 
unsigned long SFF2_FileHeader::GetNumPal()
{
    return palcount;
}
///////////////////////
SprNode::SprNode()
{
 
}
SprNode::SprNode(ifstream &In)
{
 
    In.read((char*)&groupno,sizeof(unsigned short));
    In.read((char*)&itemno,sizeof(unsigned short));
    In.read((char*)&width,sizeof(unsigned short));
    In.read((char*)&height,sizeof(unsigned short));
    In.read((char*)&axisx,sizeof(short));
    In.read((char*)&axisy,sizeof(short));
    In.read((char*)&linkoff,sizeof(unsigned short));
    In.read((char*)&format,sizeof(uint8_t));
    In.read((char*)&coldepth,sizeof(uint8_t));
    In.read((char*)&dataoff,sizeof(unsigned long));
    In.read((char*)&sprlen,sizeof(unsigned long));
    In.read((char*)&palind,sizeof(unsigned short));
    In.read((char*)&flags,sizeof(unsigned short));
}
 
 
unsigned short SprNode::GetWidth()
{
    return width;
}
 
unsigned short SprNode::GetHeight()
{
    return height;
}
 
unsigned long SprNode::GetData()
{
    return dataoff;
}
 
int SprNode::GetFormat()
{
    return format;
}
 
unsigned long SprNode::GetDataLen()
{
    return sprlen;
}
 
int SprNode::GetDepth()
{
    return coldepth;
}
 
unsigned short SprNode::GetFlags()
{
    return flags;
}
 
unsigned short SprNode::GetPalInd()
{
    return palind;
}
 
bool SprNode::isLinked()
{
    if(!sprlen)
        return true;
    return false;
}
 
unsigned long SprNode::GetOffset()
{
    return linkoff;
}
 
short SprNode::GetAxisX()
{
    return axisx;
}
short SprNode::GetAxisY()
{
    return axisy;
}
 
unsigned short SprNode::GetGroupNo()
{
    return groupno;
}
unsigned short SprNode::GetImageNo()
{
    return itemno;
}
/////////////////////////////
unsigned short PalNode::GetNumColors()
{
    return numcols;
}
unsigned long PalNode::GetData()
{
    return dataoff;
}
unsigned long PalNode::GetDataLen()
{
    return datalen;
}
 
unsigned long PalNode::GetOffset()
{
    return linkoff;
}
 
bool PalNode::isLinked()
{
    if(!datalen)
        return true;
    return false;
}
 
PalNode::PalNode(ifstream &In)
{
     In.read((char*)&groupno,sizeof(unsigned short));
     In.read((char*)&itemno,sizeof(unsigned short));
     In.read((char*)&numcols,sizeof(unsigned short));
     In.read((char*)&linkoff,sizeof(unsigned short));
     In.read((char*)&dataoff,sizeof(unsigned long));
     In.read((char*)&datalen,sizeof(unsigned long));
}
 
colorinfo *SFF2_FileHeader::GetColor (ifstream& In, PalNode *Node, int index)
{
    if(index>=Node->GetNumColors())
        return NULL;
    colorinfo *temp = new colorinfo;
    In.seekg(ldataoff+Node->GetData()+(4*index),ios::beg);
    In.read((char*)&temp->r,sizeof(uint8_t));
    In.read((char*)&temp->g,sizeof(uint8_t));
    In.read((char*)&temp->b,sizeof(uint8_t));
    In.read((char*)&temp->a,sizeof(uint8_t));
    return temp;
}
 
uint8_t *SFF2_FileHeader::GetSprite(ifstream &In,SprNode *Node)
{
    uint8_t *temp = new uint8_t[Node->GetHeight()*Node->GetWidth()];
    unsigned long offset=0, DataLength;
    if(Node->isLinked())
    {
        SprNode *LinkedNode = GetSprNode(In, Node->GetOffset());
        DataLength = LinkedNode->GetDataLen();
        delete LinkedNode;
    }
    else DataLength = Node->GetDataLen();
    if(Node->GetFlags()& 0x1)
        In.seekg((tdataoff+Node->GetData())+4,ios::beg);
    else In.seekg((ldataoff+Node->GetData())+4,ios::beg);
    switch(Node->GetFormat())
    {
        case 0:
        {
            In.read((char*)temp,Node->GetHeight()*Node->GetWidth());
            break;
        }
        case 1:
        {
            cout << "Invalid Packet" << endl;
            break;
        }
        case 2:
        {
            RLE8_Packet Packet;
            for(unsigned long a=0; a<DataLength-4;a=a)
            {
                a+= Packet.SetPacket(In,temp,offset);
            }
            break;
        }
        case 3:
        {
            RLE5_Packet Packet;
            for(unsigned long a=0; a<DataLength-4;a=a)
            {
                a+= Packet.SetPacket(In,temp,offset);
 
            }
            break;
        }
        case 4:
        {
            LZ5_Packet Packet;
            for(unsigned long a=DataLength-4; a>0;a=a)
            {
                Packet.SetPacket(In,temp,offset,a);
            }
            break;
        }
    }
    return temp;
}
 
extern "C"
{
    unsigned long RLE8_Packet::SetPacket(FILE* In, uint8_t* Sprite, unsigned long &Ofs)
    {
        //bytecount allows the program to count all bytes processed, to be used by SFF2_FileHeader::GetSprite to determine the end of the data
        unsigned long bytecount = 0;
        RunMarker = 0;
        uint8_t Raw = 0;
        fread(&Raw,1,sizeof(uint8_t),In); //Read raw byte.
        bytecount+=sizeof(uint8_t);           //increment byte counter
        RunMarker = Raw & 0xC0;
        RunLength = Raw & 0x3F;
        if(RunMarker == 0x40)                //If the run marker is set...
        {
            fread(&Raw,1,sizeof(uint8_t),In);//Read another byte for color.
            bytecount+=sizeof(uint8_t);          //and increment.
            for(int a=0; a<RunLength; a++)
            {
                Sprite[Ofs] = Raw;
                Ofs++;
            }
        }
        else                                //Or else this is the color you are searching for
        {
            Sprite[Ofs] = Raw;
            Ofs++;
        }
        return bytecount;
    }
 
    unsigned long RLE5_Packet::Set3R5V(FILE* In, uint8_t* Sprite, unsigned long &Ofs)
    {
        unsigned long bytecount = 0;
        fread(&P3R5V,sizeof(uint8_t),1,In);  //Read the packet.
        bytecount+=sizeof(uint8_t);
        uint8_t P5V = P3R5V & 0x1F;              //Break it down. P5V is the Packet's 5-bit color value.
        uint8_t P3R = (P3R5V >>5)+1;             //P3R is the packets 3-bit run length plus 1, so increment it, as well.
        for(int a=0; a<P3R; a++)
        {
            Sprite[Ofs] = P5V;
            Ofs++;
        }
        return bytecount;
    }
 
    unsigned long RLE5_Packet::SetPacket(FILE* In, uint8_t* Sprite, unsigned long &Ofs)
    {
        unsigned long bytecount = 0;
        uint8_t Raw;
        fread(&RunLength,sizeof(uint8_t),1,In);  //Read the run length.
        RunLength++;                                 //Remember to increment it after getting it out of the stream
        bytecount+=sizeof(uint8_t);
        fread(&Raw,1,sizeof(uint8_t),In);        //Read raw byte
        bytecount+=sizeof(uint8_t);
        ColorBit = Raw & 0x80;                       //ColorBit = Raw>>7
        DataLength = Raw & 0x7F;
        if(ColorBit)                                 //If the color bit is set
        {
            fread(&Raw,1,sizeof(uint8_t),In);    //Read the color
            bytecount+=sizeof(uint8_t);
        }
        else
        {
            Raw = 0;                                  //Or else the color is 0.
        }
        for(int a=0; a<RunLength; a++)
        {
            Sprite[Ofs] = Raw;
            Ofs++;
        }
        for(int a=0; a<DataLength; a++)
        {
            bytecount+=Set3R5V(In, Sprite, Ofs);      //Now it will process all the 3R5V packets in the main packet
        }
        return bytecount;
    }
 
    /*Instead of giving LZ5::SetPacket a return value, I thought it'll be feasible to have a countdown argument.
    The purpose of this is because even though the control byte is 8-bits, all 8 bits are not used in the last packet*/
    void LZ5_Packet::SetPacket(FILE* In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
    {
        fread(&CtrlByte,sizeof(uint8_t),1,In);//Read the control byte
        countdown-=sizeof(uint8_t);                //Decrement everytime you do use In.read
        for(uint8_t a=0; a<8 ; a++)
        {
            uint8_t b = uint8_t(pow(2.0,(int)a )); //This allows me to AND the control byte to determine which packet is next in line
            if(countdown)                          //This allows me to stop when all bytes have been processed
            {
                if(CtrlByte & b)DtrmnLZ(In, Sprite, Ofs, countdown); //If the bit is set, then it's an LZ packet
                else DtrmnRLE(In, Sprite, Ofs, countdown);           //If not, then it's an RLE packet
            }
        }
    }
 
    void LZ5_Packet::DtrmnLZ(FILE* In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
    {
        uint8_t Raw = (uint8_t)fgetc(In); //Just need to peak.
        ungetc(Raw,In);
        if(Raw &0x3F)                           //If it's still nonzero...
        {
            ProcessSLZ(In, Sprite, Ofs, countdown); //then the next packet is a Short LZ
        }
        else
        {
            ProcessLLZ(In, Sprite, Ofs, countdown); //those 5 bits are nothing, away. let's go long!
        }
    }
 
    void LZ5_Packet::ProcessSLZ(FILE* In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
    {
        uint8_t CopyLength;
        RecyBin++;                                  //The recycle bin, because this packet is eco-friendly.
        fread(&ShrtLZ[0],sizeof(uint8_t),1,In);
        countdown-=sizeof(uint8_t);
        if(RecyBin != 4)                            //we need another byte if the recycle bin isn't full
        {
            fread(&ShrtLZ[1],sizeof(uint8_t),1,In);
            countdown-=sizeof(uint8_t);
        }
        CopyLength = ShrtLZ[0] & 0x3f;
        CopyLength++;
        RecyByte = RecyByte << 2;
        RecyByte = RecyByte | (ShrtLZ[0]>>6) ;
        if(RecyBin == 4)
        {
            uint16_t Off = RecyByte+1;
            LZ_Copy(Sprite+Ofs,&Off,CopyLength);
            Ofs+=CopyLength;
            RecyByte = 0;                             //Clear the recycle byte.
            RecyBin = 0;                              //Empty the recycle bin
        }
        else
        {
            uint16_t Off = ShrtLZ[1]+1;
            LZ_Copy(Sprite+Ofs,&Off,CopyLength);
            Ofs+=CopyLength;
        }
    }
 
    void LZ5_Packet::ProcessLLZ(FILE* In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
    {
        uint32_t Off;                            //Same philosophy goes with offsets.
        fread(&LongLZ,sizeof(uint8_t),3,In);
        countdown-=(sizeof(uint8_t))*3;
        Off =  LongLZ[0]<<2| LongLZ[1];     //ORing is okay, seeing is that almost everything is 0's, anyway.
        Off++;
        uint32_t CopyLength = LongLZ[2]+3;
        LZ_Copy(Sprite+Ofs,&Off,CopyLength);
        Ofs+=CopyLength;
    }
 
    void LZ5_Packet::DtrmnRLE(FILE* In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
    {
        uint8_t Raw = (uint8_t)fgetc(In);
        ungetc(Raw,In);
        if(Raw& 0xE0)                               //If taking out the bits still gives you something...
        {
            ProcessSRLE(In, Sprite, Ofs, countdown);//Then it's a Short RLE
        }
        else
        {
            ProcessLRLE(In, Sprite, Ofs, countdown);
        }
    }
 
    void LZ5_Packet::ProcessSRLE(FILE* In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
    {
        uint8_t Raw, Color, RunLength;
        fread(&Raw,sizeof(uint8_t),1,In);
        countdown-=sizeof(uint8_t);
        Color = Raw & 0x1F;
        RunLength = Raw >> 5;
        for(int a=0; a<RunLength;a++)
        {
            Sprite[Ofs] = Color;
            Ofs++;
        }
    }
 
    void LZ5_Packet::ProcessLRLE(FILE* In, uint8_t* Sprite, unsigned long &Ofs, unsigned long &countdown)
    {
        uint8_t Raw, Color,RunLength;
        fread(&Raw,sizeof(uint8_t),1,In);
        countdown-=sizeof(uint8_t);
        Color = Raw & 0x1F;
        fread(&RunLength,sizeof(uint8_t),1,In);
        countdown-=sizeof(uint8_t);
        for(int a=0; a<(uint16_t)RunLength+8;a++)
        {
            Sprite[Ofs] = Raw;
            Ofs++;
        }
    }
 
    SFF2_FileHeader::SFF2_FileHeader(FILE* In)
    {
        fread(signature,sizeof(char),12,In);
        fread(&verlo3,sizeof(uint8_t),1,In);
        fread(&verlo2,sizeof(uint8_t),1,In);
        fread(&verlo1,sizeof(uint8_t),1,In);
        fread(&verhi,sizeof(uint8_t),1,In);
        fread(&reserved0,sizeof(long),1,In);
        fread(&reserved1,sizeof(long),1,In);
        fread(&compatverlo3,sizeof(uint8_t),1,In);
        fread(&compatverlo2,sizeof(uint8_t),1,In);
        fread(&compatverlo1,sizeof(uint8_t),1,In);
        fread(&compatverhi,sizeof(uint8_t),1,In);
        fread(&reserved2,sizeof(long),1,In);
        fread(&reserved3,sizeof(long),1,In);
        fread(&sproff,sizeof(unsigned long),1,In);
        fread(&sprcount,sizeof(unsigned long),1,In);
        fread(&paloff,sizeof(unsigned long),1,In);
        fread(&palcount,sizeof(unsigned long),1,In);
        fread(&ldataoff,sizeof(unsigned long),1,In);
        fread(&ldatalen,sizeof(unsigned long),1,In);
        fread(&tdataoff,sizeof(unsigned long),1,In);
        fread(&tdatalen,sizeof(unsigned long),1,In);
        fread(&reserved4,sizeof(long),1,In);
        fread(&reserved5,sizeof(long),1,In);
        fread(unused,sizeof(char),436,In);
    }
 
    SprNode* SFF2_FileHeader::GetSprNode(FILE* In, unsigned long index)
    {
        if(index>=sprcount)
            return NULL;
        fseek(In,sproff+(28*index),SEEK_SET);
        SprNode *temp = new SprNode(In);
        return temp;
 
    }
 
    PalNode* SFF2_FileHeader::GetPalNode(FILE* In, unsigned long index)
    {
        if(index>=palcount)
            return NULL;
        fseek(In,paloff+(16*index),SEEK_SET);
        PalNode *temp = new PalNode(In);
        return temp;
    }
 
    ///////////////////////
    SprNode::SprNode(FILE* In)
    {
        fread(&groupno,sizeof(unsigned short),12,In);
        fread(&itemno,sizeof(unsigned short),1,In);
        fread(&width,sizeof(unsigned short),1,In);
        fread(&height,sizeof(unsigned short),1,In);
        fread(&axisx,sizeof(short),1,In);
        fread(&axisy,sizeof(short),1,In);
        fread(&linkoff,sizeof(unsigned short),1,In);
        fread(&format,sizeof(uint8_t),1,In);
        fread(&coldepth,sizeof(uint8_t),1,In);
        fread(&dataoff,sizeof(unsigned long),1,In);
        fread(&sprlen,sizeof(unsigned long),1,In);
        fread(&palind,sizeof(unsigned short),1,In);
        fread(&flags,sizeof(unsigned short),1,In);
    }
 
    /////////////////////////////
 
    PalNode::PalNode(FILE* In)
    {
        fread(&groupno,sizeof(unsigned short),12,In);
        fread(&itemno,sizeof(unsigned short),1,In);
        fread(&numcols,sizeof(unsigned short),1,In);
        fread(&linkoff,sizeof(unsigned short),1,In);
        fread(&dataoff,sizeof(unsigned long),1,In);
        fread(&datalen,sizeof(unsigned long),1,In);
    }
 
    colorinfo *SFF2_FileHeader::GetColor (FILE* In, PalNode *Node, int index)
    {
        if(index>=Node->GetNumColors())
            return NULL;
        colorinfo *temp = new colorinfo;
        fseek(In,ldataoff+Node->GetData()+(4*index),SEEK_SET);
        fread(&temp->r,sizeof(unsigned short),12,In);
        fread(&temp->g,sizeof(unsigned short),12,In);
        fread(&temp->b,sizeof(unsigned short),12,In);
        fread(&temp->a,sizeof(unsigned short),12,In);
        return temp;
    }
 
    uint8_t *SFF2_FileHeader::GetSprite(FILE* In,SprNode *Node)
    {
        uint8_t *temp = new uint8_t[Node->GetHeight()*Node->GetWidth()];
        unsigned long offset=0, DataLength;
        if(Node->isLinked())
        {
            SprNode *LinkedNode = GetSprNode(In, Node->GetOffset());
            DataLength = LinkedNode->GetDataLen();
            delete LinkedNode;
        }
        else DataLength = Node->GetDataLen();
        if(Node->GetFlags()& 0x1)
            fseek(In,tdataoff+Node->GetData()+4,SEEK_SET);
        else fseek(In,ldataoff+Node->GetData()+4,SEEK_SET);
        switch(Node->GetFormat())
        {
            case 0:
            {
                fread(temp,sizeof(uint8_t),Node->GetHeight()*Node->GetWidth(),In);
                break;
            }
            case 1:
            {
                cout << "Invalid Packet" << endl;
                break;
            }
            case 2:
            {
                RLE8_Packet Packet;
                for(unsigned long a=0; a<DataLength-4;a=a)
                {
                    a+= Packet.SetPacket(In,temp,offset);
                }
                break;
            }
            case 3:
            {
                RLE5_Packet Packet;
                for(unsigned long a=0; a<DataLength-4;a=a)
                {
                    a+= Packet.SetPacket(In,temp,offset);
 
                }
                break;
            }
            case 4:
            {
                LZ5_Packet Packet;
                for(unsigned long a=DataLength-4; a>0;a=a)
                {
                    Packet.SetPacket(In,temp,offset,a);
                }
                break;
            }
        }
        return temp;
    }
}
