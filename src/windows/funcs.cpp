#ifdef WINDOWS

#include "util/funcs.h"
#include <io.h>

int Util::getPipe(int files[2]){
    PHANDLE read_in, write_in;
    int ok = CreatePipe(read_in, write_in, NULL, 0);
    if (ok == 0){
        return -1;
    }
    files[0] = _open_osfhandle(read_in, O_RDONLY | O_BINARY);
    files[1] = _open_osfhandle(write_in, O_WRONLY | O_BINARY);
    return 0;
}

#endif
