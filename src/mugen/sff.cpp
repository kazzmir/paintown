#include "util/file-system.h"
#include "util/bitmap.h"
#include "util/pointer.h"

#include "util.h"
#include "sprite.h"

#include <sstream>
#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

namespace Mugen{
    namespace Util{

class SffReaderInterface{
public:
    SffReaderInterface(){
    }

    virtual ~SffReaderInterface(){
    }

    virtual bool moreSprites() = 0;
    virtual PaintownUtil::ReferenceCount<Mugen::Sprite> readSprite(bool mask) = 0;
    virtual PaintownUtil::ReferenceCount<Mugen::Sprite> findSprite(int group, int item, bool mask) = 0;
};

class SffReader: public SffReaderInterface {
public:
    SffReader(const Filesystem::AbsolutePath & filename, const Filesystem::AbsolutePath & palette):
    filename(filename),
    currentSprite(0){
        /* Must read the palette first because once the sff file is opened
         * we can't open the same zip file twice.
         */
        // Palette related
        useact = false;

        //unsigned char colorsave[3]; // rgb pal save

        memset(palsave1, 0, sizeof(palsave1));

        // Load in first palette
        if (readPalette(palette, palsave1)){
            useact = true;
        }

        /* 16 skips the header stuff */
        sffStream = Storage::instance().open(filename);
        if (!sffStream){
            throw MugenException("Could not open SFF file: '" + filename.path() + "'", __FILE__, __LINE__);
        }

        filesize = sffStream->getSize();

        checkVersion(sffStream);

        /* Data starts at the 16th byte.
         */
        location = 16;
        sffStream->seek(location, SEEK_SET);
        /* FIXME: change these to uint32 or whatever */
        uint32_t totalGroups = 0;
        totalImages = 0;
        uint32_t suboffset = 0;
        uint32_t subhead = 0;
        uint32_t sharedPal = 0;

        Storage::LittleEndianReader reader(sffStream);

        totalGroups = reader.readByte4();
        totalImages = reader.readByte4();
        suboffset = reader.readByte4();
        subhead = reader.readByte4();
        sharedPal = reader.readByte1();

        if (sharedPal && sharedPal != 1){
            sharedPal = 0;
        }

        location = suboffset;

        if (location < 512 || location > 2147482717 ){
            location = 512;
        }

        Global::debug(2) << "Got Total Groups: " << totalGroups << ", Total Images: " << totalImages << ", Next Location in file: " << location << std::endl;

        // spriteIndex = new Mugen::Sprite*[totalImages + 1];
    }

    virtual void checkVersion(const PaintownUtil::ReferenceCount<Storage::File> & stream){
        Storage::LittleEndianReader reader(stream);
        string signature = reader.readString2(12);
        if (signature != "ElecbyteSpr"){
            std::ostringstream out;
            out << "Invalid signature. Got '" << signature << "'";
            throw MugenException(out.str(), __FILE__, __LINE__);
        }

        int versionLo3 = reader.readByte1();
        int versionLo2 = reader.readByte1();
        int versionLo1 = reader.readByte1();
        int versionHi = reader.readByte1();
        if (versionHi != 1){
            std::ostringstream out;
            out << "Invalid version for SFFv1. Got " << versionHi << "." << versionLo1 << "." << versionLo2 << "." << versionLo3;
            throw MugenException(out.str(), __FILE__, __LINE__);
        }
    }

    virtual ~SffReader(){
        // sffStream.close();
        // delete[] spriteIndex;
    }

    /* gets all the sprite headers without loading the pcx information */
    void quickReadSprites(bool mask){
        for (unsigned int index = 0; index < totalImages; index++){
            PaintownUtil::ReferenceCount<Mugen::SpriteV1> sprite = PaintownUtil::ReferenceCount<Mugen::SpriteV1>(new Mugen::SpriteV1(mask));
            sprite->read(sffStream, location);
            spriteIndex[index] = sprite;
            location = sprite->getNext();
        }
    }

