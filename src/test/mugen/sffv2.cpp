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
 *
 * Pal node header 2.00
 * 16 bytes
 *
 * dec  hex  size   meaning
 * 0    0     2  groupno
 * 2    2     2  itemno
 * 4    4     2  numcols
 * 6    6     2  Index number of the linked palette (if linked)
 * 8    8     4  Offset into ldata
 * 12   C     4  Palette data length (0: linked)
 */

#include "util/file-system.h"
#include "util/debug.h"
#include "util/funcs.h"
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <map>
#include <algorithm>
#include "util/graphics/bitmap.h"
#include "util/input/keyboard.h"
#include "util/input/input-manager.h"
#include "util/input/input-map.h"
#include "util/input/input-source.h"
#include "util/init.h"

#include "sff.h"

using namespace std;

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
                     uint16_t flags, unsigned int index):
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
        flags(flags),
        index(index){
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
        flags(copy.flags),
        index(copy.index){
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
        unsigned int index;
    };

    struct PaletteHeader{

        PaletteHeader(uint16_t group, uint16_t item, uint16_t colors,
                      uint16_t linked, uint32_t offset, uint32_t length,
                      unsigned int index):
            group(group),
            item(item),
            colors(colors),
            linked(linked),
            offset(offset),
            length(length),
            index(index){
            }

        PaletteHeader(){
        }

        PaletteHeader(const PaletteHeader & copy):
            group(copy.group),
            item(copy.item),
            colors(copy.colors),
            linked(copy.linked),
            offset(copy.offset),
            length(copy.length),
            index(copy.index){
            }

        uint16_t group;
        uint16_t item;
        uint16_t colors;
        uint16_t linked;
        uint32_t offset;
        uint32_t length;
        unsigned int index;
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

        Storage::LittleEndianReader reader(sffStream);
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
        ldataOffset = reader.readByte4();
        ldataLength = reader.readByte4();

        /* uncompressed data */
        tdataOffset = reader.readByte4();
        tdataLength = reader.readByte4();

        Global::debug(0) << "Ldata offset " << ldataOffset << " length " << ldataLength << endl;
        Global::debug(0) << "Tdata offset " << tdataOffset << " length " << tdataLength << endl;

        /* skip reserved */
        reader.readByte4();
        reader.readByte4();
        
        /* seek to first sprite */
        reader.seek(suboffset);

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
            Global::debug(0) << " " << group << ", " << item << " " << width << "x" << height << " format " << formatName(format) << " color depth " << (int) colorDepth << " data " << dataOffset << " length " << dataLength << (flags == 0 ? " literal" : " translate") << endl;
            sprites.push_back(SpriteHeader(group, item, width, height,
                                           axisx, axisy, linked, format,
                                           colorDepth, dataOffset, dataLength,
                                           palette, flags, index));
        }

        reader.seek(subpalette);
        for (unsigned int index = 0; index < totalPalettes; index++){
            uint16_t group = reader.readByte2();
            uint16_t item = reader.readByte2();
            uint16_t colors = reader.readByte2();
            uint16_t linked = reader.readByte2();
            uint32_t offset = reader.readByte4();
            uint32_t length = reader.readByte4();
            palettes.push_back(PaletteHeader(group, item, colors, linked, offset, length, index));
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

        /*
        const SpriteHeader & sprite = sprites.back();
        if (sprite.flags == 0){
            read(sprite, reader, ldataOffset + sprite.dataOffset, sprite.dataLength);
        } else {
            read(sprite, reader, tdataOffset + sprite.dataOffset, sprite.dataLength);
        }
        */
    }

    vector<SpriteHeader> getSprites(){
        vector<SpriteHeader> sprites;
        for (vector<SpriteHeader>::iterator it = this->sprites.begin(); it != this->sprites.end(); it++){
            const SpriteHeader & sprite = *it;
            sprites.push_back(sprite);
        }
        return sprites;
    }

    const SpriteHeader & findSprite(unsigned int index){
        for (vector<SpriteHeader>::iterator it = sprites.begin(); it != sprites.end(); it++){
            const SpriteHeader & sprite = *it;
            if (sprite.index == index){
                return sprite;
            }
        }
        throw exception();
    }

    Graphics::Bitmap readSprite(int group, int item){
        Storage::LittleEndianReader reader(sffStream);
        for (vector<SpriteHeader>::iterator it = sprites.begin(); it != sprites.end(); it++){
            const SpriteHeader & sprite = *it;
            if (sprite.group == group && sprite.item == item){
                /* Compression formats are consistent across SFF versions. The first
                 * 4 bytes of each compressed block is an integer representing the
                 * length of the data after decompression.
                 */
                if (sprite.dataLength == 0){
                    const SpriteHeader & linked = findSprite(sprite.linked);
                    return readSprite(linked.group, linked.item);
                } else {
                    if (sprite.flags == 0){
                        return read(sprite, reader, ldataOffset + sprite.dataOffset + 4, sprite.dataLength - 4);
                    } else {
                        return read(sprite, reader, tdataOffset + sprite.dataOffset + 4, sprite.dataLength - 4);
                    }
                }
            }
        }
        Global::debug(0) << "Didn't find sprite " << group << ", " << item << endl;
        throw exception();
    }

    Graphics::Bitmap read(const SpriteHeader & sprite, Storage::LittleEndianReader & reader, uint32_t offset, uint32_t length){
        Global::debug(0) << "Read sprite " << sprite.group << ", " << sprite.item << " dimensions " << sprite.width << "x" << sprite.height << endl;
        char * pixels = new char[sprite.width * sprite.height];
        memset(pixels, 0, sprite.width * sprite.height);
        sffStream.seekg(offset, ios::beg);
        try{
            switch (sprite.format){
                case 2: readRLE8(reader, length, pixels, sprite.width * sprite.height); break;
                case 3: readRLE5(reader, length, pixels, sprite.width * sprite.height); break;
                case 4: readLZ5(reader, length, pixels, sprite.width * sprite.height); break;
                default: {
                    Global::debug(0) << "Don't understand format " << sprite.format << endl;
                    throw exception();
                }
            }
        } catch (...){
            Global::debug(0) << "Ignoring error... " << endl;
        }

        map<uint8_t, Graphics::Color> palette = readPalette(sprite.palette);
        Graphics::Bitmap out(sprite.width, sprite.height);
        writePixels(out, pixels, palette);
        delete[] pixels;
        return out;
    }

    map<uint8_t, Graphics::Color> readPalette(unsigned int index){
        for (vector<PaletteHeader>::iterator it = palettes.begin(); it != palettes.end(); it++){
            const PaletteHeader & palette = *it;
            if (palette.index == index){
                if (palette.length != 0){
                    return readPalette(palette);
                } else {
                    return readPalette(palette.linked);
                }
            }
        }

        throw exception();
    }

    map<uint8_t, Graphics::Color> readPalette(const PaletteHeader & palette){
        sffStream.seekg(palette.offset + ldataOffset, ios::beg);
        uint8_t * data = new uint8_t[palette.length];
        sffStream.read((char*) data, palette.length);
        map<uint8_t, Graphics::Color> out;
        for (int color = 0; color < palette.colors; color++){
            /* Palette data is stored in 4 byte chunks per color.
             * The first 3 bytes correspond to 8-bit values for RGB color, and
             * the last byte is unused (set to 0).
             */
            int red = data[color * 4];
            int green = data[color * 4 + 1];
            int blue = data[color * 4 + 2];
            out[color] = Graphics::makeColor(red, green, blue);
        }
        delete[] data;
        return out;
    }

    /* pixels are an index into a palette */
    void writePixels(Graphics::Bitmap & out, char * pixels, map<uint8_t, Graphics::Color> & palette){
        for (int y = 0; y < out.getHeight(); y++){
            for (int x = 0; x < out.getWidth(); x++){
                uint8_t pixel = pixels[x + y * out.getWidth()];
                out.putPixel(x, y, palette[pixel]);
            }
        }
    }

    void readRLE8(Storage::LittleEndianReader & reader, uint32_t length, char * pixels, int pixelLength){
        char * output = pixels;
        while (length > 0){
            uint8_t rle = reader.readByte1();
            length -= 1;
            if ((rle & 0xc0) == 0x40){
                uint8_t color = reader.readByte1();
                length -= 1;
                int runlength = (rle & 0x3f);
                for (int i = 0; i < runlength; i++){
                    *output = color;
                    output += 1;
                }
            } else {
                *output = rle;
                output += 1;
            }
        }
    }

    void readRLE5(Storage::LittleEndianReader & reader, uint32_t length, char * pixels, int pixelLength){
            /*
RLE5packet = read(2 bytes)
if RLE5packet.color_bit is 1, then
color = read(1 byte)
else
color = 0
for run_count from 0 to RLE5packet.run_length, do
output(color)
Decode 3RL/5VAL
for bytes_processed from 0 to RLE5packet.data_length - 1, do
one_byte = read(1 byte)
color = one_byte & 0x1F
run_length = one_byte >> 5
for run_count from 0 to run_length, do
output(color)
*/
        char * output = pixels;
        while (length > 0){
            uint16_t packet = reader.readByte2();
            int runlength = packet & 0xff;
            length -= 2;
            int color = 0;
            if ((packet & (1 << 15)) == (1 << 15)){
                color = reader.readByte1();
                length -= 1;
            }
            int data = (packet >> 8) & 0x7f;
            for (int i = 0; i < runlength; i++){
                *output = color;
                output += 1;
            }
            for (int i = 0; i < data; i++){
                uint8_t rle = reader.readByte1();
                length -= 1;
                color = rle & 0x1f;
                runlength = rle >> 5;
                for (int c = 0; c < runlength; c++){
                    *output = color;
                    output += 1;
                }
            }
        }
    }

    enum LZ5PacketType{
        RLEShort,
        RLELong,
        LZ5Short,
        LZ5Long
    };

    struct LZ5Packet{
        LZ5Packet(LZ5PacketType type, int pixel, int length):
        type(type), length(length){
            data.pixel = pixel;
        }

        LZ5Packet(const LZ5Packet & copy):
        type(copy.type), length(copy.length){
            data.pixel = copy.data.pixel;
        }

        LZ5PacketType type;
        union{
            int pixel; /* for rle */
            int offset; /* for lz5 */
        } data;
        int length;
    };

    /* get the stream of packets */
    vector<LZ5Packet> readLZ5Packets(Storage::LittleEndianReader & reader, uint32_t length){
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

            ostream & out = Global::debug(1);
            for (int packet = 0; packet < 8 && total < length; packet++){
                /* I am 99.999% sure that 0 in the control packet means RLE */
                if ((control & (1 << packet)) == 0){
                    /* RLE */
                    if ((compressed[total] >> 5) == 0){
                        out << "Long RLE ";
                        int color = compressed[total] & 31;
                        total += 1;
                        int times = compressed[total] + 8;
                        total += 1;
                        packets.push_back(LZ5Packet(RLELong, color, times));
                    } else {
                        out << "Short RLE ";
                        int color = compressed[total] & 31;
                        int times = compressed[total] >> 5;
                        total += 1;
                        packets.push_back(LZ5Packet(RLEShort, color, times));
                    }
                } else {
                    /* LZ5 */
                    if ((compressed[total] & 63) != 0){
                        int byte1 = compressed[total];
                        int byte2 = 0;
                        /* check if this is the 4th short lz5 packet */
                        // recycled = (compressed[total] & 0xc0) >> (2 * lz5ShortCount);
                        recycled = (recycled << 2) | (compressed[total] >> 6);
                        if (lz5ShortCount == 3){
                            out << "Short LZ5* ";
                            lz5ShortCount = 0;
                            // recycled = (recycled << 2) | (compressed[total] >> 6);
                            byte2 = recycled;
                            recycled = 0;
                            total += 1;
                        } else {
                            out << "Short LZ5 ";
                            // recycled = (recycled << 2) | (compressed[total] >> 6);
                            byte2 = compressed[total + 1];
                            lz5ShortCount += 1;
                            total += 2;
                        }

                        int length = (byte1 & 63) + 1;
                        int offset = byte2 + 1;
                        packets.push_back(LZ5Packet(LZ5Short, offset, length));
                    } else {
                        // int offset = compressed[total] >> 6;
                        int offset = compressed[total] << 2;
                        total += 1;
                        offset |= compressed[total];
                        offset += 1;
                        // offset = (offset << 8) + compressed[total] + 1;
                        // offset = ((offset << 2) | compressed[total]) + 1;
                        total += 1;
                        int length = compressed[total] + 3;
                        packets.push_back(LZ5Packet(LZ5Long, offset, length));
                        total += 1;
                        out << "Long LZ5 ";
                    }
                }
            }
            out << endl;
        }

        delete[] compressed;
        
        return packets;
    }

    void readLZ5(Storage::LittleEndianReader & reader, uint32_t length, char * pixels, int maxPixelLength){
        vector<LZ5Packet> packets = readLZ5Packets(reader, length);
        char * dest = pixels;
        int maxLength = 0;
        int total = 0;
        Global::debug(1) << packets.size() << " packets " << maxPixelLength << " pixels" << endl;
        for (vector<LZ5Packet>::iterator it = packets.begin(); it != packets.end(); it++){
            const LZ5Packet & packet = *it;
            int index = (it - packets.begin());
            if (packet.length > maxLength){
                Global::debug(1) << "packet " << index << " " << packet.type << " length was " << packet.length << endl;
                maxLength = packet.length;
            }
            // Global::debug(0) << "Wrote " << total << " packets. Writing " << packet.length << endl;
            total += packet.length;
            switch (packet.type){
                case RLEShort:
                case RLELong: {
                    for (int i = 0; i < packet.length; i++){
                        if (dest - pixels >= maxPixelLength){
                            Global::debug(0) << "packet " << index << " tried to write too many pixels! length " << packet.length << " Packets left " << (packets.end() - it) << endl;
                            throw exception();
                        }
                        *dest = packet.data.pixel;
                        dest += 1;
                    }
                    break;
                }
                case LZ5Short:
                case LZ5Long: {
                    char * source = dest - packet.data.offset;
                    if (packet.data.offset < 0){
                        Global::debug(0) << "LZ5 offset is negative " << packet.data.offset << endl;
                        throw exception();
                    }
                    if (source < pixels){
                        Global::debug(0) << "Packet " << index << " type (" << packet.type << ") Source is beneath pixels by " << (pixels - source) << ". dest at " << (dest - pixels) << " length " << packet.length << " offset " << packet.data.offset << endl;
                        throw exception();
                    }
                    for (int i = 0; i < packet.length; i++){
                        if (dest - pixels >= maxPixelLength){
                            Global::debug(0) << "packet " << index << " tried to write too many pixels! length " << packet.length << " Packets left " << (packets.end() - it) << endl;
                            throw exception();
                        }
                        *dest = *source;
                        dest += 1;
                        source += 1;
                    }
                    break;
                }
            }
        }
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
    vector<PaletteHeader> palettes;

    int filesize;
    int location;
    uint32_t totalImages;

    uint32_t ldataOffset;
    uint32_t ldataLength;
    uint32_t tdataOffset;
    uint32_t tdataLength;
};

