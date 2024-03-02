#ifdef USE_ALLEGRO
#include <allegro.h>

/* FIXME: replace with <winalleg.h> */
#ifdef _WIN32
#define BITMAP dummyBITMAP
#include <windows.h>
#undef BITMAP
#endif

#endif
#include <algorithm>
#include "r-tech1/funcs.h"
#include "r-tech1/file-system.h"
#include "r-tech1/thread.h"
#include "r-tech1/system.h"
#include "r-tech1/utf.h"
// #include "globals.h"
#include <dirent.h>
#include <sstream>
#include <exception>
#include <string>
#include <fstream>
#include <ostream>
#include <regex>
#include "r-tech1/token.h"

#include "libs/zip/unzip.h"
#include "libs/zip/ioapi.h"

#include "libs/7z/7zFile.h"
#include "libs/7z/7z.h"
#include "libs/7z/7zAlloc.h"
#include "libs/7z/7zCrc.h"

#ifndef CROSS_BUILD
#define FS_WRAPPER
// https://github.com/gulrak/filesystem/
#include "libs/filesystem/fs-wrapper.h"
#include "libs/filesystem/glob.h"
#endif

using namespace std;

/* some filesystem access can only be done by one thread at a time. specifically, sfl
 * has its own allocator that is meant to be used in a single-threaded manner.
 * rather than try to add locks to sfl we just wrap all sfl calls with a lock.
 *
 * initialize() must be called to initialize this lock
 */
// Util::Thread::Lock lock;



namespace Path{

/* remove extra path separators (/) */
string sanitize(string path){
    size_t double_slash = path.find("//");
    while (double_slash != string::npos){
        path.erase(double_slash, 1);
        double_slash = path.find("//");
    }

    /* Remove /./ from paths because its redundant */
    size_t useless = path.find("/./");
    while (useless != string::npos){
        path.erase(useless, 2);
        useless = path.find("/./");
    }

    return path;
}

static string removeEndSlashes(string path){
    size_t last = path.rfind("/");
    while (path.size() > 0 && last == path.size() - 1){
        path.erase(last, 1);
        last = path.rfind("/");
    }
    return path;
}

static int invert(int c){
    if (c == '\\'){
        return '/';
    }
    return c;
}

std::string invertSlashes(string str){
    transform(str.begin(), str.end(), str.begin(), invert);
    return str;
}

const string & Path::path() const {
    return mypath;
}
        
const string Path::getExtension() const {
    size_t dot = mypath.rfind('.');
    if (dot == string::npos){
        return "";
    } else {
        return mypath.substr(dot + 1);
    }
}

bool Path::isEmpty() const {
    return mypath.empty();
}
        
Path::~Path(){
}

Path::Path(){
}

Path::Path(const std::string & path):
mypath(sanitize(invertSlashes(path))){
}

Path::Path(const Path & path):
mypath(sanitize(invertSlashes(path.path()))){
}

RelativePath::RelativePath(){
}

RelativePath::RelativePath(const std::string & path):
Path(path){
    if (! path.empty() && path[0] == '/'){
        ostringstream out;
        out << "Relative path '" << path << "' cannot start with a /. Only absolute paths can start with /";
        throw Storage::IllegalPath(__FILE__, __LINE__, out.str());
    }
}

/* a/b/c/d -> b/c/d */
std::string stripFirstDir(const std::string & str){
    if (str.find("/") != std::string::npos || str.find( "\\") != std::string::npos){
        std::string temp = str;
        size_t rem = temp.find("/");
        if (rem != std::string::npos){
            return str.substr(rem+1,str.size());
        }
        rem = temp.find("\\");
        if( rem != std::string::npos ){
            return str.substr(rem+1,str.size());
        }
    }
    return str; 
}

static vector<string> splitPath(string path){
    vector<string> all;
    if (path.size() > 0 && path[0] == '/'){
        all.push_back("/");
    }
    size_t found = path.find('/');
    while (found != string::npos){
        if (found > 0){
            all.push_back(path.substr(0, found));
        }
        path.erase(0, found + 1);
        found = path.find('/');
    }   
    if (path.size() != 0){
        all.push_back(path);
    }   
    return all;
}

static string joinPath(const vector<string> & paths){
    ostringstream out;
    bool first = true;
    for (vector<string>::const_iterator it = paths.begin(); it != paths.end(); it++){
        if (!first){
            out << '/';
        }
        out << *it;
        first = false;
    }
    return out.str();
}

/* a/b/c/d -> a/b/c
 * a/b/c/d/ -> a/b/c
 */
static string dirname(string path){
    vector<string> paths = splitPath(path);
    if (paths.size() > 1){
        paths.pop_back();
        return joinPath(paths);
    } else if (paths.size() == 1){
        if (paths[0] == "/"){
            return "/";
        }
        return ".";
    } else {
        return ".";
    }
    /*
    while (path.size() > 0 && path[path.size() - 1] == '/'){
        path.erase(path.size() - 1);
    }

    if (path.find("/") != string::npos ||
        path.find("\\") != string::npos){
        size_t rem = path.find_last_of("/");
        if (rem != string::npos){
            return path.substr(0, rem + 1);
        }

        rem = path.find_last_of("\\");
        if (rem != string::npos){
            return path.substr(0, rem + 1);
        }
    }

    return "";
    */
}

RelativePath::RelativePath(const RelativePath & path):
Path(path){
}
        
RelativePath RelativePath::removeFirstDirectory() const {
    return RelativePath(stripFirstDir(path()));
}
        
bool RelativePath::isFile() const {
    vector<string> paths = splitPath(path());
    return paths.size() == 1;
}
        
RelativePath RelativePath::firstDirectory() const {
    vector<string> paths = splitPath(path());
    if (paths.size() > 1){
        return RelativePath(paths[0]);
    }
    return RelativePath();
}

RelativePath RelativePath::getDirectory() const {
    return RelativePath(dirname(path()));
}

RelativePath RelativePath::getFilename() const {
    return RelativePath(stripDir(path()));
}

bool RelativePath::operator<(const RelativePath & path) const {
    return this->path() < path.path();
}
        
bool RelativePath::operator==(const RelativePath & path) const {
    return this->path() == path.path();
}
        
bool RelativePath::operator!=(const RelativePath & path) const {
    return !(*this == path);
}

RelativePath RelativePath::join(const RelativePath & him) const {
    return RelativePath(path() + "/" + him.path());
}

RelativePath & RelativePath::operator=(const RelativePath & copy){
    setPath(copy.path());
    return *this;
}

AbsolutePath::AbsolutePath(){
}

AbsolutePath::AbsolutePath(const std::string & path):
Path(path){
}

AbsolutePath::AbsolutePath(const AbsolutePath & path):
Path(path){
}

AbsolutePath & AbsolutePath::operator=(const AbsolutePath & copy){
    setPath(copy.path());
    return *this;
}

AbsolutePath AbsolutePath::removeFirstDirectory() const {
    return AbsolutePath(stripFirstDir(path()));
}

bool AbsolutePath::operator==(const AbsolutePath & path) const {
    return removeEndSlashes(this->path()) == removeEndSlashes(path.path());
}
        
bool AbsolutePath::operator!=(const AbsolutePath & path) const {
    return !(*this == path);
}
        
bool AbsolutePath::operator<(const AbsolutePath & path) const {
    return this->path() < path.path();
}

string AbsolutePath::firstDirectory() const {
    vector<string> paths = splitPath(path());
    if (paths.size() > 1){
        return paths[0];
    }
    return removeEndSlashes(path());
}
        
bool AbsolutePath::isFile() const {
    vector<string> paths = splitPath(path());
    return path().find("/") == string::npos;
    // paths.size() == 1;
}
        
RelativePath AbsolutePath::remove(const AbsolutePath & what) const {
    string real = path();
    real.erase(0, what.path().size());
    while (real.find("/") == 0){
        real.erase(0, 1);
    }
    return RelativePath(real);
}
        
AbsolutePath AbsolutePath::getDirectory() const {
    return AbsolutePath(dirname(path()));
}

AbsolutePath AbsolutePath::getFilename() const {
    return AbsolutePath(stripDir(path()));
}
        
std::string AbsolutePath::getLastComponent() const {
    if (getFilename().path() == ""){
        return stripDir(removeEndSlashes(path()));
    }
    return getFilename().path();
}
        
AbsolutePath AbsolutePath::join(const RelativePath & path) const {
    return AbsolutePath(this->path() + "/" + path.path());
}
        
InsensitivePath::InsensitivePath(const Path & what):
Path(what){
}

bool InsensitivePath::operator==(const Path & path) const {
    return Util::upperCaseAll(this->path()) == Util::upperCaseAll(path.path());
}

std::string removeExtension(const std::string & str){
    if (str.find(".") != std::string::npos){
        return str.substr(0, str.find_last_of("."));
    }
    return str;
}
    
AbsolutePath replaceExtension(const Filesystem::AbsolutePath & input, const std::string & extension){
    return AbsolutePath(removeExtension(input.path()) + "." + extension);
}

/* a/b/c/d -> d */
std::string stripDir(const std::string & str){
    if (str.find("/") != std::string::npos || str.find("\\") != std::string::npos){
        std::string temp = str;
        size_t rem = temp.find_last_of( "/" );
        if (rem != std::string::npos){
            return str.substr(rem+1,str.size());
        }
        rem = temp.find_last_of( "\\" );
        if( rem != std::string::npos ){
            return str.substr(rem+1,str.size());
        }
    }
    return str; 
}

/* a/b/c/d -> a/b/c/ */
std::string stripFilename(const std::string & str){
    std::string temp = str;
    if( str.find( "/") != std::string::npos || str.find( "\\") != std::string::npos ){
        size_t rem = temp.find_last_of( "/" );
        if( rem != std::string::npos ){
            return str.substr(0,rem+1);
        }
        rem = temp.find_last_of( "\\" );
        if( rem != std::string::npos ){
            return str.substr(0,rem+1);
        }
    }
    return "";
}

}

