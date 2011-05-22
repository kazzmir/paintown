/* from http://www.bmarquis.com/wiki/index.php5?title=Elecbyte_Spritesheet_File_Format_Version_2 */
#ifndef _SFF_H_
#define _SFF_H_
 
#include <fstream>
using namespace std;
#include <cstdio>
#include <stdint.h>
 
class RLE8_Packet
{
    uint8_t RunLength;
    uint8_t RunMarker;
    //Public Members
    public:
        unsigned long SetPacket(ifstream&, uint8_t*, unsigned long&); //Processes a RLE8 Packet from read char
        unsigned long SetPacket(FILE*, uint8_t*, unsigned long&);
};
 
class RLE5_Packet
{
  uint8_t RunLength;
  uint8_t ColorBit;
  uint8_t DataLength;
  uint8_t P3R5V; // a 3-bit Runlength/5-bit color packet
  public:
        //C++ IO
        unsigned long SetPacket(ifstream&, uint8_t*, unsigned long&);//Processes a RLE5 Packet from read char
        unsigned long Set3R5V(ifstream&, uint8_t*, unsigned long&);//Processes the 3-byte RL/ 5-bit color packets. Never to be used explicitly
        //C IO
        unsigned long SetPacket(FILE*, uint8_t*, unsigned long&);//Processes a RLE5 Packet from read char
        unsigned long Set3R5V(FILE*, uint8_t*, unsigned long&);//Instead, let RLE5_Packet::SetPacket handle everything.
};
 
class LZ5_Packet
{
  uint8_t CtrlByte;
  uint8_t ShrtRLE; // bit 0-4: pixel color, bit 5-7: runlength
  uint8_t LongRLE[2];// bit 0-4: pixel color, bit 5-7 = 0, bits 8-15: runlength minus 8
  uint8_t ShrtLZ[2];// bit 0-5: copylength minus 1, bit 6-7, recycle bits
  uint8_t RecyByte;//The recycled byte.
  uint8_t LongLZ[3];// bit 0-5: 0, bit 6-15 offset-1, bit 16-23 copylength-3
  uint8_t RecyBin; //Short LZ Counter;
  //Public Members
  public:
        LZ5_Packet();
        void naivememcpy(uint8_t *dst, const uint8_t *src, unsigned int num);//As documented on Elecbytes wiki page.
        void LZ_Copy(uint8_t *dst, const uint16_t *ofs, unsigned int num);//As documented on Elecbytes wiki page.
        void LZ_Copy(uint8_t *dst, const uint32_t *ofs, unsigned int num);//As documented on Elecbytes wiki page.
        //C++
        void SetPacket(ifstream&, uint8_t*, unsigned long&,unsigned long&);//Processes a LZ5 Packet to determine the order of Processing
        void DtrmnRLE(ifstream&, uint8_t*, unsigned long&,unsigned long&);//Determines whether the packet to be processed is a short or long RLE
        void DtrmnLZ(ifstream&, uint8_t*, unsigned long&,unsigned long&);//Determines whether the packet to be processed is a short or long LZ
        void ProcessSRLE(ifstream&, uint8_t*, unsigned long&,unsigned long&);//Processes a Short RLE packet
        void ProcessLRLE(ifstream&, uint8_t*, unsigned long&,unsigned long&);//Processes a Long RLE packet
        void ProcessSLZ(ifstream&, uint8_t*, unsigned long&,unsigned long&);//Processes a Short LZ packet
        void ProcessLLZ(ifstream&, uint8_t*, unsigned long&,unsigned long&);//Processes a Long LZ packet
        //C
        void SetPacket(FILE*, uint8_t*, unsigned long&,unsigned long&);//Processes a LZ5 Packet to determine the order of Processing
        void DtrmnRLE(FILE*, uint8_t*, unsigned long&,unsigned long&);//Determines whether the packet to be processed is a short or long RLE
        void DtrmnLZ(FILE*, uint8_t*, unsigned long&,unsigned long&);//Determines whether the packet to be processed is a short or long LZ
        void ProcessSRLE(FILE*, uint8_t*, unsigned long&,unsigned long&);//Processes a Short RLE packet
        void ProcessLRLE(FILE*, uint8_t*, unsigned long&,unsigned long&);//Processes a Long RLE packet
        void ProcessSLZ(FILE*, uint8_t*, unsigned long&,unsigned long&);//Processes a Short LZ packet
        void ProcessLLZ(FILE*, uint8_t*, unsigned long&,unsigned long&);//Processes a Long LZ packet
};
 
class SprNode
{
    protected:
        unsigned short groupno;
        unsigned short itemno;
        unsigned short width;
        unsigned short height;
        short axisx;
        short axisy;
        unsigned short linkoff;  //If Sprite is linked, then this is the offset of the linked sprite
        uint8_t format;          //0 == Raw; 1==Invalid; 2==RLE8; 3==RLE5; 4==LZ5
        uint8_t coldepth;
        unsigned long dataoff;
        unsigned long sprlen;
        unsigned short palind;
        unsigned short flags;
    public:
        SprNode();
        SprNode(ifstream &);
        SprNode(FILE*);
        unsigned short GetGroupNo();
        unsigned short GetImageNo();
        unsigned short GetWidth();
        unsigned short GetHeight();
        int GetDepth();
        int GetFormat();
        unsigned long GetData();
        unsigned long GetDataLen();
        unsigned short GetFlags();
        unsigned short GetPalInd();
        unsigned long GetOffset();
        bool isLinked();
        short GetAxisX();
        short GetAxisY();
};
 
class colorinfo
{
    public:
        uint8_t r,g,b,a;
};
 
class PalNode
{
    unsigned short groupno;
    unsigned short itemno;
    unsigned short numcols;
    unsigned short linkoff;
    unsigned long dataoff;
    unsigned long datalen;
    public:
        PalNode(ifstream &);
        PalNode(FILE*);
        unsigned short GetNumColors();
        unsigned long GetData();
        unsigned long GetDataLen();
        unsigned long GetOffset();
        bool isLinked();
};
 
class SFF2_FileHeader
{
    protected:
        char signature[12];
        uint8_t verlo3;
        uint8_t verlo2;
        uint8_t verlo1;
        uint8_t verhi;
        long reserved0;
        long reserved1;
        uint8_t compatverlo3;
        uint8_t compatverlo2;
        uint8_t compatverlo1;
        uint8_t compatverhi;
        long reserved2;
        long reserved3;
        unsigned long sproff;
        unsigned long sprcount;
        unsigned long paloff;
        unsigned long palcount;
        unsigned long ldataoff;
        unsigned long ldatalen;
        unsigned long tdataoff;
        unsigned long tdatalen;
        long reserved4;
        long reserved5;
        char unused[436];
    public:
        bool isGenuine();
        unsigned long GetlData();
        unsigned long GetNumSpr();
        unsigned long GetNumPal();
        //C++
        SFF2_FileHeader();
        SFF2_FileHeader(ifstream &);
        SprNode *GetSprNode(ifstream &, unsigned long);
        PalNode *GetPalNode(ifstream &, unsigned long);
        colorinfo *GetColor (ifstream&, PalNode*, int);
        uint8_t *GetSprite(ifstream&, SprNode*);
        //C
        SFF2_FileHeader(FILE*);
        SprNode *GetSprNode(FILE*, unsigned long);
        PalNode *GetPalNode(FILE*, unsigned long);
        colorinfo *GetColor (FILE*, PalNode*, int);
        uint8_t *GetSprite(FILE*, SprNode*);
};
#endif