    /* Actually loads the pcx data */
    PaintownUtil::ReferenceCount<Mugen::SpriteV1> loadSprite(PaintownUtil::ReferenceCount<Mugen::SpriteV1> sprite, bool mask){
        if (!sprite->isLoaded()){
            if (sprite->getLength() == 0){
                const PaintownUtil::ReferenceCount<Mugen::SpriteV1> temp = loadSprite(spriteIndex[sprite->getPrevious()], mask);
                if (!temp){
                    std::ostringstream out;
                    out << "Unknown linked sprite " << sprite->getPrevious();
                    throw MugenException(out.str(), __FILE__, __LINE__);
                }
                sprite->copyImage(temp);
            } else {
                bool islinked = false;
                sprite->loadPCX(sffStream, islinked, useact, palsave1, mask);
            }
        }
        return sprite;
    }

    PaintownUtil::ReferenceCount<Mugen::Sprite> findSprite(int group, int item, bool mask){
        if (spriteIndex.size() == 0){
            quickReadSprites(mask);
        }
        for (map<int, PaintownUtil::ReferenceCount<Mugen::SpriteV1> >::iterator it = spriteIndex.begin(); it != spriteIndex.end(); it++){
            PaintownUtil::ReferenceCount<Mugen::SpriteV1> sprite = it->second;
            if (sprite->getGroupNumber() == group &&
                sprite->getImageNumber() == item){
                /* make a deep copy */
                return PaintownUtil::ReferenceCount<Mugen::SpriteV1>(new Mugen::SpriteV1(*loadSprite(sprite, mask))).convert<Mugen::Sprite>();
            }
        }
        return PaintownUtil::ReferenceCount<Mugen::Sprite>(NULL);
    }

    PaintownUtil::ReferenceCount<Mugen::Sprite> readSprite(bool mask){
        bool islinked = false;
        if (location > filesize){
            std::ostringstream out;
            out << "Error in SFF file: " << filename.path() << ". Offset of image (" << location << ") beyond the end of the file (" << filesize << ").";
            throw MugenException(out.str(), __FILE__, __LINE__);
        }

        PaintownUtil::ReferenceCount<Mugen::SpriteV1> sprite = PaintownUtil::ReferenceCount<Mugen::SpriteV1>(new Mugen::SpriteV1(mask));
        sprite->read(sffStream, location);
        location = sprite->getNext();

        if (sprite->getLength() == 0){
            const PaintownUtil::ReferenceCount<Mugen::SpriteV1> temp = spriteIndex[sprite->getPrevious()];
            if (!temp){
                std::ostringstream out;
                out << "Unknown linked sprite " << sprite->getPrevious();
                throw MugenException(out.str(), __FILE__, __LINE__);
            }
            sprite->copyImage(temp);
        } else {
            sprite->loadPCX(sffStream, islinked, useact, palsave1, mask);
        }
            
        spriteIndex[currentSprite] = sprite;
        currentSprite += 1;

        return sprite.convert<Mugen::Sprite>();
    }

    bool moreSprites(){
        return currentSprite < totalImages;
    }

protected:
    const Filesystem::AbsolutePath filename;
    PaintownUtil::ReferenceCount<Storage::File> sffStream;
    unsigned long currentSprite;
    int totalSprites;
    map<int, PaintownUtil::ReferenceCount<Mugen::SpriteV1> > spriteIndex;
    bool useact;
    int filesize;
    int location;
    uint32_t totalImages;
    unsigned char palsave1[768]; // First image palette
};

class SffV2Reader: public SffReaderInterface {
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
        /* 16 skips the header stuff */
        sffStream = Storage::instance().open(filename);
        if (!sffStream){
            throw MugenException("Could not open SFF file: '" + filename.path() + "'", __FILE__, __LINE__);
        }

        filesize = sffStream->getSize();

        totalImages = 0;

