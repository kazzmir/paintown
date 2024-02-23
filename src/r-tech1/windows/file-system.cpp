#ifdef WINDOWS

#include <windows.h>

// #define _WIN32_IE 0x400
#include <shlobj.h>

#include <vector>
#include <algorithm>
#include <regex>
#include "r-tech1/funcs.h"
#include "r-tech1/file-system.h"
#include "r-tech1/thread.h"
#include "r-tech1/system.h"
#include "r-tech1/utf.h"

using namespace std;

Filesystem::AbsolutePath Filesystem::userDirectory(){
    ostringstream str;
    char path[MAX_PATH];
    SHGetSpecialFolderPathA(0, path, CSIDL_APPDATA, false);
    str << path << "/paintown/";
    return Filesystem::AbsolutePath(str.str());
}

Filesystem::AbsolutePath Filesystem::configFile(){
    ostringstream str;
    char path[MAX_PATH];
    SHGetSpecialFolderPathA(0, path, CSIDL_APPDATA, false);
    str << path << "/paintown_configuration.txt";
    return Filesystem::AbsolutePath(str.str());
}

vector<Filesystem::AbsolutePath> Filesystem::getFiles(const AbsolutePath & dataPath, const string & find, bool caseInsensitive){
    Util::Thread::ScopedLock scoped(lock);
    vector<AbsolutePath> files;

    vector<AbsolutePath> more = virtualDirectory.findFiles(dataPath, find, caseInsensitive);
    files.insert(files.end(), more.begin(), more.end());

    DebugLog2 << "Looking for file: " << find << " in dataPath: " << dataPath.path() << " (" << find << ")"  << std::endl;
    class Globber
    {
    private:
        typedef struct {
            std::string filename;
            std::string path;
            bool isDirectory;
        } File;
    public:
        Globber(const std::string & pattern, const std::string & path):
        pattern(createRegex(pattern)),
        origin(path){
            doMatches(path);
        }
        ~Globber(){
        }
        std::vector<std::string> matches;
    private:
        void doMatches(const std::string & path){
            try {
                
                DebugLog3 << "String: " << origin << " path: " << path << std::endl;
                for (const File & entry: this->getFiles(path)){
                    DebugLog3 << "Entry: " << entry.filename <<  std::endl;
                    bool matched = std::regex_match(entry.filename, pattern);
                    
                    // Do not recurse
                    /*
                    if (entry.isDirectory && matched){
                        //DebugLog2 << "Searching directory: " << entry.path() << std::endl;
                        DebugLog2 << "Searching directory: " << entry.filename << std::endl;
                        //doMatches(entry.path().string());
                        doMatches(entry.path);
                    //} else if (fs::is_regular_file(entry) && std::regex_match(relative_path.string(), pattern)) {
                    } else if (matched){
                    */
                    if (matched){
                        DebugLog3 << "Found pattern match on entry: " << entry.filename << std::endl;
                        matches.emplace_back(entry.filename);
                    } else {
                        DebugLog3 << "No match found for string: " << entry.path << std::endl;
                    }
                }
            } catch (...){
                DebugLog2 << "Error parsing directories." << std::endl;
            }
        }
        // Function to iterate over a directory using Windows API
        const std::vector<File> getFiles(const std::string & path) {
            //DebugLog << "Starting get files from path: " << path << std::endl;
            std::vector<File> files;

            WIN32_FIND_DATA findFileData;
            HANDLE hFind = FindFirstFile((path + "\\*").c_str(), &findFileData);
            
            if (hFind != INVALID_HANDLE_VALUE) {
                do {
                    File file = { 
                                    std::string(findFileData.cFileName), 
                                    path + "/" + std::string(findFileData.cFileName), 
                                    (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0 
                                };
                    if (file.isDirectory && 
                        (file.filename == "." || file.filename == "..")) {
                        // Skip directories . and ..
                        continue;
                    }
                    //DebugLog << "File: " << file.filename << std::endl;
                    files.emplace_back(file);
                } while (FindNextFile(hFind, &findFileData) != 0);

                FindClose(hFind);
            } else {
                DebugLog2 << "Error opening directory: " << GetLastError() << std::endl;
            }
            return files;
        }

        std::string createRegex(const std::string & glob){
            std::string regexStr = "^";
            for (char ch : glob) {
                switch (ch) {
                    case '*':
                        regexStr += ".*";
                        break;
                    case '?':
                        regexStr += ".";
                        break;
                    case '.':
                        regexStr += "\\.";
                        break;
                    default:
                        regexStr += ch;
                }
            }
            regexStr += "$";
            DebugLog3 << "Regex string: " << regexStr << std::endl; 
            return regexStr;
        }

        std::regex pattern;
        std::string origin;
    };

    Globber glob(find, dataPath.path());
    for (std::string globFile : glob.matches){
        DebugLog2 << "Got datapath: " << dataPath.path() << " globFile: " << globFile << std::endl;
        files.emplace_back(AbsolutePath(dataPath.join(Filesystem::RelativePath(globFile))));
    }
    DebugLog2 << "Got total matched files: " << files.size() << std::endl;
    return files;
}

#endif