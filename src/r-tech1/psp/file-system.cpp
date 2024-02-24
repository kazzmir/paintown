#ifdef MINPSPW

#include "r-tech1/funcs.h"
#include "r-tech1/file-system.h"
#include "r-tech1/thread.h"
#include "r-tech1/system.h"
#include "r-tech1/utf.h"

#include <pspiofilemgr.h>
#include <string>
#include <vector>
#include <stack>
#include <string>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <regex>

using namespace std;

vector<Filesystem::AbsolutePath> Filesystem::getFiles(const Filesystem::AbsolutePath & dataPath, const string & find, bool caseInsensitive){

        std::vector<Filesystem::AbsolutePath> files;
        std::stack<Filesystem::AbsolutePath> directories;

        directories.push(dataPath);

        while (!directories.empty()) {
            Filesystem::AbsolutePath currentDir = directories.top();
            directories.pop();

            // Open the directory
            SceUID dir = sceIoDopen(currentDir.path().c_str());
            if (dir < 0) {
                // Failed to open directory
                continue;
            }

            // Read files and directories in the directory
            SceIoDirent dirent;
            while (sceIoDread(dir, &dirent) > 0) {
                if (FIO_S_ISDIR(dirent.d_stat.st_mode)) {
                    // Skip directories "." and ".."
                    if (strcmp(dirent.d_name, ".") == 0 || strcmp(dirent.d_name, "..") == 0)
                        continue;

                    // Push subdirectory onto the stack for processing
                    directories.push(currentDir.join(Filesystem::RelativePath(dirent.d_name)));
                } else {
                    // Check if the file name matches the search criteria
                    std::string fileName = dirent.d_name;
                    if (caseInsensitive) {
                        // Convert both file name and search criteria to lowercase for case-insensitive comparison
                        std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
                        std::string findLower = find;
                        std::transform(findLower.begin(), findLower.end(), findLower.begin(), ::tolower);
                        std::regex regexPattern(findLower);
                        if (std::regex_search(fileName, regexPattern)) {
                            files.push_back(currentDir.join(Filesystem::RelativePath(dirent.d_name)));
                        }
                    } else {
                        // Case-sensitive comparison
                        std::regex regexPattern(find);
                        if (std::regex_search(fileName, regexPattern)) {
                            files.push_back(currentDir.join(Filesystem::RelativePath(dirent.d_name)));
                        }
                    }
                }
            }

            // Close the directory
            sceIoDclose(dir);
        }

        return files;
}

#endif
