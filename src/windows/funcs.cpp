#ifdef WINDOWS

#include <windows.h>
#include "util/funcs.h"
#include <io.h>
#include <fcntl.h>

int Util::getPipe(int files[2]){
    HANDLE read_in, write_in;
    int ok = CreatePipe(&read_in, &write_in, NULL, 0);
    if (ok == 0){
        return -1;
    }
    // files[0] = _open_osfhandle(read_in, O_RDONLY | O_BINARY);
    // files[1] = _open_osfhandle(write_in, O_WRONLY | O_BINARY);
    files[0] = _open_osfhandle((intptr_t) read_in, _O_RDONLY);
    files[1] = _open_osfhandle((intptr_t) write_in, _O_WRONLY);
    return 0;
}

#endif