namespace Storage{
        
Exception::Exception(const std::string & where, int line, const std::string & file):
Exc::Base(where, line),
reason(file){
}

Exception::Exception(const std::string & where, int line, const Exc::Base & nested, const std::string & file):
Exc::Base(where, line, nested),
reason(file){
}
        
Exception::Exception(const Exception & copy):
Exc::Base(copy),
reason(copy.reason){
}

Exception::~Exception() throw (){
}
        
const std::string Exception::getReason() const {
    return reason;
}

NotFound::NotFound(const std::string & where, int line, const std::string & file):
Exception(where, line, file){
}

NotFound::NotFound(const std::string & where, int line, const Exc::Base & nested, const std::string & file):
Exception(where, line, nested, file){
}
        
NotFound::NotFound(const NotFound & copy):
Exception(copy){
}

NotFound::~NotFound() throw (){
}

IllegalPath::IllegalPath(const std::string & where, int line, const std::string & file):
Exception(where, line, file){
}

IllegalPath::IllegalPath(const std::string & where, int line, const Exc::Base & nested, const std::string & file):
Exception(where, line, nested, file){
}
        
IllegalPath::IllegalPath(const IllegalPath & copy):
Exception(copy){
}

IllegalPath::~IllegalPath() throw(){
}

System::System(){
}

System::~System(){
}

static int FileCounter = 0;
vector<Filesystem::AbsolutePath> System::getFiles(const Filesystem::AbsolutePath & dataPath, const Filesystem::RelativePath & find, bool caseInsensitive){
    DebugLog3 << "Is find: " << find.path() << " a file? (" << find.isFile() << ")" << std::endl;
    if (find.isFile()){
        DebugLog3 << "File (" << find.getFilename().path() << ") passed returning directory: " << find.path() << std::endl;
        std::vector<Filesystem::AbsolutePath> files = getFiles(dataPath, find.path(), caseInsensitive);
        DebugLog3 << "Got total files: " << files.size() << " iteration: " << FileCounter << std::endl;
        FileCounter++;
        return files;
    }

    /* split the path into its consituent parts
    * a/b/c -> a/b and c
    * search for a/b, then search for c in the results
    */
    Filesystem::RelativePath directory = find.getDirectory();
    Filesystem::RelativePath file = find.getFilename();

    DebugLog3 << "Checking directory: " << directory.path() << " for file: " << file.path() << std::endl;
    vector<Filesystem::AbsolutePath> more = getFiles(dataPath, directory, caseInsensitive);
    DebugLog3 << "Total files: " << more.size() << " found." << std::endl;
    vector<Filesystem::AbsolutePath> out;
    for (vector<Filesystem::AbsolutePath>::iterator it = more.begin(); it != more.end(); it++){
        Filesystem::AbsolutePath path = *it;
        /* if its not a directory then we can't keep searching */
        if (::System::isDirectory(path.path())){
            vector<Filesystem::AbsolutePath> findMore = getFiles(path, file, caseInsensitive);
            out.insert(out.end(), findMore.begin(), findMore.end());
        }
    }
    return out;
}
        
Filesystem::AbsolutePath System::findContainer(const RelativePath & dataPath){
    try{
        return find(RelativePath(dataPath.path() + ".zip"));
    } catch (const NotFound & fail){
    }

    try{
        return find(RelativePath(dataPath.path() + ".7z"));
    } catch (const NotFound & fail){
    }

    throw NotFound(dataPath.path(), __LINE__, __FILE__);
}

vector<Filesystem::AbsolutePath> System::getContainerFilesRecursive(const Filesystem::AbsolutePath & dataPath){
    vector<Filesystem::AbsolutePath> out;

    vector<Filesystem::AbsolutePath> zips = getFilesRecursive(dataPath, "*.zip");
    out.insert(out.end(), zips.begin(), zips.end());
    zips = getFilesRecursive(dataPath, "*.7z");
    out.insert(out.end(), zips.begin(), zips.end());

    return out;
}
        
vector<Path::AbsolutePath> System::getContainerFiles(const AbsolutePath & dataPath){
    vector<Filesystem::AbsolutePath> out;

    vector<Filesystem::AbsolutePath> zips = getFiles(dataPath, "*.zip");
    out.insert(out.end(), zips.begin(), zips.end());
    zips = getFiles(dataPath, "*.7z");
    out.insert(out.end(), zips.begin(), zips.end());

    return out;
}

vector<Filesystem::AbsolutePath> System::getContainerFiles(const RelativePath & path){
    vector<Filesystem::AbsolutePath> out;

    vector<Filesystem::AbsolutePath> zips = getFiles(path, Filesystem::RelativePath("*.zip"), false);
    out.insert(out.end(), zips.begin(), zips.end());
    zips = getFiles(path, Filesystem::RelativePath("*.7z"), false);
    out.insert(out.end(), zips.begin(), zips.end());

    return out;
}

static Util::ReferenceCount<System> self;
System & instance(){
    if (self != NULL){
        return *self;
    }
    self = new Filesystem(Util::getDataPath2());
    return *self;
}

File::File(){
}
        
File::~File(){
}

std::string File::readAsString(){
    std::vector<int8_t> bytes = readAsBytes();
    std::string out(bytes.begin(), bytes.end());
    return out;
}

std::vector<int8_t> File::readAsBytes(){
    vector<int8_t> out;

    char buffer[4096];
    int read = readLine(buffer, 4096);
    while (read > 0){
        for (int i = 0; i < read; i++){
            out.push_back(buffer[i]);
        }

        read = readLine(buffer, 4096);
    }

    return out;
}
        
class NormalFile: public File {
public:
    NormalFile(const Path::AbsolutePath & path, Access mode = Read):
        path(path){
            ios_base::openmode iosMode = fstream::in;
            switch (mode){
                case Read: iosMode = fstream::in; check(path); break;
                case Write: iosMode = fstream::out; break;
                case ReadWrite: iosMode = fstream::in | fstream::out; check(path); break;
            }

            in.open(path.path().c_str(), iosMode | fstream::binary);
            if (in.good()){
                in >> noskipws;
            }
        }

