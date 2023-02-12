#ifndef _nl_group_h
#define _nl_group_h

#include "hawklib.h"

#ifdef __cplusplus
extern "C" {
#endif

HL_EXP SOCKET HL_APIENTRY nlGroupGetFdset(NLint group, fd_set *fd);

#ifdef __cplusplus
}
#endif

#endif
