#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <exception>

#include <sys/stat.h>

using namespace std;

namespace Bor{

class Eof: public std::exception {
public:
    Eof(){
    }

    virtual ~Eof() throw (){
    }
};

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

/* Reads Bor/Openbor packfiles */
class PackReader{
public:
    struct File{
        File(uint32_t start, uint32_t length):
            start(start),
            length(length){
            }

        File():
            start(0),
            length(0){
            }

        uint32_t start;
        uint32_t length;
    };

    PackReader(const string & filename):
    filename(filename){
        ifstream stream;
        stream.open(filename.c_str(), std::ios::in | std::ios::binary);
        LittleEndianReader reader(stream);
        uint32_t magic = reader.readByte4();
        if (magic != MAGIC){
            cout << "Not a packfile! " << std::hex << magic << endl;
            return;
        } else {
            // cout << "Ok got a packfile" << endl;
        }
        uint32_t version = reader.readByte4();
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
    }

    void mkdirs(string path){
        string dir(path);
        int start = 0;
        int slash = dir.find('/', start);
        // cout << "Dir is " << dir << endl;
        string sofar = ".";
        while (slash != string::npos){
            string part = dir.substr(start, slash - start);
            // cout << "Create dir '" << (sofar + "/" + part) << "'" << endl;
            mkdir((sofar + "/" + part).c_str(), 0775);
            sofar += "/" + part;
            start = slash + 1;
            slash = dir.find('/', start);
        }
        // string part = dir.substr(start);
        // mkdir((sofar + "/" + part).c_str(), 0775);
    }

    string replaceSlashes(string in){
        int place = in.find('\\');
        while (place != string::npos){
            in[place] = '/';
            place = in.find('\\');
        }
        return in;
    }

    string lowercase(string in){
        for (unsigned int i = 0; i < in.length(); i++){
            if (in[i] >= 'A' && in[i] <= 'Z'){
                in[i] = in[i] - 'A' + 'a';
            }
        }
        return in;
    }

    void extract(string where){
        mkdir(where.c_str(), 0775);
        ifstream stream;
        stream.open(filename.c_str(), std::ios::in | std::ios::binary);
        for (std::map<std::string, File>::iterator it = files.begin(); it != files.end(); it++){
            string path = lowercase(replaceSlashes((*it).first));
            // cout << "Path is '" << path << "'" << endl;
            File offset = (*it).second;
            stream.seekg(offset.start);
            char * data = new char[offset.length];
            stream.read(data, offset.length);
            string final = where + "/" + path;
            // cout << "Final is '" << final << "'" << endl;
            mkdirs(final);
            ofstream out(final.c_str(), std::ios::out | std::ios::binary);
            cout << "Writing " << final << endl;
            out.write(data, offset.length);
        }
    }

private:
    static const uint32_t MAGIC = 0x4B434150;
    std::string filename;
    std::map<std::string, File> files;
};

}

int main(int argc, char ** argv){
    string file;
    bool extract = false;
    for (int i = 1; i < argc; i++){
        if (string(argv[i]) == "extract"){
            extract = true;
        } else {
            file = string(argv[i]);
        }
    }
    Bor::PackReader reader(file);
    if (extract){
        reader.extract(file + "-out");
    }
}