    Token * location(){
        Token * head = new Token();
        *head << "file";
        *head << path.path();
        return head;
    }

    /* Throws NotFound if path doesnt exist */
    void check(const Path::AbsolutePath & path){
        if (!instance().systemExists(path)){
            ostringstream out;
            out << "Could not find '" << path.path() << "'";
            throw NotFound(__FILE__, __LINE__, out.str());
        }
    }

    long getModificationTime(){
        return ::System::getModificationTime(path.path());
    }

    bool canStream(){
        return true;
    }

    long tell(){
        return in.tellg();
    }

    void reset(){
        in.clear();
        // Need to seek back to beginning
        in.seekg(0, std::ios::beg);
    }

    off_t seek(off_t position, int whence){
        switch (whence){
            case SEEK_SET: in.seekg(position, ios::beg); break;
            case SEEK_CUR: in.seekg(position, ios::cur); break;
            case SEEK_END: in.seekg(position, ios::end); break;
        }
        return in.tellg();
    }

    bool eof(){
        return in.eof();
    }

    int getSize(){
        streampos here = in.tellg();
        in.seekg(0, ios::end);
        int length = in.tellg();
        in.seekg(here, ios::beg);
        return length;
    }

    bool good(){
        return in.good();
    }

    File & operator>>(unsigned char & c){
        in >> c;
        return *this;
    }

    int readLine(char * output, int size){
        in.read(output, size);
        return in.gcount();
    }

    ~NormalFile(){
        in.close();
    }

protected:
    const Path::AbsolutePath path;
    std::fstream in;
};

StringFile::StringFile(const std::string & start):
data(start),
stream(start){
    stream >> noskipws;
}

Token * StringFile::location(){
    Token * head = new Token();
    *head << "<string file>";
    return head;
}

long StringFile::getModificationTime(){
    /* FIXME: maybe return INT_MAX or something? */
    return 0;
}

void StringFile::reset(){
    /* need to reset the stream */
    stream = std::istringstream(data);
    stream.clear();
}

int StringFile::readLine(char * output, int size){
    stream.read(output, size);
    return stream.gcount();
}

bool StringFile::canStream(){
    return true;
}

int StringFile::getSize(){
    return data.size();
}

long StringFile::tell(){
    return stream.tellg();
}

off_t StringFile::seek(off_t position, int whence){
    switch (whence){
        case SEEK_SET: stream.seekg(position, ios::beg); break;
        case SEEK_CUR: stream.seekg(position, ios::cur); break;
        case SEEK_END: stream.seekg(position, ios::end); break;
    }
    return stream.tellg();
}

bool StringFile::eof(){
    return stream.eof();
}

bool StringFile::good(){
    return stream.good();
}

File & StringFile::operator>>(unsigned char & c){
    stream >> c;
    return *this;
}

StringFile::~StringFile(){
}

/* For 7z */
class LzmaContainer{
public:
    LzmaContainer(const string & path, const Filesystem::AbsolutePath & start):
    path(path),
    start(start){
        allocator.Alloc = SzAlloc;
        allocator.Free = SzFree;
        allocatorTemporary.Alloc = SzAllocTemp;
        allocatorTemporary.Free = SzFreeTemp;
        if (InFile_Open(&archiveStream.file, path.c_str())){
            /* Error */
        }

        FileInStream_CreateVTable(&archiveStream);
        LookToRead_CreateVTable(&lookStream, False);

        lookStream.realStream = &archiveStream.s;
        LookToRead_Init(&lookStream);

        CrcGenerateTable();

        SzArEx_Init(&database);
        int ok = SzArEx_Open(&database, &lookStream.s, &allocator, &allocatorTemporary);
        if (ok == SZ_OK){
            /* Can read files */
            UInt16 * name = NULL;
            size_t tempSize = 0;
            for (unsigned int index = 0; index < database.db.NumFiles; index++){
                size_t offset = 0;
                size_t outSizeProcessed = 0;
                const CSzFileItem * file = database.db.Files + index;
                size_t length;
                if (file->IsDir){
                    continue;
                }
                length = SzArEx_GetFileNameUtf16(&database, index, NULL);
                if (length > tempSize){
                    delete[] name;
                    name = new UInt16[length];
                    memset(name, 0, length);
                    tempSize = length;
                }

                SzArEx_GetFileNameUtf16(&database, index, name);
                files[Utf::utf16_to_utf8(name)] = index;
            }

            delete[] name;
        }
    }

