/* ldata
 *  literal data.  Must be loaded verbatim.  In M.U.G.E.N, sprite data in the
 *   ldata block is loaded verbatim into memory, and decompressed as necessary on-the-fly.
 *
 * tdata
 *  translate data.  Must be translated (e.g. decompressed) during load.
 *
 * everything is little endian
 *
 * sff header
 * dec  hex  size   meaning
 *  0    0    12   "ElecbyteSpr\0" signature
 *  12    C     1   verlo3; 0
 *  13    D     1   verlo2; 0
 *  14    E     1   verlo1; 0
 *  15    F     1   verhi; 2
 *  16   10     4   reserved; 0
 *  20   14     4   reserved; 0
 *  24   18     1   compatverlo3; 0
 *  25   19     1   compatverlo1; 0
 *  26   1A     1   compatverlo2; 0
 *  27   1B     1   compatverhi; 2
 *  28   1C     4   reserved; 0
 *  32   20     4   reserved; 0
 *  36   24     4   offset where first sprite node header data is located
 *  40   28     4   Total number of sprites
 *  44   2C     4   offset where first palette node header data is located
 *  48   30     4   Total number of palettes
 *  52   34     4   ldata offset
 *  56   38     4   ldata length
 *  60   3C     4   tdata offset
 *  64   40     4   tdata length
 *  68   44     4   reserved; 0
 *  72   48     4   reserved; 0
 *  76   4C   436   unused
 *
 * sprite
 * dec  hex  size   meaning
 *  0    0     2   groupno
 *  2    2     2   itemno
 *  4    4     2   width
 *  6    6     2   height
 *  8    8     2   axisx
 * 10    A     2   axisy
 * 12    C     2   Index number of the linked sprite (if linked)
 * 14    E     1   fmt
 * 15    F     1   coldepth
 * 16   10     4   offset into ldata or tdata
 * 20   14     4   Sprite data length (0: linked)
 * 24   18     2   palette index
 * 26   1A     2   flags
 *
 * fmt
 * 0 raw
 * 1 invalid (no use)
 * 2 RLE8
 * 3 RLE5
 * 4 LZ5
 *
 * flags
 * 0    unset: literal (use ldata); set: translate (use tdata; decompress on load)
 * 1-15 unused
 */

#include "util/file-system.h"
#include "util/debug.h"
#include <stdio.h>
#include <fstream>
#include <map>
#include <algorithm>

using namespace std;

namespace Util{

Filesystem::AbsolutePath getDataPath2(){
    return Filesystem::AbsolutePath("/");
}

static int upperCase(int c){
    return toupper(c);
}

string upperCaseAll(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), upperCase);
    return str;
}

}

static int computeFileSize(const string & path){
    // Lets get the filesize
    FILE * stream = fopen(path.c_str(), "r" );
    fseek(stream, 0, SEEK_END);
    int filesize = ftell(stream);
    fclose(stream);
    return filesize;
}

class SffV2Reader{
public:
    struct SpriteHeader{
        SpriteHeader(uint16_t group, uint16_t item, uint16_t width,
                     uint16_t height, uint16_t axisx, uint16_t axisy,
                     uint16_t linked, uint8_t format, uint8_t colorDepth,
                     uint32_t dataOffset, uint32_t dataLength, uint16_t palette,
                     uint16_t flags):
        group(group),
        item(item),
        width(width),
        height(height),
        axisx(axisx),
        axisy(axisy),
        linked(linked),
        format(format),
        colorDepth(colorDepth),
        dataOffset(dataOffset),
        dataLength(dataLength),
        palette(palette),
        flags(flags){
        }

        SpriteHeader(const SpriteHeader & copy):
        group(copy.group),
        item(copy.item),
        width(copy.width),
        height(copy.height),
        axisx(copy.axisx),
        axisy(copy.axisy),
        linked(copy.linked),
        format(copy.format),
        colorDepth(copy.colorDepth),
        dataOffset(copy.dataOffset),
        dataLength(copy.dataLength),
        palette(copy.palette),
        flags(copy.flags){
        }

        SpriteHeader(){
        }