Graphics::Bitmap getSff(const char * path, int index){
    ifstream in(path);
    SFF2_FileHeader file(in);
    SprNode * node = file.GetSprNode(in, index);
    uint8_t * pixels = file.GetSprite(in, node);
    Graphics::Bitmap out(node->GetWidth(), node->GetHeight());
    map<uint8_t, Graphics::Color> palette;
    PalNode * paletteNode = file.GetPalNode(in, node->GetPalInd());
    for (int y = 0; y < node->GetHeight(); y++){
        for (int x = 0; x < node->GetWidth(); x++){
            uint8_t pixel = pixels[x + y * out.getWidth()];
            if (palette.find(pixel) == palette.end()){
                colorinfo * color = file.GetColor(in, paletteNode, pixel);
                if (color != NULL){
                    palette[pixel] = Graphics::makeColor(color->r, color->g, color->b);
                }
                delete color;
            }
                // Global::debug(0) << x << ", " << y << " " << (int) pixel << " is " << Graphics::makeColor(color->r, color->g, color->b) << endl;
            out.putPixel(x, y, palette[pixel]);
            /*
            if (palette.find(pixel) == palette.end()){
                palette[pixel] = Graphics::makeColor(Util::rnd(128) + 128,
                                                     Util::rnd(128) + 128,
                                                     Util::rnd(128) + 128);
            }
            out.putPixel(x, y, palette[pixel]);
            */
        }
    }
    delete paletteNode;
    delete node;
    delete[] pixels;
    return out;
}

