#if defined(PS3)

#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>

/* ps3 doesn't have an implementation of access()? if it gets one this function
 * can be removed.
 */
int access(const char * path, int mode){
    struct stat information;
    int ok = stat(path, &information);
    // printf("stat of '%s' is %d\n", path.c_str(), ok);
    if (ok == 0){
        if (mode == R_OK){
            if (((information.st_mode & S_IRUSR) == S_IRUSR) ||
                ((information.st_mode & S_IRGRP) == S_IRGRP) ||
                ((information.st_mode & S_IROTH) == S_IROTH)){
                return 0;
            } else {
            /* handle other modes if they become useful to us */
                return -1;
            }
       } else {
           return -1;
       }
    } else {
        // perror("stat");
        return -1;
    }
}

#endif