        uint16_t group;
        uint16_t item;
        uint16_t width;
        uint16_t height;
        uint16_t axisx;
        uint16_t axisy;
        uint16_t linked;
        uint8_t format;
        uint8_t colorDepth;
        uint32_t dataOffset;
        uint32_t dataLength;
        uint16_t palette;
        uint16_t flags;
    };

    SffV2Reader(const Filesystem::AbsolutePath & filename):
    filename(filename),
    currentSprite(0){
        Global::debug(0) << "Opening file " << filename.path() << endl;
        /* 16 skips the header stuff */
        sffStream.open(filename.path().c_str(), ios::binary);
        if (!sffStream){
            printf("Failed to open %s\n", filename.path().c_str());
            throw exception();
            // throw MugenException("Could not open SFF file: '" + filename.path() + "'");
        }

        filesize = computeFileSize(filename.path());

        // location = 16;
        // sffStream.seekg(location, ios::beg);
        totalImages = 0;
        uint32_t suboffset = 0;
        uint32_t subhead = 0;
        uint32_t subpalette = 0;
        uint32_t totalPalettes = 0;

        Filesystem::LittleEndianReader reader(sffStream);
        string signature = reader.readString2(12);
        if (signature != "ElecbyteSpr"){
            Global::debug(0) << "Invalid signature. Got '" << signature << "'" << endl;
            throw exception();
        }

        int versionLo3 = reader.readByte1();
        int versionLo2 = reader.readByte1();
        int versionLo1 = reader.readByte1();
        int versionHi = reader.readByte1();
        if (versionHi != 2){
            Global::debug(0) << "Invalid version. Got " << versionHi << "." << versionLo1 << "." << versionLo2 << "." << versionLo3 << endl;
            throw exception();
        }

        /* skip reserved */
        reader.readByte4();
        reader.readByte4();

        /* ignore compatability version number */
        reader.readByte4();

        /* skip reserved */
        reader.readByte4();
        reader.readByte4();

        suboffset = reader.readByte4();
        totalImages = reader.readByte4();
        subpalette = reader.readByte4();
        totalPalettes = reader.readByte4();
        
        Global::debug(0) << "Image offset " << suboffset << " total images " << totalImages << " palette offset " << subpalette << " total palettes " << totalPalettes << endl;

        /* compressed data */
        uint32_t ldataOffset = reader.readByte4();
        uint32_t ldataLength = reader.readByte4();

        /* uncompressed data */
        uint32_t tdataOffset = reader.readByte4();
        uint32_t tdataLength = reader.readByte4();

        /* skip reserved */
        reader.readByte4();
        reader.readByte4();
        
        /* seek to first sprite */
        sffStream.seekg(suboffset, ios::beg);

        for (unsigned int index = 0; index < totalImages; index++){
            uint16_t group = reader.readByte2();
            uint16_t item = reader.readByte2();
            uint16_t width = reader.readByte2();
            uint16_t height = reader.readByte2();
            uint16_t axisx = reader.readByte2();
            uint16_t axisy = reader.readByte2();
            uint16_t linked = reader.readByte2();
            uint8_t format = reader.readByte1();
            uint8_t colorDepth = reader.readByte1();
            uint32_t dataOffset = reader.readByte4();
            uint32_t dataLength = reader.readByte4();
            uint16_t palette = reader.readByte2();
            uint16_t flags = reader.readByte2();
            /* if flags == 0 then use ldata, if 1 then tdata */
            Global::debug(0) << " " << group << ", " << item << " " << width << "x" << height << " format " << formatName(format) << " data " << dataOffset << " length " << dataLength << (flags == 0 ? " literal" : " translate") << endl;
            sprites.push_back(SpriteHeader(group, item, width, height,
                                           axisx, axisy, linked, format,
                                           colorDepth, dataOffset, dataLength,
                                           palette, flags));
        }

        /*
        for (vector<SpriteHeader>::iterator it = sprites.begin(); it != sprites.end(); it++){
            const SpriteHeader & sprite = *it;
            if (sprite.flags == 0){
                read(sprite, reader, ldataOffset, ldataLength);
            } else {
                read(sprite, reader, tdataOffset, tdataLength);
            }
        }
        */

        const SpriteHeader & sprite = sprites.back();
        if (sprite.flags == 0){
            read(sprite, reader, ldataOffset + sprite.dataOffset, sprite.dataLength);
        } else {
            read(sprite, reader, tdataOffset + sprite.dataOffset, sprite.dataLength);
        }
    }