        Storage::LittleEndianReader reader(sffStream);
        string signature = reader.readString2(12);
        if (signature != "ElecbyteSpr"){
            throw MugenException(string("Invalid signature for SFFv2 file. Got '") + signature + "'", __FILE__, __LINE__);
        }

        int versionLo3 = reader.readByte1();
        int versionLo2 = reader.readByte1();
        int versionLo1 = reader.readByte1();
        int versionHi = reader.readByte1();
        if (versionHi != 2){
            std::ostringstream out;
            out << "Invalid version for SFFv2. Got " << versionHi << "." << versionLo1 << "." << versionLo2 << "." << versionLo3;
            throw MugenException(out.str(), __FILE__, __LINE__);
        }

        /* skip reserved */
        reader.readByte4();
        reader.readByte4();

        /* ignore compatability version number */
        reader.readByte4();

        /* skip reserved */
        reader.readByte4();
        reader.readByte4();

        uint32_t suboffset = reader.readByte4();
        totalImages = reader.readByte4();
        uint32_t subpalette = reader.readByte4();
        uint32_t totalPalettes = reader.readByte4();
        
        Global::debug(1) << "Image offset " << suboffset << " total images " << totalImages << " palette offset " << subpalette << " total palettes " << totalPalettes << std::endl;

        /* compressed data */
        ldataOffset = reader.readByte4();
        ldataLength = reader.readByte4();

        /* uncompressed data */
        tdataOffset = reader.readByte4();
        tdataLength = reader.readByte4();

        Global::debug(1) << "Ldata offset " << ldataOffset << " length " << ldataLength << std::endl;
        Global::debug(1) << "Tdata offset " << tdataOffset << " length " << tdataLength << std::endl;

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
            Global::debug(1) << " " << group << ", " << item << " " << width << "x" << height << " format " << formatName(format) << " color depth " << (int) colorDepth << " data " << dataOffset << " length " << dataLength << (flags == 0 ? " literal" : " translate") << std::endl;
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
    }

    vector<SpriteHeader> getSprites(){
        return this->sprites;
    }

    const SpriteHeader & findSpriteHeader(unsigned int index){
        for (vector<SpriteHeader>::iterator it = sprites.begin(); it != sprites.end(); it++){
            const SpriteHeader & sprite = *it;
            if (sprite.index == index){
                return sprite;
            }
        }
        std::ostringstream out;
        out << "Could not find a sprite with index " << index;
        throw MugenException(out.str(), __FILE__, __LINE__);
    }

    const SpriteHeader & findSpriteHeader(int group, int item){
        for (vector<SpriteHeader>::iterator it = sprites.begin(); it != sprites.end(); it++){
            const SpriteHeader & sprite = *it;
            if (sprite.group == group && sprite.item == item){
                return sprite;
            }
        }
        std::ostringstream out;
        out << "No sprite for group " << group << ", " << item;
        throw MugenException(out.str(), __FILE__, __LINE__);
    }

    PaintownUtil::ReferenceCount<Mugen::Sprite> readSprite(const SpriteHeader & sprite, bool mask){
        /* FIXME: do something with mask */
        return PaintownUtil::ReferenceCount<Mugen::SpriteV2>(new Mugen::SpriteV2(readBitmap(sprite), sprite.group, sprite.item, sprite.axisx, sprite.axisy)).convert<Mugen::Sprite>();
    }

    PaintownUtil::ReferenceCount<Mugen::Sprite> readSprite(bool mask){
        if (currentSprite < totalImages){
            int here = currentSprite;
            currentSprite += 1;
            return readSprite(sprites[here], mask);
        }

        return PaintownUtil::ReferenceCount<Mugen::Sprite>(NULL);
    }
    
    virtual PaintownUtil::ReferenceCount<Mugen::Sprite> findSprite(int group, int item, bool mask){
        return readSprite(findSpriteHeader(group, item), mask);
    }

