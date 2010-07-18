#include "pack-reader.h"

#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "debug.h"
#include "util/regex.h"
#include "util/file-system.h"

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

    virtual ~EndianReader(){
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

static string lowercase(string in){
    for (unsigned int i = 0; i < in.length(); i++){
        if (in[i] >= 'A' && in[i] <= 'Z'){
            in[i] = in[i] - 'A' + 'a';
        }
    }
    return in;
}

static string sanitizePath(const string & path){
    return Filesystem::sanitize(Filesystem::invertSlashes(lowercase(path)));
}

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
                files[sanitizePath(name)] = File(start, size);
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

static string convertToRegex(const string & input){
    ostringstream out;
    for (unsigned int i = 0; i < input.size(); i++){
        char current = input[i];
        switch (current){
            case '*' : {
                out << ".*";
                break;
            }
            case '.' : {
                out << "\\.";
                break;
            }
            default : {
                out << current;
                break;
            }
        }
    }
    return out.str();
}

static bool matchPart(const string & path, const string & search){
    string regex = convertToRegex(search);
    return Util::matchRegex(path, regex);
}

/* split "foo/bar/baz" into [foo, bar, baz] */
static vector<string> split(const string & path){
    vector<string> out;
    size_t begin = 0;
    size_t last = 0;
    begin = path.find("/");
    // Global::debug(0) << "Splitting '" << path << "'" << endl;
    while (begin != string::npos){
        string split = path.substr(last, begin - last);
        // Global::debug(0) << " Part '" << split << "'" << endl;
        out.push_back(split);
        last = begin + 1;
        begin = path.find("/", last);
    }
    out.push_back(path.substr(last));
    return out;
}

static bool matchPath(const string & path, const string & search){
    vector<string> pathParts = split(path);
    vector<string> searchParts = split(search);
    vector<string>::iterator pathIt = pathParts.begin();
    vector<string>::iterator searchIt = searchParts.begin();
    while (true){
        if (pathIt == pathParts.end() && searchIt == searchParts.end()){
            /* Both are at the end, success! */
            return true;
        } else if (pathIt == pathParts.end() || searchIt == searchParts.end()){
            /* one got to the end before the other, failure! */
            return false;
        }
        if (matchPart(*pathIt, *searchIt)){
            pathIt++;
            searchIt++;
        } else {
            return false;
        }
    }
    return false;
}
    
vector<string> PackReader::findPaths(const std::string & search){
    vector<string> found;
    for (map<string, File>::iterator it = files.begin(); it != files.end(); it++){
        const string & path = (*it).first;
        if (matchPath(path, search)){
            found.push_back(path);
        }
    }
    return found;
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