    void read(const SpriteHeader & sprite, Filesystem::LittleEndianReader & reader, uint32_t offset, uint32_t length){
        Global::debug(0) << "Read sprite " << sprite.group << ", " << sprite.item << endl;
        sffStream.seekg(offset, ios::beg);
        switch (sprite.format){
            case 2: readRLE8(reader, length); break;
            case 3: readRLE5(reader, length); break;
            case 4: readLZ5(reader, length); break;
            default: {
                Global::debug(0) << "Don't understand format " << sprite.format << endl;
                throw exception();
            }
        }
    }

    void readRLE8(Filesystem::LittleEndianReader & reader, uint32_t length){
    }

    void readRLE5(Filesystem::LittleEndianReader & reader, uint32_t length){
    }

    enum LZ5PacketType{
        RLEShort,
        RLELong,
        LZ5Short,
        LZ5Long
    };

    struct LZ5Packet{
        LZ5PacketType type;
        int pixel; /* offset for lz5 */
        int length;
    };

    /* get the stream of packets */
    vector<LZ5Packet> readLZ5Packets(Filesystem::LittleEndianReader & reader, uint32_t length){
        vector<LZ5Packet> packets;
        uint8_t * compressed = new uint8_t[length];
        reader.readBytes(compressed, length);
        uint32_t total = 0;

        uint8_t recycled = 0;
        uint8_t lz5ShortCount = 0;
        while (total < length){
            uint8_t control = compressed[total];
            total += 1;
            /*
            uint8_t data[8];
            int dataRead = 0;
            for (dataRead = 0; dataRead < 8 && total < length; dataRead += 1, total += 1){
                data[dataRead] = compressed[total];
            }
            */

            ostream & out = Global::debug(0);
            for (int packet = 0; packet < 8 && total < length; packet++){
                if ((control & (1 << packet)) == 0){
                    /* RLE */
                    if ((compressed[total] >> 4) == 0){
                        out << "Long RLE ";
                        total += 4;
                    } else {
                        out << "Short RLE ";
                        total += 2;
                    }
                } else {
                    /* LZ5 */
                    if ((compressed[total] & 31) == 0){
                        if (lz5ShortCount == 3){
                            out << "Short LZ5* ";
                            lz5ShortCount = 0;
                            recycled = 0;
                            total += 1;
                        } else {
                            out << "Short LZ5 ";
                            recycled = (recycled << 2) | (compressed[total] >> 6);
                            lz5ShortCount += 1;
                            total += 2;
                        }
                    } else {
                        total += 4;
                        out << "Long LZ5 ";
                    }
                }
            }
            out << endl;

            /*
            int current = 0;
            ostream & out = Global::debug(0);
            while (current < 8){
                if ((control & (1 << current)) == 0){
                    if ((data[current] >> 4) == 0){
                        out << "Long RLE ";
                        current += 3;
                    } else {
                        out << "Short RLE ";
                        current += 2;
                    }
                } else {
                    if ((data[current] & 31) == 0){
                        current += 2;
                        out << "Short LZ5 ";
                    } else {
                        current += 4;
                        out << "Long LZ5 ";
                    }
                }
            }
            if (current >= 8){
                out << " overflow!";
            }
            out << endl;
            */
        }

        delete[] compressed;
        
        return packets;
    }

    void readLZ5(Filesystem::LittleEndianReader & reader, uint32_t length){
        vector<LZ5Packet> packets = readLZ5Packets(reader, length);
                /*
        while (total < length){
            total += 1;
            uint8_t data[8];
            int dataRead = 0;
            for (dataRead = 0; dataRead < 8 && total < length; dataRead += 1, total += 1){
                data[dataRead] = reader.readByte1();
            }
            ostream & out = Global::debug(0);
            out << " ";
            for (int i = 0; i < dataRead; i++){
                bool enabled = control & (1 << i);
                out << (i + 1) << ": ";
                if (enabled){
                    if ((data[i] & 31) == 0){
                        out << "Short LZ5 ";
                    } else {
                        out << "Long LZ5 ";
                    }
                } else {
                    if ((data[i] >> 4) == 0){
                        out << "Long RLE ";
                    } else {
                        out << "Short RLE ";
                    }
                }
            }
            out << endl;
        }
        */
    }