    Graphics::Bitmap readBitmap(const SpriteHeader & sprite){
        Storage::LittleEndianReader reader(sffStream);
        if (sprite.dataLength == 0){
            return readBitmap(findSpriteHeader(sprite.linked));
        } else {
            /* Compression formats are consistent across SFF versions. The first
             * 4 bytes of each compressed block comprises an integer representing
             * the length of the data after decompression.
             */
            if (sprite.flags == 0){
                return read(sprite, reader, ldataOffset + sprite.dataOffset + 4, sprite.dataLength - 4);
            } else {
                return read(sprite, reader, tdataOffset + sprite.dataOffset + 4, sprite.dataLength - 4);
            }
        }
        
        /* Pretty sure we can't get here */
        throw MugenException("Internal error", __FILE__, __LINE__);
    }

    Graphics::Bitmap read(const SpriteHeader & sprite, Storage::LittleEndianReader & reader, uint32_t offset, uint32_t length){
        // Global::debug(0) << "Read sprite " << sprite.group << ", " << sprite.item << " dimensions " << sprite.width << "x" << sprite.height << std::endl;
        char * pixels = new char[sprite.width * sprite.height];
        memset(pixels, 0, sprite.width * sprite.height);
        sffStream->seek(offset, SEEK_SET);
        try{
            switch (sprite.format){
                case 2: readRLE8(reader, length, pixels, sprite.width * sprite.height); break;
                case 3: readRLE5(reader, length, pixels, sprite.width * sprite.height); break;
                case 4: readLZ5(reader, length, pixels, sprite.width * sprite.height); break;
                default: {
                    std::ostringstream out;
                    out << "Don't understand SffV2 format " << sprite.format;
                    throw MugenException(out.str(), __FILE__, __LINE__);
                }
            }
        } catch (...){
            Global::debug(1) << "Ignoring Sffv2 sprite error... " << std::endl;
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

        std::ostringstream out;
        out << "Could not find palette with index " << index;
        throw MugenException(out.str(), __FILE__, __LINE__);
    }

    map<uint8_t, Graphics::Color> readPalette(const PaletteHeader & palette){
        if (paletteCache.find(palette.index) != paletteCache.end()){
            return paletteCache[palette.index];
        } else {
            sffStream->seek(palette.offset + ldataOffset, SEEK_SET);
            uint8_t * data = new uint8_t[palette.length];
            sffStream->readLine((char*) data, palette.length);
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
            paletteCache[palette.index] = out;
            return out;
        }
    }

    /* pixels are an index into a palette */
    void writePixels(Graphics::Bitmap & out, char * pixels, map<uint8_t, Graphics::Color> & palette){
        int height = out.getHeight();
        int width = out.getWidth();
        Graphics::Color * lookup = new Graphics::Color[palette.size()];
        for (map<uint8_t, Graphics::Color>::iterator it = palette.begin(); it != palette.end(); it++){
            lookup[it->first] = it->second;
        }
        for (int y = 0; y < height; y++){
            for (int x = 0; x < width; x++){
                uint8_t pixel = pixels[x + y * width];
                out.putPixel(x, y, lookup[pixel]);
            }
        }
        delete[] lookup;
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

            /* FIXME: remove 'out' because it probably hurts performance */
            std::ostream & out = Global::debug(1);
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
            out << std::endl;
        }

        delete[] compressed;
        
        return packets;
    }

