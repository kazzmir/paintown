#include "pack-reader.h"

#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "util/debug.h"
#include "util/regex.h"
#include "util/file-system.h"

using namespace std;

namespace Bor{

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

static string lowercase(string in){
    for (unsigned int i = 0; i < in.length(); i++){
        if (in[i] >= 'A' && in[i] <= 'Z'){
            in[i] = in[i] - 'A' + 'a';
        }
    }
    return in;
}

static string sanitizePath(const string & path){
    return Path::sanitize(Path::invertSlashes(lowercase(path)));
}

PackReader::PackReader(const Filesystem::AbsolutePath & filename):
filename(filename){
    ifstream stream;
    Global::debug(0) << "Reading pak file " << filename.path() << endl;
    stream.open(filename.path().c_str(), std::ios::in | std::ios::binary);
    Storage::LittleEndianReader reader(stream);
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
            string name = reader.readStringX(80);
            if (name.size() != 0){
                files[sanitizePath(name)] = File(start, size);
            }
            done = name.size() == 0;
            // cout << name << " at " << start << " size " << size << " length " << length << endl;
            // cout << " seek to " << (current + length) << endl;
            // reader.seek(current + length);
            done |= stream.eof();
        }
    } catch (const Storage::Eof & eof){
    }
    stream.close();

    handle.open(filename.path().c_str(), std::ios::in | std::ios::binary);
    Util::Thread::initializeLock(&readLock);
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
    return Util::matchRegex(path, Util::Regex(regex));
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
    Util::Thread::acquireLock(&readLock);
    handle.seekg(file.start);
    char * data = new char[file.length];
    handle.read(data, file.length);
    Util::Thread::releaseLock(&readLock);
    return data;
}
    
const PackReader::File & PackReader::getFile(const std::string & path){
    if (files.find(lowercase(path)) != files.end()){
        return files.find(lowercase(path))->second;
    }
    ostringstream out;
    out << "No such pak file '" << path << "'";
    throw PackError(__FILE__, __LINE__, out.str());
}
    
uint32_t PackReader::getFileLength(const std::string & path){
    const File & file = getFile(path);
    return file.length;
}

PackReader::~PackReader(){
    handle.close();
    Util::Thread::destroyLock(&readLock);
}

}