    void readFile(const Path::AbsolutePath & path, unsigned char ** buffer, size_t * size){
        Path::RelativePath find(path.remove(start));
        *buffer = 0;
        *size = 0;
        if (files.find(find.path()) != files.end()){
            int index = files[find.path()];
            UInt32 block = 0;
            size_t offset = 0;
            size_t processed = 0;
            int ok = SzArEx_Extract(&database, &lookStream.s, index,
                                    &block, buffer, size, &offset, &processed,
                                    &allocator, &allocatorTemporary);
            if (ok != SZ_OK){
                Global::debug(0) << "Could not read file from 7z archive: " << path.path() << endl;
                return;
            }
            if (offset != 0){
                memmove(*buffer, *buffer + offset, processed);
            }

            /* The original size is the size of the block buffer, but we only care
             * about the size of the actual file.
             */
            *size = processed;
        }
    }

    vector<string> getFiles(){
        vector<string> names;
        for (map<string, int>::iterator it = files.begin(); it != files.end(); it++){
            names.push_back(it->first);
        }
        return names;
    }

    string getPath() const {
        return path;
    }

    string getMount() const {
        return start.path();
    }

    long getModificationTime(const Path::AbsolutePath & path){
        if (files.find(path.path()) != files.end()){
            int index = files[path.path()];
            const CSzFileItem * file = database.db.Files + index; 
            if (file->MTimeDefined){
                CNtfsFileTime time = file->MTime;

                /* Divide by 10 million here? */
                return (time.Low | ((UInt64)time.High << 32)) / 10000000;

                /*
                unsigned year, mon, day, hour, min, sec;
                UInt64 v64 = (time.Low | ((UInt64)time.High << 32)) / 10000000;
                Byte ms[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                unsigned t;
                UInt32 v;
                sec = (unsigned)(v64 % 60); v64 /= 60;
                min = (unsigned)(v64 % 60); v64 /= 60;
                hour = (unsigned)(v64 % 24); v64 /= 24;

                v = (UInt32)v64;

                const int PERIOD_4 = (4 * 365 + 1);
                const int PERIOD_100 = (PERIOD_4 * 25 - 1);
                const int PERIOD_400 = (PERIOD_100 * 4 + 1);

                year = (unsigned)(1601 + v / PERIOD_400 * 400);
                v %= PERIOD_400;

                t = v / PERIOD_100; if (t ==  4) t =  3; year += t * 100; v -= t * PERIOD_100;
                t = v / PERIOD_4;   if (t == 25) t = 24; year += t * 4;   v -= t * PERIOD_4;
                t = v / 365;        if (t ==  4) t =  3; year += t;       v -= t * 365;

                if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)){
                    ms[1] = 29;
                }

                for (mon = 1; mon <= 12; mon++){
                    unsigned s = ms[mon - 1];
                    if (v < s)
                        break;
                    v -= s;
                }

                day = (unsigned)v + 1;
                struct tm outTime;
                memset(&outTime, 0, sizeof(outTime));
                outTime.tm_sec = sec;
                outTime.tm_min = min;
                outTime.tm_hour = hour;
                outTime.tm_mday = day;
                outTime.tm_mon = mon;
                / * tm_year The number of years since 1900. * /
                outTime.tm_year = year - 1900;
                outTime.tm_isdst = -1;
                return mktime(&outTime);
                */
            }
        }

        return 0;
    }

    virtual ~LzmaContainer(){
        SzArEx_Free(&database, &allocator);
        File_Close(&archiveStream.file);
    }

    string path;
    Path::AbsolutePath start;

    CFileInStream archiveStream;
    CLookToRead lookStream;
    CSzArEx database;
    ISzAlloc allocator;
    ISzAlloc allocatorTemporary;

    /* Map the filename to its index in the 7z database */
    map<string, int> files;
};

/* overlays:
 * add x/y.zip
 * y.zip contains
 *   example.txt
 * try to read x/example.txt, goto y.zip
 * or y.zip could contain y/
 * 
 * z.zip contains
 *   example.txt
 *
 * how can you prevent two zip files from providing the same files?
 */
class ZipContainer{
public:
    ZipContainer(const string & path, const Filesystem::AbsolutePath & start):
    path(path),
    start(start),
    locked(false){
        zipFile = unzOpen(path.c_str());
        if (zipFile == NULL){
            throw Exception(__FILE__, __LINE__, "Could not open zip file");
        }

        if (unzGoToFirstFile(zipFile) != UNZ_OK){
            throw Exception(__FILE__, __LINE__, "Could not get to first file");
        }

        do{
            char filename[1024];
            unz_file_info fileInfo;
            unzGetCurrentFileInfo(zipFile, &fileInfo, filename, sizeof(filename), NULL, 0, NULL, 0);
            files.push_back(string(filename));

        } while (unzGoToNextFile(zipFile) != UNZ_END_OF_LIST_OF_FILE);

        /*
        zlib_filefunc64_32_def functions;
        functions.open_file_func = __real_open;
        functions.tell_file_func = __real_lseek;
        functions.seek_file_func = __real_lseek;
        */
        /*
        unzFile zip = unzOpen(path.c_str());
        unz_global_info info;
        if (unzGetGlobalInfo(zip, &info) == UNZ_OK){
            Global::debug(0) << "Entries: " << info.number_entry << std::endl;
        }

        if (unzGoToFirstFile(zip) != UNZ_OK){
            throw Exception(__FILE__, __LINE__, "Could not get to first file");
        }
        do{
            char filename[1024];
            unz_file_info fileInfo;
            unzGetCurrentFileInfo(zip, &fileInfo, filename, sizeof(filename), NULL, 0, NULL, 0);
            Global::debug(0) << "Got file " << filename << std::endl;
            Global::debug(0) << " Compressed " << fileInfo.compressed_size << " uncompressed " << fileInfo.uncompressed_size << std::endl;

        } while (unzGoToNextFile(zip) != UNZ_END_OF_LIST_OF_FILE);
        unzClose(zip);
        */
    }