    void readLZ5(Storage::LittleEndianReader & reader, uint32_t length, char * pixels, int maxPixelLength){
        vector<LZ5Packet> packets = readLZ5Packets(reader, length);
        char * dest = pixels;
        int maxLength = 0;
        int total = 0;
        Global::debug(1) << packets.size() << " packets " << maxPixelLength << " pixels" << std::endl;
        for (vector<LZ5Packet>::iterator it = packets.begin(); it != packets.end(); it++){
            const LZ5Packet & packet = *it;
            int index = (it - packets.begin());
            if (packet.length > maxLength){
                Global::debug(1) << "packet " << index << " " << packet.type << " length was " << packet.length << std::endl;
                maxLength = packet.length;
            }
            // Global::debug(0) << "Wrote " << total << " packets. Writing " << packet.length << endl;
            total += packet.length;
            switch (packet.type){
                case RLEShort:
                case RLELong: {
                    for (int i = 0; i < packet.length; i++){
                        if (dest - pixels >= maxPixelLength){
                            std::ostringstream out;
                            out << "packet " << index << " tried to write too many pixels! length " << packet.length << " Packets left " << (packets.end() - it);
                            throw MugenException(out.str(), __FILE__, __LINE__);
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
                        std::ostringstream out;
                        out << "LZ5 offset is negative " << packet.data.offset;
                        throw MugenException(out.str(), __FILE__, __LINE__);
                    }
                    if (source < pixels){
                        std::ostringstream out;
                        out << "Packet " << index << " type (" << packet.type << ") Source is beneath pixels by " << (pixels - source) << ". dest at " << (dest - pixels) << " length " << packet.length << " offset " << packet.data.offset;
                        throw MugenException(out.str(), __FILE__, __LINE__);
                    }
                    for (int i = 0; i < packet.length; i++){
                        if (dest - pixels >= maxPixelLength){
                            std::ostringstream out;
                            out << "packet " << index << " tried to write too many pixels! length " << packet.length << " Packets left " << (packets.end() - it);
                            throw MugenException(out.str(), __FILE__, __LINE__);
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
    }

    bool moreSprites(){
        return currentSprite < totalImages;
    }

protected:
    const Filesystem::AbsolutePath filename;
    PaintownUtil::ReferenceCount<Storage::File> sffStream;
    unsigned long currentSprite;
    int totalSprites;
    vector<SpriteHeader> sprites;
    vector<PaletteHeader> palettes;

    int filesize;
    int location;
    uint32_t totalImages;

    uint32_t ldataOffset;
    uint32_t ldataLength;
    uint32_t tdataOffset;
    uint32_t tdataLength;
    
    map< int, map<uint8_t, Graphics::Color> > paletteCache;
};

/* Looks for a txt file in the container that describes the sprites. The format
 * is as follows
 *
 * group# item# axisX# axisY# filename
 * group# item# axisX# axisY# filename
 * ...
 *
 */
class ImageContainerReader: public SffReaderInterface {
public:
    struct Image{
        Image(int group, int item, int axisX, int axisY, string file):
        group(group),
        item(item),
        axisX(axisX),
        axisY(axisY),
        file(file){
        }

        int group;
        int item;
        int axisX;
        int axisY;
        string file;
    };

    ImageContainerReader(const Filesystem::AbsolutePath & path):
    container(path),
    current(files.begin()){
        vector<string> files = Storage::instance().containerFileList(path);
        bool ok = false;
        string descriptionFile;
        for (vector<string>::iterator it = files.begin(); it != files.end(); it++){
            if (it->find(".txt") != string::npos){
                ok = true;
                descriptionFile = *it;
                break;
            }
        }

        if (!ok){
            throw MugenException(string("Could not find a description txt file in the container file ") + path.path(), __FILE__, __LINE__);
        }

        mount = Filesystem::AbsolutePath(path.path() + "_mount");
        Storage::instance().addOverlay(path, mount);
        populate(mount, descriptionFile);
    }

    void populate(const Filesystem::AbsolutePath & folder, const string & descriptionFile){
        PaintownUtil::ReferenceCount<Storage::File> data = Storage::instance().open(folder.join(Filesystem::RelativePath(descriptionFile)));
        if (!data->good()){
            throw MugenException(string("Could not read description file ") + descriptionFile, __FILE__, __LINE__);
        }
        int size = data->getSize();
        if (size > 0){
            char * raw = new char[size+1];
            /* Just to make sure the string ends properly */
            raw[size] = '\0';
            data->readLine(raw, size);
            std::istringstream input;
            input.str(string(raw));
            // Global::debug(0) << "Raw '" << raw << "'" << std::endl;
            while (!input.eof()){
                int group = -99;
                int item = -99;
                int axisx = 0;
                int axisy = 0;
                string filename;
                input >> group >> item >> axisx >> axisy >> filename;
                if (group == -99){
                    break;
                } else {
                    files.push_back(Image(group, item, axisx, axisy, filename));
                }
            }
            delete[] raw;
        }

        current = files.begin();
    }

    virtual ~ImageContainerReader(){
        Storage::instance().removeOverlay(container, mount);
    }

    virtual bool moreSprites(){
        return current != files.end();
    }

    Graphics::Bitmap readBitmap(const string & path){
        PaintownUtil::ReferenceCount<Storage::File> file = Storage::instance().open(mount.join(Filesystem::RelativePath(path)));
        return Graphics::Bitmap(*file);
    }

    virtual PaintownUtil::ReferenceCount<Mugen::Sprite> readSprite(bool mask){
        if (current == files.end()){
            return PaintownUtil::ReferenceCount<Mugen::Sprite>(NULL);
        }
        const Image & image = *current;
        current++;
        /* FIXME: do something with mask */
        return PaintownUtil::ReferenceCount<Mugen::SpriteV2>(new Mugen::SpriteV2(readBitmap(image.file), image.group, image.item, image.axisX, image.axisY)).convert<Mugen::Sprite>();
    }

    virtual PaintownUtil::ReferenceCount<Mugen::Sprite> findSprite(int group, int item, bool mask){
        for (vector<Image>::iterator it = files.begin(); it != files.end(); it++){
            const Image & image = *it;
            if (image.group == group && image.item == item){
                /* FIXME: do something with mask */
                return PaintownUtil::ReferenceCount<Mugen::SpriteV2>(new Mugen::SpriteV2(readBitmap(image.file), image.group, image.item, image.axisX, image.axisY)).convert<Mugen::Sprite>();
            }
        }
        return PaintownUtil::ReferenceCount<Mugen::Sprite>(NULL);
    }

protected:
    Filesystem::AbsolutePath container;
    Filesystem::AbsolutePath mount;
    std::vector<Image> files;
    std::vector<Image>::iterator current;
};

static int majorVersion(const Filesystem::AbsolutePath & filename){
    PaintownUtil::ReferenceCount<Storage::File> stream = Storage::instance().open(filename);
    if (!stream){
        throw MugenException("Could not open SFF file: '" + filename.path() + "'", __FILE__, __LINE__);
    }

    Storage::LittleEndianReader reader(stream);
    string signature = reader.readString2(12);
    if (signature != "ElecbyteSpr"){
        throw MugenException(string("Invalid signature for SFF file. Got '") + signature + "'", __FILE__, __LINE__);
    }

    int versionLo3 = reader.readByte1();
    int versionLo2 = reader.readByte1();
    int versionLo1 = reader.readByte1();
    int versionHi = reader.readByte1();

    return versionHi;
}

static bool isSffv1(const Filesystem::AbsolutePath & filename){
    try{
        return majorVersion(filename) == 1;
    } catch (const MugenException & fail){
        return false;
    }
}

static bool isSffv2(const Filesystem::AbsolutePath & filename){
    try{
        return majorVersion(filename) == 2;
    } catch (const MugenException & fail){
        return false;
    }
}

static PaintownUtil::ReferenceCount<SffReaderInterface> getSffReader(const Filesystem::AbsolutePath & filename, const Filesystem::AbsolutePath & palette){
    if (isSffv1(filename)){
        return PaintownUtil::ReferenceCount<SffReaderInterface>(new SffReader(filename, palette));
    }
    if (isSffv2(filename)){
        /* FIXME: use palette somehow */
        return PaintownUtil::ReferenceCount<SffReaderInterface>(new SffV2Reader(filename));
    }
    if (Storage::isContainer(filename)){
        return PaintownUtil::ReferenceCount<SffReaderInterface>(new ImageContainerReader(filename));
    }

    if (Storage::instance().isDirectory(filename)){
        /* TODO */
    }

    throw MugenException(filename.path() + " is not an sffv1 or sffv2 file", __FILE__, __LINE__);
    return PaintownUtil::ReferenceCount<SffReaderInterface>(NULL);
}

    }
}

void Mugen::Util::readSprites(const Filesystem::AbsolutePath & filename, const Filesystem::AbsolutePath & palette, Mugen::SpriteMap & sprites, bool mask){
    PaintownUtil::ReferenceCount<SffReaderInterface> reader = getSffReader(filename, palette);
    /* where replaced sprites go */
    vector<PaintownUtil::ReferenceCount<Mugen::Sprite> > unused;
    while (reader->moreSprites()){
        // try{
            PaintownUtil::ReferenceCount<Mugen::Sprite> sprite = reader->readSprite(mask);

            Mugen::SpriteMap::iterator first_it = sprites.find(sprite->getGroupNumber());
            if (first_it != sprites.end()){
                std::map< unsigned int, PaintownUtil::ReferenceCount<Mugen::Sprite> >::iterator it = first_it->second.find(sprite->getImageNumber());
                if (it != first_it->second.end()){
                    Global::debug(0) << "Warning: replacing sprite in " << Storage::instance().cleanse(filename).path() << " group " << sprite->getGroupNumber() << " item " << sprite->getImageNumber() << std::endl;
                    unused.push_back(it->second);
                }
            }
            sprites[sprite->getGroupNumber()][sprite->getImageNumber()] = sprite;
        /* 5/5/2012: if a sprite can't be read then throw an error */
        /*
        } catch (const MugenException & e){
            Global::debug(0) << e.getReason() << endl;
        }
        */
    }

    /* delete all replaced sprites */
    /*for (vector< PaintownUtil::ReferenceCount<Mugen::Sprite> >::iterator it = unused.begin(); it != unused.end(); it++){
        delete (*it);
    }*/
}
PaintownUtil::ReferenceCount<Mugen::Sprite> Mugen::Util::probeSff(const Filesystem::AbsolutePath &file, int groupNumber, int spriteNumber, bool mask, const Filesystem::AbsolutePath & actFile){
    PaintownUtil::ReferenceCount<SffReaderInterface> reader = getSffReader(file, actFile);
    PaintownUtil::ReferenceCount<Mugen::Sprite> found = reader->findSprite(groupNumber, spriteNumber, mask);
    if (found != NULL){
        return found;
    }
    std::ostringstream out;
    out << "Could not find sprite " << groupNumber << ", " << spriteNumber << " in " << file.path();
    throw MugenException(out.str(), __FILE__, __LINE__);
}
 
void Mugen::Util::getIconAndPortrait(const Filesystem::AbsolutePath & sffPath, const Filesystem::AbsolutePath & actPath, PaintownUtil::ReferenceCount<Mugen::Sprite> * icon, PaintownUtil::ReferenceCount<Mugen::Sprite> * portrait){
    PaintownUtil::ReferenceCount<SffReaderInterface> reader = getSffReader(sffPath, actPath);
    *icon = reader->findSprite(9000, 0, true);
    *portrait = reader->findSprite(9000, 1, true);
    if (*icon == NULL || *portrait == NULL){
        bool failed_icon = *icon == NULL;
        bool failed_portrait = *portrait == NULL;
        //delete *icon;
        *icon = NULL;
        //delete *portrait;
        *portrait = NULL;
        std::ostringstream out;
        if (failed_icon){
            out << "Could not find sprite " << 9000 << ", " << 0 << " in " << sffPath.path();
        } else {
            out << "Could not find sprite " << 9000 << ", " << 1 << " in " << sffPath.path();
        }
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
}
