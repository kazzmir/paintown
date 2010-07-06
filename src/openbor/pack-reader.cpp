#include <iostream>
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

protected:
    virtual int32_t convert(const vector<char> & bytes) = 0;

    vector<char> readBytes(int length){
        vector<char> bytes;
        for (int i = 0; i < length; i++){
            char byte;
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
    virtual int32_t convert(const vector<char> & bytes){
        int32_t out = 0;
        for (vector<char>::const_reverse_iterator it = bytes.rbegin(); it != bytes.rend(); it++){
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
    virtual int32_t convert(const vector<char> & bytes){
        int32_t out = 0;
        for (vector<char>::const_iterator it = bytes.begin(); it != bytes.end(); it++){
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
    }

private:
    static const uint32_t MAGIC = 0x4B434150;
};

};

int main(int argc, char ** argv){
    Bor::PackReader reader(argv[1]);
}