int main(int argc, char ** argv){
    Global::debug(0) << "Sffv2 reader" << endl;
    if (argc > 1){
        try{
            Global::init(Global::WINDOWED);
            Keyboard::pushRepeatState(true);
            Filesystem::AbsolutePath path(argv[1]);
            SffV2Reader reader(path);
            
            vector<SffV2Reader::SpriteHeader> sprites = reader.getSprites();
            unsigned int index = 0;
            InputManager manager;
            InputMap<int> input;
            input.set(Keyboard::Key_ESC, 0, true, 0);
            input.set(Keyboard::Key_LEFT, 0, true, 1);
            input.set(Keyboard::Key_RIGHT, 0, true, 2);
            Graphics::Bitmap buffer(640, 480);
            bool done = false;
            while (!done){
                buffer.clear();
                const SffV2Reader::SpriteHeader & sprite = sprites[index];
                reader.readSprite(sprite.group, sprite.item).draw(5, 5, buffer);
                getSff(argv[1], sprite.index).draw(300, 5, buffer);
                buffer.BlitToScreen();

                bool ok = false;
                while (!ok){
                    InputManager::poll();
                    vector<InputMap<int>::InputEvent> events = InputManager::getEvents(input, InputSource(true));
                    for (vector<InputMap<int>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
                        InputMap<int>::InputEvent event = *it;

                        if (!event.enabled){
                            continue;
                        }

                        if (event.out == 1){
                            if (index > 0){
                                index -= 1;
                                ok = true;
                            }
                        }

                        if (event.out == 2){
                            if (index < sprites.size() - 1){
                                index += 1;
                                ok = true;
                            }
                        }

                        if (event.out == 0){
                            ok = true;
                            done = true;
                        }
                    }

                    if (!ok){
                        Util::rest(1);
                    }
                }
            }

            /*
            Graphics::Bitmap what = reader.readSprite(1400, 7);
            Graphics::Bitmap buffer(640, 480);
            buffer.clear();
            what.draw(5, 5, buffer);
            buffer.BlitToScreen();
            InputManager manager;
            InputMap<int> blah;
            blah.set(Keyboard::Key_ESC, 0, true, 0);
            manager.waitForPress(blah, 0);
            */
        } catch (...){
        }
    } else {
        printf("Give a .sff file as an argument\n");
    }
}