    ~ZipContainer(){
        if (zipFile != NULL){
            unzClose(zipFile);
        }
    }

    string getPath() const {
        return path;
    }

    string getMount() const {
        return start.path();
    }

    long modificationTime(){
        char filename[1024];
        unz_file_info fileInfo;
        unzGetCurrentFileInfo(zipFile, &fileInfo, filename, sizeof(filename), NULL, 0, NULL, 0);
        struct tm outTime;
        memset(&outTime, 0, sizeof(outTime));
        outTime.tm_sec = fileInfo.tmu_date.tm_sec;
        outTime.tm_min = fileInfo.tmu_date.tm_min;
        outTime.tm_hour = fileInfo.tmu_date.tm_hour;
        outTime.tm_mday = fileInfo.tmu_date.tm_mday;
        outTime.tm_mon = fileInfo.tmu_date.tm_mon;
        /* tm_year The number of years since 1900. */
        outTime.tm_year = fileInfo.tmu_date.tm_year - 1900;
        outTime.tm_isdst = -1;
        return mktime(&outTime);
    }

    void findFile(const Path::AbsolutePath & file){
        Path::RelativePath find(file.remove(start));
        if (unzLocateFile(zipFile, find.path().c_str(), 2) != UNZ_OK){
            Global::debug(0) << "Could not find " << find.path() << std::endl;
        } else {
            Global::debug(1) << "Found " << find.path() << " in zip file " << path << std::endl;
        }
    }

    int currentFileSize(){
        char filename[1024];
        unz_file_info fileInfo;
        unzGetCurrentFileInfo(zipFile, &fileInfo, filename, sizeof(filename), NULL, 0, NULL, 0);
        return fileInfo.uncompressed_size;
    }

    int read(char * buffer, int size){
        // Global::debug(0) << "offset before read " << unzGetOffset(zipFile) << " tell " << unztell(zipFile) << std::endl;
        int got = unzReadCurrentFile(zipFile, buffer, size);
        // Global::debug(0) << "offset after read " << unzGetOffset(zipFile) << " tell " << unztell(zipFile) << std::endl;
        if (got <= 0){
            throw Exception(__FILE__, __LINE__, "Could not read bytes from zip");
        }
        return got;
    }

    long tell(){
        return unztell(zipFile);
    }

    void open(const Path::AbsolutePath & file){
        if (locked){
            std::ostringstream out;
            char filename[1024];
            unz_file_info fileInfo;
            unzGetCurrentFileInfo(zipFile, &fileInfo, filename, sizeof(filename), NULL, 0, NULL, 0);
            out << "Could not open zip file entry " << file.path() << " because a zip file is already open: " << filename;
            throw Exception(__FILE__, __LINE__, out.str());
        }
        findFile(file);
        if (unzOpenCurrentFile(zipFile) != UNZ_OK){
            std::ostringstream out;
            out << "Could not open zip file entry " << file.path();
            throw Exception(__FILE__, __LINE__, out.str());
        }
        locked = true;
    }

    void close(){
        unzCloseCurrentFile(zipFile);
        locked = false;
    }

    vector<string> getFiles() const {
        return files;
    }

protected:
    /* Path to the zip file itself */
    const string path;
    /* Where overlay starts */
    const Path::AbsolutePath start;

    unzFile zipFile;
    vector<string> files;
    /* Only one file can be opened at a time, so if another file is opened
     * while locked=true then throw an error
     */
    bool locked;
};

class ZipFile: public File {
public:
    ZipFile(const Path::AbsolutePath & path, const Util::ReferenceCount<ZipContainer> & zip):
        path(path),
        zip(zip),
        atEof(false),
        position(0){
            zip->open(path);
        }

    virtual ~ZipFile(){
        zip->close();
    }

    long getModificationTime(){
        return zip->modificationTime();
    }

    Token * location(){
        Token * head = new Token();
        *head << "container";
        /* container zipfile mount-point file-inside-zip */
        *head << zip->getPath();
        *head << zip->getMount();
        *head << path.path();
        return head;
    }
        
    off_t seek(off_t where, int whence){
        /* It seems that minizip is not capable of seeking in a specific file in a zip
         * container so we have to re-open the file and read `position' bytes to
         * emulate the seek behavior.
         */
        switch (whence){
            case SEEK_SET: {
                if (where >= position){
                    /* Read bytes until we hit the offset */
                    position += skipBytes(where - position);
                } else {
                    zip->close();
                    zip->open(path);
                    /* re-open file and read where bytes */
                    position = skipBytes(where);
                }
                break;
            }
            case SEEK_CUR: {
                position += skipBytes(where);
                break;
            }
            case SEEK_END: {
                return seek(getSize() + where, SEEK_SET);
                break;
            }
        }
        if (position > getSize()){
            position = getSize();
        }
        atEof = position == getSize();

        return position;
    }
        
    bool eof(){
        return atEof;
    }
        
    long tell(){
        return zip->tell();
    }

    void reset(){
        /* TODO or nothing..? */
        zip->close();
        position = 0;
        atEof = false;
        zip->open(path);
    }

    bool canStream(){
        return false;
    }

    bool good(){
        /* FIXME */
        return true;
    }

    File & operator>>(unsigned char & c){
        readLine((char*) &c, 1);
        return *this;
    }
        
    int getSize(){
        return zip->currentFileSize();
    }
        
    int readLine(char * output, int size){
        try{
            int read = zip->read(output, size);
            position += read;
            return read;
        } catch (const Exception & nomore){
            atEof = true;
            return 0;
        }
    }

    protected:
        /* skips `bytes'.
         * returns number of bytes skipped (may be less than bytes)
         */
    int skipBytes(int bytes){
        char dummy[1024];
        int total = 0;
        while (bytes > 0){
            /* Calls the zip file directly here so we don't mess up position */
            int read = zip->read(dummy, bytes > (int) sizeof(dummy) ? (int) sizeof(dummy) : bytes);
            total += read;
            if (read == 0){
                return total;
            }
            bytes -= read;
        }
        return total;
    }

    const Path::AbsolutePath path;
    const Util::ReferenceCount<ZipContainer> zip;
    bool atEof;
    /* keep track of bytes read so we can seek easier */
    int position;
};

Descriptor::Descriptor(){
}

Descriptor::~Descriptor(){
}

class ZipDescriptor: public Descriptor {
public:
    ZipDescriptor(const Path::AbsolutePath & path, const Util::ReferenceCount<ZipContainer> & container):
    path(path),
    container(container){
    }

