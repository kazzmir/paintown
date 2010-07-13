#include "pack-reader.h"

#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "debug.h"

using namespace std;

namespace Bor{

/* Only used in this file */
class Eof: public std::exception {
public:
    Eof(){
    }

    virtual ~Eof() throw (){
    }
};

PackError::PackError(const std::string & file, int line, const std::string & reason):
Exception::Base(file, line),
reason(reason){
}

PackError::~PackError() throw (){
}
    
Exception::Base * PackError::copy() const {
    return new PackError(file, line, reason);
}

const string PackError::getReason() const {
    return reason;
}

class EndianReader{
public:
    EndianReader(ifstream & stream):
    stream(stream){
    }

    virtual int8_t readByte1(){
        return convert(readBytes(sizeof(int8_t)));
    }

    virtual int16_t readByte2(){
        return convert(readBytes(sizeof(int16_t)));
    }

    virtual int32_t readByte4(){
        return convert(readBytes(sizeof(int32_t)));
    }

    virtual string readString(int length){
        ostringstream out;
        uint8_t letter = readByte1();
        while (letter != 0 && length > 0){
            out << letter;
            letter = readByte1();
        }
        return out.str();
    }

    virtual string readString2(int length){
        ostringstream out;
        vector<uint8_t> bytes = readBytes(length);
        for (vector<uint8_t>::iterator it = bytes.begin(); it != bytes.end(); it++){
            char byte = *it;
            if (byte == 0){
                break;
            }
            out << *it;
        }
        return out.str();
    }

    virtual void seekEnd(streamoff where){
        stream.seekg(where, ios::end);
    }

    virtual void seek(streampos where){
        stream.seekg(where);
    }

    virtual int position(){
        return stream.tellg();
    }

protected:
    virtual int32_t convert(const vector<uint8_t> & bytes) = 0;

    vector<uint8_t> readBytes(int length){
        vector<uint8_t> bytes;
        for (int i = 0; i < length; i++){
            uint8_t byte = 0;
            stream.read((char*) &byte, 1);
            if (stream.eof()){
                throw Eof();
            } else {
            }
            bytes.push_back(byte);
        }
        return bytes;
    }

    ifstream & stream;
};

/* combines bytes b0 b1 b2 b3 as b0 + b1*2^8 + b2*2^16 + b3*2^24 */
class LittleEndianReader: public EndianReader {
public:
    LittleEndianReader(ifstream & stream):
    EndianReader(stream){
    }
protected:
    virtual int32_t convert(const vector<uint8_t> & bytes){
        uint32_t out = 0;
        for (vector<uint8_t>::const_reverse_iterator it = bytes.rbegin(); it != bytes.rend(); it++){
            out = (out << 8) + *it;
        }
        return out;
    }
};

/* combines bytes b0 b1 b2 b3 as b0*2^24 + b1*2^16 + b2*2^8 + b3 */
class BigEndianReader: public EndianReader {
public:
    BigEndianReader(ifstream & stream):
    EndianReader(stream){
    }
protected:
    virtual int32_t convert(const vector<uint8_t> & bytes){
        uint32_t out = 0;
        for (vector<uint8_t>::const_iterator it = bytes.begin(); it != bytes.end(); it++){
            out = (out << 8) + *it;
        }
        return out;
    }
};

PackReader::PackReader(const Filesystem::AbsolutePath & filename):
filename(filename){
    ifstream stream;
    Global::debug(0) << "Reading pak file " << filename.path() << endl;
    stream.open(filename.path().c_str(), std::ios::in | std::ios::binary);
    LittleEndianReader reader(stream);
    uint32_t magic = reader.readByte4();
    if (magic != MAGIC){
        ostringstream error;
        error << filename.path() << " is not a packfile! " << std::hex << magic;
        throw PackError(__FILE__, __LINE__, error.str());
    } else {
        // cout << "Ok got a packfile" << endl;
    }
    uint32_t version = reader.readByte4();
    /* just assume we can read all versions for now */

    // cout << "Version is " << version << endl;
    reader.seekEnd(-4);
    uint32_t headerPosition = reader.readByte4();
    reader.seek(headerPosition);

    // cout << "Header at 0x" << std::hex << headerPosition << std::dec << endl;

    bool done = false;
    try{
        while (!done){
            uint32_t current = reader.position();
            uint32_t length = reader.readByte4();
            uint32_t start = reader.readByte4();
            uint32_t size = reader.readByte4();
            /* will read upto 80 bytes, but potentially fewer if a null byte
             * is reached.
             */
            string name = reader.readString(80);
            if (name.size() != 0){
                files[name] = File(start, size);
            }
            done = name.size() == 0;
            // cout << name << " at " << start << " size " << size << " length " << length << endl;
            // cout << " seek to " << (current + length) << endl;
            // reader.seek(current + length);
            done |= stream.eof();
        }
    } catch (const Eof & eof){
    }
    stream.close();

    handle.open(filename.path().c_str(), std::ios::in | std::ios::binary);
}

/* read a blob of bytes */
char * PackReader::readFile(const File & file){
    handle.seekg(file.start);
    char * data = new char[file.length];
    handle.read(data, file.length);
    return data;
}

PackReader::~PackReader(){
    handle.close();
}

}