    string formatName(int format){
        switch (format){
            case 0: return "raw";
            case 1: return "invalid";
            case 2: return "RLE8";
            case 3: return "RLE5";
            case 4: return "LZ5";
            default: return "unknown";
        }
    }

    virtual ~SffV2Reader(){
        sffStream.close();
        // delete[] spriteIndex;
    }

    /* gets all the sprite headers without loading the pcx information */
    void quickReadSprites(){
        /*
        for (unsigned int index = 0; index < totalImages; index++){
            MugenSprite * sprite = new MugenSprite();
            sprite->read(sffStream, location);
            spriteIndex[index] = sprite;
            location = sprite->getNext();
        }
        */
    }

    /* Actually loads the pcx data */
    /*
    MugenSprite * loadSprite(MugenSprite * sprite, bool mask){
        if (sprite->getBitmap(mask) == NULL){
            if (sprite->getLength() == 0){
                const MugenSprite * temp = loadSprite(spriteIndex[sprite->getPrevious()], mask);
                if (!temp){
                    ostringstream out;
                    out << "Unknown linked sprite " << sprite->getPrevious() << endl;
                    throw MugenException(out.str());
                }
                sprite->copyImage(temp);
            } else {
                bool islinked = false;
                sprite->loadPCX(sffStream, islinked, useact, palsave1, mask);
            }
        }
        return sprite;
    }
    */

    /*
    MugenSprite * findSprite(int group, int item, bool mask){
        if (spriteIndex.size() == 0){
            quickReadSprites();
        }
        for (map<int, MugenSprite *>::iterator it = spriteIndex.begin(); it != spriteIndex.end(); it++){
            MugenSprite * sprite = it->second;
            if (sprite->getGroupNumber() == group &&
                sprite->getImageNumber() == item){
                / * make a deep copy * /
                return new MugenSprite(*loadSprite(sprite, mask));
            }
        }
        return NULL;
    }
    */

    /* deletes all sprites, only call this if you don't want them! */
    /*
    void cleanup(){
        for (map<int, MugenSprite *>::iterator it = spriteIndex.begin(); it != spriteIndex.end(); it++){
            MugenSprite * sprite = it->second;
            delete sprite;
        }
    }
    */

    /*
    MugenSprite * readSprite(bool mask){
        bool islinked = false;
        if (location > filesize){
            throw MugenException("Error in SFF file: " + filename.path() + ". Offset of image beyond the end of the file.");
        }

        MugenSprite * sprite = new MugenSprite();
        sprite->read(sffStream, location);
        location = sprite->getNext();

        if (sprite->getLength() == 0){
            const MugenSprite * temp = spriteIndex[sprite->getPrevious()];
            if (!temp){
                ostringstream out;
                out << "Unknown linked sprite " << sprite->getPrevious() << endl;
                throw MugenException(out.str());
            }
            sprite->copyImage(temp);
        } else {
            sprite->loadPCX(sffStream, islinked, useact, palsave1, mask);
        }
            
        spriteIndex[currentSprite] = sprite;
        currentSprite += 1;

        return sprite;
    }
    */

    bool moreSprites(){
        return currentSprite < totalImages;
    }

protected:
    const Filesystem::AbsolutePath filename;
    ifstream sffStream;
    unsigned long currentSprite;
    int totalSprites;
    // map<int, MugenSprite*> spriteIndex;
    vector<SpriteHeader> sprites;
    bool useact;
    int filesize;
    int location;
    uint32_t totalImages;
    unsigned char palsave1[768]; // First image palette
};

int main(int argc, char ** argv){
    Global::debug(0) << "Sffv2 reader" << endl;
    if (argc > 1){
        try{
            Filesystem::AbsolutePath path(argv[1]);
            SffV2Reader reader(path);
        } catch (...){
        }
    } else {
        printf("Give a .sff file as an argument\n");
    }
}