    Path::AbsolutePath path;
    Util::ReferenceCount<ZipContainer> container;

    using Descriptor::open;
    virtual Util::ReferenceCount<File> open(File::Access mode){
        return Util::ReferenceCount<File>(new ZipFile(path, container));
    }

    virtual ~ZipDescriptor(){
    }
};

/* Extracts an entire file from a 7z archive into memory */
class LzmaFile: public File {
public:
    LzmaFile(const Path::AbsolutePath & path, const Util::ReferenceCount<LzmaContainer> & container):
    path(path),
    container(container),
    position(0),
    memory(NULL),
    size(0){
        container->readFile(path, &memory, &size);
    }

    virtual ~LzmaFile(){
        SzFree(NULL, memory);
    }

    bool eof(){
        return position >= (int) size;
    }

    virtual bool good(){
        return memory != NULL;
    }

    virtual int getSize(){
        return size;
    }

    virtual bool canStream(){
        return true;
    }

    virtual void reset(){
        /* nothing */
    }

    virtual long tell(){
        return position;
    }

    virtual Token * location(){
        Token * head = new Token();
        *head << "container";
        /* container zipfile mount-point file-inside-zip */
        *head << container->getPath();
        *head << container->getMount();
        *head << path.path();
        return head;

    }

    virtual long getModificationTime(){
        return container->getModificationTime(path);
    }

    virtual off_t seek(off_t position, int whence){
        switch (whence){
            case SEEK_SET: this->position = position; break;
            case SEEK_CUR: this->position += position; break;
            case SEEK_END: this->position = this->size + position; break;
        }
        if (this->position < 0){
            this->position = 0;
        }
        if (this->position > (int) size){
            this->position = size;
        }
        return this->position;
    }

    virtual File & operator>>(unsigned char & out){
        if (this->position < (int) size){
            out = memory[position];
            position += 1;
        }
        return *this;
    }

    virtual int readLine(char * output, int size){
        if (size > (int) (this->size - this->position)){
            size = this->size - this->position;
        }
        memmove(output, memory + this->position, size);
        this->position += size;
        return size;
    }

protected:
    const Path::AbsolutePath path;
    const Util::ReferenceCount<LzmaContainer> container;
    /* keep track of bytes read so we can seek easier */
    int position;
        
    unsigned char * memory;
    size_t size;
};

class LzmaDescriptor: public Descriptor {
public:
    LzmaDescriptor(const Path::AbsolutePath & path, const Util::ReferenceCount<LzmaContainer> & container):
    path(path),
    container(container){
    }

    Path::AbsolutePath path;
    Util::ReferenceCount<LzmaContainer> container;

    using Descriptor::open;
    virtual Util::ReferenceCount<File> open(File::Access mode){
        return Util::ReferenceCount<File>(new LzmaFile(path, container));
    }

