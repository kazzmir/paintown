#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

namespace Bor{

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

    virtual void seekEnd(int where){
        stream.seekg(where, ios::end);
    }

    virtual void seek(int where){
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
            uint8_t byte;
            stream >> byte;
            bytes.push_back(byte);
        }
        return bytes;
    }

    ifstream & stream;
};

class LittleEndianReader: public EndianReader {
public:
    LittleEndianReader(ifstream & stream):
    EndianReader(stream){
    }
protected:
    virtual int32_t convert(const vector<uint8_t> & bytes){
        int32_t out = 0;
        for (vector<uint8_t>::const_reverse_iterator it = bytes.rbegin(); it != bytes.rend(); it++){
            out = (out << 8) + *it;
        }
        return out;
    }
};

class BigEndianReader: public EndianReader {
public:
    BigEndianReader(ifstream & stream):
    EndianReader(stream){
    }
protected:
    virtual int32_t convert(const vector<uint8_t> & bytes){
        int32_t out = 0;
        for (vector<uint8_t>::const_iterator it = bytes.begin(); it != bytes.end(); it++){
            out = (out << 8) + *it;
        }
        return out;
    }
};

/* Reads Bor/Openbor packfiles */
class PackReader{
public:
    PackReader(const string & filename){
        ifstream stream;
        stream.open(filename.c_str(), std::ios::in | std::ios::binary);
        LittleEndianReader reader(stream);
        uint32_t magic = reader.readByte4();
        if (magic != MAGIC){
            cout << "Not a packfile! " << std::hex << magic << endl;
        } else {
            cout << "Ok got a packfile" << endl;
        }
        uint32_t version = reader.readByte4();
        reader.seekEnd(-4);
        uint32_t headerPosition = reader.readByte4();
        reader.seek(headerPosition);

        cout << "Header at " << headerPosition << endl;

        bool done = false;
        while (!done){
            uint32_t current = reader.position();
            uint32_t length = reader.readByte4();
            uint32_t start = reader.readByte4();
            uint32_t size = reader.readByte4();
            string name = reader.readString(80);
            done = name.size() == 0;
            cout << name << " at " << start << " size " << size << endl;
            cout << " seek to " << (current + length) << endl;
            reader.seek(current + length);
            done |= stream.eof();
        }
    }

private:
    static const uint32_t MAGIC = 0x4B434150;
};

}

int main(int argc, char ** argv){
    Bor::PackReader reader(argv[1]);
}
