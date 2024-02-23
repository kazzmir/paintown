#ifdef MINPSPW

#include "r-tech1/funcs.h"
#include "r-tech1/file-system.h"
#include "r-tech1/thread.h"
#include "r-tech1/system.h"
#include "r-tech1/utf.h"

#include <pspiofilemgr.h>
#include <string>
#include <vector>
#include <string>
#include <cstring> // Include the header file for strcmp
#include <algorithm> // Include the header file for std::transform
#include <cctype> // Include the header file for ::tolower

using namespace std;
vector<Filesystem::AbsolutePath> Filesystem::getFiles(const AbsolutePath & dataPath, const string & find, bool caseInsensitive){
    std::vector<AbsolutePath> files;

    // Open the directory
    SceUID dir = sceIoDopen(dataPath.path().c_str());
    if (dir < 0) {
        // Error opening directory
        return files;
    }

    // Read directory entries
    SceIoDirent dirent;
    while (sceIoDread(dir, &dirent) > 0) {
        // Skip special entries
        if (std::strcmp(dirent.d_name, ".") == 0 || std::strcmp(dirent.d_name, "..") == 0) {
            continue;
        }

        // Check if the filename contains the search string
        std::string filename = dirent.d_name;
        if (caseInsensitive) {
            // Convert both filename and search string to lowercase for case-insensitive comparison
            std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
            std::string findLower = find;
            std::transform(findLower.begin(), findLower.end(), findLower.begin(), ::tolower);
            if (filename.find(findLower) != std::string::npos) {
                AbsolutePath ap = Filesystem::AbsolutePath(dataPath.path() + "/" + filename);
                files.push_back(ap);
            }
        } else {
            if (filename.find(find) != std::string::npos) {
                AbsolutePath ap = Filesystem::AbsolutePath(dataPath.path() + "/" + filename);
                files.push_back(ap);
            }
        }
    }

    // Close the directory
    sceIoDclose(dir);

    return files;
}
#endif