    virtual ~LzmaDescriptor(){
    }
};

/* Keep this updated with all the supported container types */
bool isContainer(const Path::AbsolutePath & path){
    return path.getExtension() == "zip" ||
           path.getExtension() == "7z";
}
    
vector<std::string> containerTypes(){
    vector<string> types;
    types.push_back("zip");
    types.push_back("7z");
    return types;
}

bool System::isDirectory(const AbsolutePath & path){
    return virtualDirectory.isDirectory(path) || systemIsDirectory(path);
}

bool System::exists(const AbsolutePath & path){
    return virtualDirectory.exists(path) || systemExists(path);
}

void System::overlayFile(const AbsolutePath & where, Util::ReferenceCount<LzmaContainer> container){
    virtualDirectory.addFile(where, Util::ReferenceCount<LzmaDescriptor>(new LzmaDescriptor(where, container)));
}

void System::overlayFile(const AbsolutePath & where, Util::ReferenceCount<ZipContainer> zip){
    virtualDirectory.addFile(where, Util::ReferenceCount<ZipDescriptor>(new ZipDescriptor(where, zip)));
}

void System::unoverlayFile(const AbsolutePath & where){
    virtualDirectory.removeFile(where);
}
        
vector<string> System::containerFileList(const AbsolutePath & container){
    Util::ReferenceCount<ZipContainer> zip(new ZipContainer(container.path(), Filesystem::AbsolutePath()));
    return zip->getFiles();
}

static bool isZipFile(const Filesystem::AbsolutePath & path){
    return path.path().find(".zip") != string::npos;
}

static bool is7zFile(const Filesystem::AbsolutePath & path){
    return path.path().find(".7z") != string::npos;
}

template <class Container>
static void addOverlayFiles(System & system, const Filesystem::AbsolutePath & where, const Util::ReferenceCount<Container> & container){
    vector<string> files = container->getFiles();
    for (vector<string>::const_iterator it = files.begin(); it != files.end(); it++){
        string path = *it;
        Global::debug(1) << "Add overlay to " << where.join(Filesystem::RelativePath(path)).path() << std::endl;
        system.overlayFile(where.join(Filesystem::RelativePath(path)), container);
    }
}

void System::addOverlay(const AbsolutePath & container, const AbsolutePath & where){
    if (isZipFile(container)){
        Global::debug(1) << "Opening zip file " << container.path() << std::endl;
        addOverlayFiles(*this, where, Util::ReferenceCount<ZipContainer>(new ZipContainer(container.path(), where)));
    } else if (is7zFile(container)){
        addOverlayFiles(*this, where, Util::ReferenceCount<LzmaContainer>(new LzmaContainer(container.path(), where)));
    }
}

void System::removeOverlay(const AbsolutePath & container, const AbsolutePath & where){
    Util::ReferenceCount<ZipContainer> zip(new ZipContainer(container.path(), where));
    vector<string> files = zip->getFiles();
    for (vector<string>::const_iterator it = files.begin(); it != files.end(); it++){
        string path = *it;
        Global::debug(1) << "Remove overlay from " << where.join(Filesystem::RelativePath(path)).path() << std::endl;
        unoverlayFile(where.join(Filesystem::RelativePath(path)));
    }
}

Util::ReferenceCount<File> System::open(const AbsolutePath & path, File::Access mode){
    Util::ReferenceCount<Descriptor> descriptor = virtualDirectory.lookup(path);
    if (descriptor != NULL){
        return descriptor->open(mode);
    } else {
        return Util::ReferenceCount<File>(new NormalFile(path, mode));
    }
}

std::string readFile(const Path::AbsolutePath & path){
    Util::ReferenceCount<Storage::File> file = Storage::instance().open(path);
    if (file != NULL){
        char * data = new char[file->getSize() + 1];
        file->readLine(data, file->getSize());
        data[file->getSize()] = 0;
        std::string out(data);
        delete[] data;
        return out;
    }
    std::ostringstream out;
    out << "Could not open file " << path.path();
    throw Exception(__FILE__, __LINE__, out.str());
}

bool hasInstance(){
    // return true;
    return self != NULL;
}

System & setInstance(const Util::ReferenceCount<System> & what){
    self = what;
    return *self;
}

/* will read upto 'length' bytes unless a null byte is seen first */
string EndianReader::readStringX(int length){
    ostringstream out;
    uint8_t letter = readByte1();
    while (letter != 0 && length > 0){
        out << letter;
        letter = readByte1();
        length -= 1;
    }
    return out.str();
}

/* unconditionally reads 'length' bytes */
std::string EndianReader::readString2(int length){
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
            
void EndianReader::seekEnd(streamoff where){
    internal->seekEnd(where);
}

void EndianReader::seek(streampos where){
    internal->seek(where);
}

int EndianReader::position(){
    return internal->tell();
}
            
void EndianReader::readBytes(uint8_t * out, int length){
    internal->read((char*) out, length);
}

vector<uint8_t> EndianReader::readBytes(int length){
    vector<uint8_t> bytes;
    for (int i = 0; i < length; i++){
        uint8_t byte = 0;
        internal->read((char*) &byte, 1);
        if (internal->eof()){
            throw Eof();
        } else {
        }
        bytes.push_back(byte);
    }
    return bytes;
}

EndianReader::Internal::Internal(){
}

EndianReader::Internal::~Internal(){
}

bool EndianReader::StreamInternal::eof(){
    return stream.eof();
}

int EndianReader::StreamInternal::read(char * data, int length){
    stream.read(data, length);
    return stream.gcount();
}

void EndianReader::StreamInternal::seekEnd(std::streamoff where){
    stream.seekg(where, ios::end);
}

void EndianReader::StreamInternal::seek(std::streamoff where){
    stream.seekg(where);
}

int EndianReader::StreamInternal::tell(){
    return stream.tellg();
}

EndianReader::StreamInternal::~StreamInternal(){
}

bool EndianReader::FileInternal::eof(){
    return file->eof();
}

void EndianReader::FileInternal::seekEnd(std::streamoff where){
    file->seek(where, SEEK_END);
}

void EndianReader::FileInternal::seek(std::streamoff where){
    file->seek(where, SEEK_SET);
}

int EndianReader::FileInternal::read(char * data, int length){
    return file->readLine(data, length);
}

int EndianReader::FileInternal::tell(){
    return file->tell();
}

EndianReader::FileInternal::~FileInternal(){
}

}

Filesystem::Filesystem(const AbsolutePath & path):
dataPath(path){
}

Filesystem::AbsolutePath Filesystem::lookup(const RelativePath path){
    vector<Filesystem::AbsolutePath> places;

#define push(x) try{ places.push_back(x); } catch (const Storage::IllegalPath & fail){ }
    push(dataPath.join(path));
    push(userDirectory().join(path));
    push(Filesystem::AbsolutePath(path.path()));
#undef push

    /* start error stuff early */
    ostringstream out;
    out << "Cannot find " << path.path() << ". I looked in ";
    bool first = true;
    for (vector<Filesystem::AbsolutePath>::iterator it = places.begin(); it != places.end(); it++){
        const Filesystem::AbsolutePath & final = *it;
        if (exists(final)){
            return final;
        }
        if (!first){
            out << ", ";
        } else {
            first = false;
        }
        out << "'" << final.path() << "'";
    }

    // out << "Cannot find " << path.path() << ". I looked in '" << dataPath.join(path).path() << "', '" << userDirectory().join(path).path() << "', and '" << path.path() << "'";

    throw NotFound(__FILE__, __LINE__, out.str());


#if 0
    /* first try the main data directory */
    Filesystem::AbsolutePath final = dataPath.join(path);
    if (::System::readable(final.path())){
        return final;
    }
    /* then try the user directory, like ~/.paintown */
    final = userDirectory().join(path);
    if (::System::readable(final.path())){
        return final;
    }
    /* then just look in the cwd */
    if (::System::readable(path.path())){
        return Filesystem::AbsolutePath(path.path());
    }

    ostringstream out;
    out << "Cannot find " << path.path() << ". I looked in '" << dataPath.join(path).path() << "', '" << userDirectory().join(path).path() << "', and '" << path.path() << "'";

    throw NotFound(__FILE__, __LINE__, out.str());
#endif
}

Filesystem::AbsolutePath Filesystem::lookupInsensitive(const Filesystem::AbsolutePath & directory, const Filesystem::RelativePath & path){
    if (path.path() == ""){
        throw NotFound(__FILE__, __LINE__, "Given empty path to lookup");
    }
    if (path.path() == "."){
        return directory;
    }
    if (path.path() == ".."){
        return directory.getDirectory();
    }
    if (path.isFile()){
        vector<AbsolutePath> all = getFiles(directory, "*", true);
        for (vector<AbsolutePath>::iterator it = all.begin(); it != all.end(); it++){
            AbsolutePath & check = *it;
            if (InsensitivePath(check.getFilename()) == path){
                return check;
            }
        }

        ostringstream out;
        out << "Cannot find " << path.path() << " in " << directory.path();
        throw NotFound(__FILE__, __LINE__, out.str());
    } else {
        return lookupInsensitive(lookupInsensitive(directory, path.firstDirectory()), path.removeFirstDirectory());
    }
}

vector<Filesystem::AbsolutePath> Filesystem::findDirectoriesIn(const Filesystem::AbsolutePath & path){
    vector<Filesystem::AbsolutePath> dirs = virtualDirectory.findDirectories(path, "*", false);

    DIR * dir = opendir(path.path().c_str());
    if (dir == NULL){
        return dirs;
    }

    struct dirent * entry = readdir(dir);
    while (entry != NULL){
        if (string(entry->d_name) != "." && string(entry->d_name) != ".."){
            string total = path.path() + "/" + entry->d_name;
            if (::System::isDirectory(total)){
                dirs.push_back(AbsolutePath(total));
            }
        }
        entry = readdir(dir);
    }

    closedir(dir);

    return dirs;
}

vector<Filesystem::AbsolutePath> Filesystem::findDirectories(const RelativePath & path){
    typedef vector<AbsolutePath> Paths;
    Paths dirs;

    Paths main_dirs = findDirectoriesIn(dataPath.join(path));
    Paths user_dirs = findDirectoriesIn(userDirectory().join(path));
    Paths here_dirs = findDirectoriesIn(Filesystem::AbsolutePath(path.path()));

    dirs.insert(dirs.end(), main_dirs.begin(), main_dirs.end());
    dirs.insert(dirs.end(), user_dirs.begin(), user_dirs.end());
    dirs.insert(dirs.end(), here_dirs.begin(), here_dirs.end());

    return dirs;
}

/* a/b/c/ -> a/b/c */
static string removeTrailingSlash(string str){
    while (str.size() > 0 && str[str.size() - 1] == '/'){
        str = str.erase(str.size() - 1);
    }
    return str;
}

#ifndef CROSS_BUILD
vector<Filesystem::AbsolutePath> Filesystem::getFiles(const AbsolutePath & dataPath, const string & find, bool caseInsensitive){
#ifdef USE_ALLEGRO
    struct al_ffblk info;
    vector<AbsolutePath> files;

    if (al_findfirst((dataPath.path() + "/" + find).c_str(), &info, FA_ALL ) != 0){
        return files;
    }
    files.push_back(AbsolutePath(dataPath.path() + "/" + string(info.name)));
    while ( al_findnext( &info ) == 0 ){
        files.push_back(AbsolutePath(dataPath.path() + "/" + string(info.name)));
    }
    al_findclose( &info );
    return files;
#else
    Util::Thread::ScopedLock scoped(lock);
    vector<AbsolutePath> files;

    vector<AbsolutePath> more = virtualDirectory.findFiles(dataPath, find, caseInsensitive);
    files.insert(files.end(), more.begin(), more.end());

#ifdef FS_WRAPPER
    // Convert to type std::filesystem::path
    //fs::path path = dataPath.path();
    //path /= find;
    try {
        DebugLog2 << "Looking for file: " << find << " in dataPath: " << dataPath.path() << " (" << find << ")"  << std::endl;
        // for (fs::path & globFile : glob::glob(path.generic_string())){
        for (fs::path & globFile : glob::glob1(dataPath.path(), find, false)){
            DebugLog2 << "Got datapath: " << dataPath.path().c_str() << " globFile: " << globFile.c_str() << std::endl;
            files.push_back(AbsolutePath(dataPath.join(Filesystem::RelativePath(globFile.string()))));
        }
    } catch(fs::filesystem_error &ex){
        DebugLog2 << "Directory or dataPath: " << dataPath.path() << " does not exist. Reason: " << ex.what() << std::endl;
    }
    return files;
#endif
#endif
}
#endif

std::vector<Filesystem::AbsolutePath> Filesystem::getFiles(const RelativePath & path, const RelativePath & find, bool caseInsensitive){
    vector<AbsolutePath> directories;
    directories.push_back(dataPath.join(path));
    directories.push_back(userDirectory().join(path));
    directories.push_back(Filesystem::AbsolutePath(path.path()));

    vector<AbsolutePath> files;
    for (vector<AbsolutePath>::iterator it = directories.begin(); it != directories.end(); it++){
        Global::debug(0) << "Search for " << find.path() << " in " << it->path() << std::endl;
        vector<AbsolutePath> found = getFiles(*it, find, caseInsensitive);
        files.insert(files.end(), found.begin(), found.end());
    }
    return files;
}



template <class X>
static void append(vector<X> & destination, const vector<X> & source){
    /*
    for (typename vector<X>::const_iterator it = source.begin(); it != source.end(); it++){
        destination.push_back(*it);
    }
    */
    copy(source.begin(), source.end(), back_insert_iterator<vector<X> >(destination));
}
    
vector<Filesystem::AbsolutePath> Filesystem::getAllDirectories(const AbsolutePath & path){
    vector<AbsolutePath> all = findDirectoriesIn(path);
    vector<AbsolutePath> final;
    append(final, all);
    for (vector<AbsolutePath>::iterator it = all.begin(); it != all.end(); it++){
        vector<AbsolutePath> more = getAllDirectories(*it);
        append(final, more);
    }
    return final;
}

vector<Filesystem::AbsolutePath> Filesystem::getFilesRecursive(const AbsolutePath & dataPath, const string & find, bool caseInsensitive){
    if (!exists(dataPath)){
        return vector<AbsolutePath>();
    }
    vector<AbsolutePath> directories = getAllDirectories(dataPath);
    directories.push_back(dataPath);
    vector<AbsolutePath> files;
    for (vector<AbsolutePath>::iterator it = directories.begin(); it != directories.end(); it++){
        vector<AbsolutePath> found = getFiles(*it, find, caseInsensitive);
        append(files, found);
    }
    return files;
}

/*
std::string find(const std::string path){
    if (path.length() == 0){
        throw NotFound("No path given");
    }
    if (path[0] == '/'){
        string str(path);
        str.erase(0, 1);
        string out = lookup(str);
        if (System::isDirectory(out)){
            return sanitize(out + "/");
        }
        return sanitize(out);
    }
    string out = lookup(path);
    if (System::isDirectory(out)){
        return sanitize(out + "/");
    }
    return sanitize(out);
}
*/

Filesystem::AbsolutePath Filesystem::find(const RelativePath & path){
    if (path.isEmpty()){
        throw NotFound(__FILE__, __LINE__, "No path given");
    }

    AbsolutePath out = lookup(path);
    if (::System::isDirectory(out.path())){
        return AbsolutePath(out.path() + "/");
    }
    return AbsolutePath(out.path());
}
    
Filesystem::AbsolutePath Filesystem::findInsensitive(const RelativePath & path){
    try{
        /* try sensitive lookup first */
        return lookup(path);
    } catch (const NotFound & fail){
    }
    /* get the base directory */
    AbsolutePath directory = lookup(path.getDirectory());
    return lookupInsensitive(directory, path.getFilename());
}
    
bool Filesystem::exists(const RelativePath & path){
    try{
        AbsolutePath absolute = find(path);
        return true;
    } catch (const NotFound & found){
        return false;
    }
}

bool Filesystem::systemIsDirectory(const AbsolutePath & path){
    return ::System::isDirectory(path.path());
}

bool Filesystem::systemExists(const AbsolutePath & path){
    return ::System::readable(path.path());
}

Filesystem::RelativePath Filesystem::cleanse(const AbsolutePath & path){
    string str = path.path();
    if (str.find(dataPath.path()) == 0){
        str.erase(0, dataPath.path().length());
    } else if (str.find(userDirectory().path()) == 0){
        str.erase(0, userDirectory().path().length());
    }
    return RelativePath(str);
}
