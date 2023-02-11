/*
  HawkNL cross platform network library
  Copyright (C) 2000-2004 Phil Frisbie, Jr. (phil@hawksoft.com)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA  02111-1307, USA.

  Or go to http://www.gnu.org/copyleft/lgpl.html
*/

#include <string.h>
#include "hawknl/nlinternal.h"
#include "hawknl/sock.h"
#include "hawknl/serial.h"
#include "hawknl/parallel.h"
#include "r-tech1/funcs.h"

#ifdef PS3
#include <net/net.h>
#endif

#ifdef WII
#include <network.h>
#endif

#ifdef NL_INCLUDE_LOOPBACK
#include "hawknl/loopback.h"
#endif

#if defined HL_WINDOWS_APP && defined NL_INCLUDE_IPX
  #include "hawknl/ipx.h"
#endif

#define MAX_NET_DRIVERS     6

volatile nl_state_t nlState = {NL_FALSE, NL_TRUE};

/* mutexes for global variables */
static Util::Thread::Lock socklock, instatlock, outstatlock;

static volatile NLboolean nlBlocking = NL_FALSE;

static volatile nl_stats_t nlInstats;
static volatile nl_stats_t nlOutstats;

static volatile NLsocket nlNextsocket = 0;
static volatile NLint nlNumsockets = 0;
static volatile NLint nlMaxNumsockets = NL_MIN_SOCKETS; /* this is dynamic, and can grow as needed */
static volatile NLint nlInitCount = 0;

pnl_socket_t *nlSockets = NULL;

/* the current selected driver */
static nl_netdriver_t /*@null@*/*driver = NULL;

static nl_netdriver_t netdrivers[] =
{
    {
        (NLchar*)TEXT("NL_IP"),
        (NLchar*)TEXT("NL_RELIABLE NL_UNRELIABLE NL_RELIABLE_PACKETS NL_BROADCAST NL_UDP_MULTICAST NL_MULTICAST"),
        NL_IP,
        NL_FALSE,
        sock_Init,
        sock_Shutdown,
        sock_Listen,
        sock_AcceptConnection,
        sock_Open,
        sock_Connect,
        sock_Close,
        sock_Read,
        sock_Write,
        sock_AddrToString,
        sock_StringToAddr,
        sock_GetLocalAddr,
        sock_GetAllLocalAddr,
        sock_SetLocalAddr,
        sock_GetNameFromAddr,
        sock_GetNameFromAddrAsync,
        sock_GetAddrFromName,
        sock_GetAddrFromNameAsync,
        sock_AddrCompare,
        sock_GetPortFromAddr,
        sock_SetAddrPort,
        sock_GetSystemError,
        sock_PollGroup,
        sock_PollSocket,
        sock_Hint,
        sock_SetSocketOpt,
        sock_GetSocketOpt
    }
#ifdef NL_INCLUDE_LOOPBACK
    ,
    {
        (NLchar*)TEXT("NL_LOOP_BACK"),
        (NLchar*)TEXT("NL_RELIABLE NL_UNRELIABLE NL_RELIABLE_PACKETS NL_BROADCAST"),
        NL_LOOP_BACK,
        NL_FALSE,
        loopback_Init,
        loopback_Shutdown,
        loopback_Listen,
        loopback_AcceptConnection,
        loopback_Open,
        loopback_Connect,
        loopback_Close,
        loopback_Read,
        loopback_Write,
        loopback_AddrToString,
        loopback_StringToAddr,
        loopback_GetLocalAddr,
        loopback_GetAllLocalAddr,
        loopback_SetLocalAddr,
        loopback_GetNameFromAddr,
        loopback_GetNameFromAddrAsync,
        loopback_GetAddrFromName,
        loopback_GetAddrFromNameAsync,
        loopback_AddrCompare,
        loopback_GetPortFromAddr,
        loopback_SetAddrPort,
        loopback_GetSystemError,
        loopback_PollGroup,
        loopback_PollSocket,
        loopback_Hint,
        loopback_SetSocketOpt,
        loopback_GetSocketOpt
    }
#endif /* NL_INCLUDE_LOOPBACK */
#if defined HL_WINDOWS_APP && defined NL_INCLUDE_IPX
    ,
    {
        (NLchar*)TEXT("NL_IPX"),
        (NLchar*)TEXT("NL_RELIABLE NL_UNRELIABLE NL_RELIABLE_PACKETS NL_BROADCAST"),
        NL_IPX,
        NL_FALSE,
        ipx_Init,
        ipx_Shutdown,
        sock_Listen,
        ipx_AcceptConnection,
        ipx_Open,
        sock_Connect,
        sock_Close,
        sock_Read,
        sock_Write,
        ipx_AddrToString,
        ipx_StringToAddr,
        ipx_GetLocalAddr,
        ipx_GetAllLocalAddr,
        ipx_SetLocalAddr,
        ipx_GetNameFromAddr,
        ipx_GetNameFromAddrAsync,
        ipx_GetAddrFromName,
        ipx_GetAddrFromNameAsync,
        ipx_AddrCompare,
        ipx_GetPortFromAddr,
        ipx_SetAddrPort,
        sock_GetSystemError,
        sock_PollGroup,
        sock_PollSocket,
        ipx_Hint,
        ipx_SetSocketOpt,
        ipx_GetSocketOpt
    }
#endif /* HL_WINDOWS_APP && NL_INCLUDE_IPX */
    ,
    {
        (NLchar*)NULL,
    }
};

/*

 Internal helper functions.

*/

static NLboolean isSafeString(const NLchar *string)
{
    int i;
    NLboolean   nullfound = NL_FALSE;

    /* make sure string is null terminated at less than NL_MAX_STRING_LENGTH */
    for(i=0;i<NL_MAX_STRING_LENGTH;i++)
    {
        if(string[i] == (NLchar)'\0')
        {
            nullfound = NL_TRUE;
            break;
        }
    }
    if(nullfound == NL_FALSE)
    {
        return NL_FALSE;
    }
    /* check for formating characters */
    if(_tcsrchr(string, '%') != NULL)
    {
        return NL_FALSE;
    }
    return NL_TRUE;
}

static void safecat(NLchar *dest, const NLchar *src)
{
    int len;

    if(isSafeString(dest) != NL_TRUE || isSafeString(src) != NL_TRUE)
    {
        /* don't do anything */
        return;
    }
    len = (int)_tcslen(dest);
    if( len < (NL_MAX_STRING_LENGTH - 1))
    {
        _tcsncat(dest, src, (size_t)(NL_MAX_STRING_LENGTH - len));
        dest[NL_MAX_STRING_LENGTH - 1] = (NLchar)'\0';
    }
}

NLsocket nlGetNewSocket(void)
{
    NLsocket    newsocket = NL_INVALID;
    nl_socket_t *sock = NULL;

    if(Util::Thread::acquireLock(&socklock) != 0)
    {
        nlSetError(NL_SYSTEM_ERROR);
        return NL_INVALID;
    }
    if(nlNumsockets == nlMaxNumsockets)
    {
        nl_socket_t **temp;
        NLint       tempmaxnumsockets = nlMaxNumsockets;

        /* expand the list of sockets pointers */
        tempmaxnumsockets *= 2;
        temp = (nl_socket_t **)realloc((void *)nlSockets, tempmaxnumsockets * sizeof(nl_socket_t *));
        if(temp == NULL)
        {
            Util::Thread::releaseLock(&socklock);
            nlSetError(NL_OUT_OF_MEMORY);
            return NL_INVALID;
        }
        nlSockets = temp;
        nlMaxNumsockets = tempmaxnumsockets;
    }
    /* get a socket number */
    if(nlNumsockets == (NLint)nlNextsocket)
    {
        newsocket = nlNextsocket++;
        /* allocate the memory */
        sock = (nl_socket_t *)malloc(sizeof(nl_socket_t));
        if(sock == NULL)
        {
            Util::Thread::releaseLock(&socklock);
            nlSetError(NL_OUT_OF_MEMORY);
            return NL_INVALID;
        }
        else
        {
            nlSockets[newsocket] = sock;
        }
        /* clear the structure */
        memset(sock, 0, sizeof(nl_socket_t));

        if(Util::Thread::initializeLock(&sock->readlock) != true || Util::Thread::initializeLock(&sock->writelock) != true)
        {
            nlSetError(NL_SYSTEM_ERROR);
            Util::Thread::releaseLock(&socklock);
            return NL_INVALID;
        }
    }
    else
    /* there is an open socket slot somewhere below nlNextsocket */
    {
        NLsocket    i;
        Util::Thread::Lock readlock, writelock;

        for(i=0;i<nlNextsocket;i++)
        {
            if(nlSockets[i]->inuse == NL_FALSE)
            {
                /* found an open socket slot */
                newsocket = i;
                sock = nlSockets[i];
                break;
            }
        }
        /* let's check just to make sure we did find a socket */
        if(sock == NULL)
        {
            Util::Thread::releaseLock(&socklock);
            nlSetError(NL_OUT_OF_MEMORY);
            return NL_INVALID;
        }
        readlock = sock->readlock;
        writelock = sock->writelock;
        /* clear the structure */
        memset(sock, 0, sizeof(nl_socket_t));
        sock->readlock = readlock;
        sock->writelock = writelock;
    }

    /* sockets are blocking until set for non-blocking */
    sock->blocking = nlBlocking;
    sock->inuse = NL_TRUE;
    nlNumsockets++;
    Util::Thread::releaseLock(&socklock);
    return newsocket;
}

static void nlReturnSocket(NLsocket socket)
{
    nl_socket_t     *sock = nlSockets[socket];

    if((sock != NULL) && (sock->inuse == NL_TRUE))
    {
        sock->inuse = NL_FALSE;
        if(sock->inbuf != NULL)
        {
            free(sock->inbuf);
            sock->inbuf = NULL;
        }
        if(sock->outbuf != NULL)
        {
            free(sock->outbuf);
            sock->outbuf = NULL;
        }
        nlNumsockets--;
    }
}

void nlFreeSocket(NLsocket socket)
{
    nl_socket_t     *sock = nlSockets[socket];

    if(sock != NULL)
    {
        if(sock->inbuf != NULL)
        {
            free(sock->inbuf);
        }
        if(sock->outbuf != NULL)
        {
            free(sock->outbuf);
        }
        Util::Thread::destroyLock(&sock->readlock);
        Util::Thread::destroyLock(&sock->writelock);
        free(sock);
    }
}

NLboolean nlIsValidSocket(NLsocket socket)
{
    nl_socket_t *sock;

    if(socket < 0 || socket > nlMaxNumsockets)
    {
        nlSetError(NL_INVALID_SOCKET);
        return NL_FALSE;
    }
    sock = nlSockets[socket];
    if(sock == NULL)
    {
        nlSetError(NL_INVALID_SOCKET);
        return NL_FALSE;
    }
    if(sock->inuse == NL_FALSE)
    {
        nlSetError(NL_INVALID_SOCKET);
        return NL_FALSE;
    }
    return NL_TRUE;
}

NLboolean nlLockSocket(NLsocket socket, NLint which)
{
    nl_socket_t     *sock = nlSockets[socket];

    if((which&NL_READ) > 0)
    {
        if(Util::Thread::acquireLock(&sock->readlock) != 0)
        {
            nlSetError(NL_SYSTEM_ERROR);
            return NL_FALSE;
        }
    }
    if((which&NL_WRITE) > 0)
    {
        if(Util::Thread::acquireLock(&sock->writelock) != 0)
        {
            if((which&NL_READ) > 0)
            {
                Util::Thread::releaseLock(&sock->readlock);
            }
            nlSetError(NL_SYSTEM_ERROR);
            return NL_FALSE;
        }
    }
    return NL_TRUE;
}

void nlUnlockSocket(NLsocket socket, NLint which)
{
    nl_socket_t     *sock = nlSockets[socket];

    if((which&NL_WRITE) > 0){
        Util::Thread::releaseLock(&sock->writelock);
    }
    if((which&NL_READ) > 0)
    {
        Util::Thread::releaseLock(&sock->readlock);
    }
}

static void nlUpdateStats(volatile nl_stats_t *stats, NLint nbytes, NLint npackets)
{
    time_t  t;

    (void)time(&t);
    if(stats->stime == 0)
    {
        /* must be the first time through */
        stats->stime = t;
        stats->lastbucket = -1;
        stats->firstround = NL_TRUE;
    }
    /* do the basic update */
    stats->packets += npackets;
    stats->bytes += nbytes;

    /* check to see if we need to do the full update */
    if(stats->stime != t)
    {
        NLint   i;
        NLlong  count = 0;
        time_t  diff = t - stats->stime;

        stats->stime = t;

        if(stats->curbytes > stats->highest)
        {
            stats->highest = stats->curbytes;
        }
        if(diff >= NL_NUM_BUCKETS)
        {
            diff = NL_NUM_BUCKETS;
        }

        while(diff > 1)
        {
            /* we need to zero out skipped over buckets */
            stats->lastbucket++;
            if(stats->lastbucket == NL_NUM_BUCKETS)
            {
                stats->lastbucket = 0;
            }
            stats->bucket[stats->lastbucket] = 0;
            diff--;
        }
        stats->lastbucket++;
        if(stats->lastbucket == NL_NUM_BUCKETS)
        {
            stats->lastbucket = 0;
            stats->firstround = NL_FALSE;
        }
        stats->bucket[stats->lastbucket] = stats->curbytes;
        if(stats->firstround == NL_TRUE)
        {
            /* this corrects the stats for the first second */
            for(i=stats->lastbucket + 1;i<NL_NUM_BUCKETS;i++)
            {
                stats->bucket[i] = stats->curbytes;
            }
        }
        stats->curbytes = 0;

        for(i=0;i<NL_NUM_BUCKETS;i++)
        {
            count += stats->bucket[i];
        }
        stats->average =  count / NL_NUM_BUCKETS;
    }
    stats->curbytes += nbytes;
}

static void nlUpdateInStats(NLint nbytes, NLint npackets)
{
    if(nlState.socketStats == NL_FALSE)
    {
        return;
    }
    Util::Thread::acquireLock(&instatlock);
    nlUpdateStats(&nlInstats, nbytes, npackets);
    Util::Thread::releaseLock(&instatlock);
}

static void nlUpdateOutStats(NLint nbytes, NLint npackets)
{
    if(nlState.socketStats == NL_FALSE)
    {
        return;
    }
    Util::Thread::acquireLock(&outstatlock);
    nlUpdateStats(&nlOutstats, nbytes, npackets);
    Util::Thread::releaseLock(&outstatlock);
}

static void nlUpdateSocketInStats(NLsocket socket, NLint nbytes, NLint npackets)
{
    nl_socket_t     *sock = nlSockets[socket];

    if(nlState.socketStats == NL_FALSE)
    {
        return;
    }
    nlUpdateStats(&sock->instats, nbytes, npackets);
}

static void nlUpdateSocketOutStats(NLsocket socket, NLint nbytes, NLint npackets)
{
    nl_socket_t     *sock = nlSockets[socket];

    if(nlState.socketStats == NL_FALSE)
    {
        return;
    }
    nlUpdateStats(&sock->outstats, nbytes, npackets);
}

/*

  Low level functions, a thin layer over Sockets.

*/

/*
   Trys to init all drivers, BUT DOES NOT SELECT A DRIVER
*/

HL_EXP NLboolean HL_APIENTRY nlInit(void)
{
    int i, numdrivers = 0;

#ifdef PS3
    netInitialize();
#endif

#ifdef WII
    net_init();
#endif

    nlSetError(NL_NO_ERROR);
    /* init socket memory, mutexes, and global variables */
    if(nlInitCount == 0)
    {
        nlMaxNumsockets = NL_MIN_SOCKETS;
        if(nlSockets == NULL)
        {
            nlSockets = (nl_socket_t **)malloc(nlMaxNumsockets * sizeof(nl_socket_t *));
        }
        if(nlSockets == NULL)
        {
            nlSetError(NL_OUT_OF_MEMORY);
            nlShutdown();
            return NL_FALSE;
        }
        if(nlGroupInit() == NL_FALSE)
        {
            nlShutdown();
            return NL_FALSE;
        }
        if(Util::Thread::initializeLock(&socklock) != true || Util::Thread::initializeLock(&instatlock) != true ||
            Util::Thread::initializeLock(&outstatlock) != true)
        {
            nlSetError(NL_SYSTEM_ERROR);
            nlShutdown();
            return NL_FALSE;
        }
        nlNumsockets = 0;
        nlNextsocket = 0;
        nlBlocking = NL_FALSE;
        nlState.socketStats = NL_FALSE;
        nlState.nl_big_endian_data = NL_TRUE;

        for(i=0;i<MAX_NET_DRIVERS;i++)
        {
            if(netdrivers[i].name == NULL)
            {
                break;
            }
            if(netdrivers[i].initialized == NL_TRUE)
            {
                numdrivers++;
            }
            else if(netdrivers[i].Init() == NL_TRUE)
            {
                netdrivers[i].initialized = NL_TRUE;
                numdrivers++;
            }
        }
        if(numdrivers == 0)
        {
            nlSetError(NL_NO_NETWORK);
            nlShutdown();
            return NL_FALSE;
        }
    }
    nlInitCount++;
    return NL_TRUE;
}

/*
   Called at the end of your program, shuts down the active driver and frees memory
*/

HL_EXP void HL_APIENTRY nlShutdown(void)
{
    --nlInitCount;
    
    if(nlInitCount > 0)
    {
        return;
    }
    if(driver != NULL)
    {
        /* close any open sockets */
        Util::Thread::acquireLock(&socklock);
        if(nlSockets != NULL)
        {
            NLsocket s;
            
            for(s=0;s<nlNextsocket;s++)
            {
                if(nlSockets[s] != NULL)
                {
                    if(nlIsValidSocket(s) == NL_TRUE)
                    {
                        driver->Close(s);
                        Util::rest(0);
                    }
                }
            }
        }
        /* now we can shutdown the driver */
        driver->Shutdown();
        driver->initialized = NL_FALSE;
        driver = NULL;
    }
    else
    {
        nlSetError(NL_NO_NETWORK);
    }
    
    Util::rest(1);
    
    /* now free all the socket structures */
    if(nlSockets != NULL)
    {
        NLsocket s;
        
        for(s=0;s<nlNextsocket;s++)
        {
            if(nlSockets[s] != NULL)
            {
                if(nlIsValidSocket(s) == NL_TRUE)
                {
                    (void)nlLockSocket(s, NL_BOTH);
                    nlReturnSocket(s);
                    nlUnlockSocket(s, NL_BOTH);
                    Util::rest(0);
                }
                nlFreeSocket(s);
            }
        }
        free(nlSockets);
        nlSockets = NULL;
    }
    Util::Thread::releaseLock(&socklock);
    nlGroupShutdown();
    /* destroy the mutexes */
    Util::Thread::destroyLock(&socklock);
    Util::Thread::destroyLock(&instatlock);
    Util::Thread::destroyLock(&outstatlock);
}

/*
   Enables a socket to listen for incomming connections
*/

HL_EXP NLboolean HL_APIENTRY nlListen(NLsocket socket)
{
    if(driver != NULL)
    {
        if(nlIsValidSocket(socket) == NL_TRUE)
        {
            NLboolean result;

            if(nlLockSocket(socket, NL_BOTH) == NL_FALSE)
            {
                return NL_FALSE;
            }
            result = driver->Listen(socket);
            nlUnlockSocket(socket, NL_BOTH);
            return result;
        }
        nlSetError(NL_INVALID_SOCKET);
        return NL_FALSE;
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

/*
   Accepts a pending connection.
   Creates a new socket object for this connection.
*/

HL_EXP NLsocket HL_APIENTRY nlAcceptConnection(NLsocket socket)
{
    if(driver)
    {
        if(nlIsValidSocket(socket) == NL_TRUE)
        {
            NLsocket newsocket;

            if(nlLockSocket(socket, NL_BOTH) == NL_FALSE)
            {
                return NL_INVALID;
            }
            newsocket = driver->AcceptConnection(socket);
            nlUnlockSocket(socket, NL_BOTH);
            if(newsocket != NL_INVALID)
            {
                /* the new socket was locked when it is created */
                nlUnlockSocket(newsocket, NL_BOTH);
            }
            return newsocket;
        }
        nlSetError(NL_INVALID_SOCKET);
        return NL_INVALID;
    }
    nlSetError(NL_NO_NETWORK);
    return NL_INVALID;
}

/*
   Creates a new socket object.
   Can be used for reading or broadcast as is.
   For non-broadcast use, call nlConnectSocket to connect to a remote address.
*/

HL_EXP NLsocket HL_APIENTRY nlOpen(NLushort port, NLenum type)
{
    if(driver)
    {
        return (driver->Open(port, type));
    }

    nlSetError(NL_NO_NETWORK);
    return NL_INVALID;
}

/*
   Connect a socket to a remote address.
*/

HL_EXP NLboolean HL_APIENTRY nlConnect(NLsocket socket, const NLaddress *address)
{
    if(driver)
    {
        if(nlIsValidSocket(socket) == NL_TRUE)
        {
            if(address == NULL)
            {
                nlSetError(NL_NULL_POINTER);
            }
            else
            {
                NLboolean result;

                if(nlLockSocket(socket, NL_BOTH) == NL_FALSE)
                {
                    return NL_FALSE;
                }
                result = driver->Connect(socket, address);
                nlUnlockSocket(socket, NL_BOTH);
                return result;
            }
        }
        else
        {
            nlSetError(NL_INVALID_SOCKET);
        }
        return NL_FALSE;
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

/*
   Close the socket.
*/

HL_EXP NLboolean HL_APIENTRY nlClose(NLsocket socket)
{
    if(driver)
    {
        if(nlIsValidSocket(socket) == NL_TRUE)
        {
            if(Util::Thread::acquireLock(&socklock) != 0)
            {
                nlSetError(NL_SYSTEM_ERROR);
                return NL_FALSE;
            }
	    /*
            if(nlLockSocket(socket, NL_BOTH) == NL_FALSE)
            {
                return NL_FALSE;
            }
	    */
            driver->Close(socket);
            /* return the socket for reuse */
            nlReturnSocket(socket);
            // nlUnlockSocket(socket, NL_BOTH);
            if(Util::Thread::releaseLock(&socklock) != 0)
            {
                nlSetError(NL_SYSTEM_ERROR);
                return NL_FALSE;
            }
            return NL_TRUE;
        }
        else
        {
            nlSetError(NL_INVALID_SOCKET);
            return NL_TRUE;
        }
    }

#ifdef WII
    net_deinit();
#endif

#ifdef PS3
    netDeinitialize();
#endif

    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

/*
   Reads from a socket.
*/

HL_EXP NLint HL_APIENTRY nlRead(NLsocket socket, NLvoid *buffer, NLint nbytes)
{
    if(driver)
    {
        if(nlIsValidSocket(socket) == NL_TRUE)
        {
            if(buffer == NULL)
            {
                nlSetError(NL_NULL_POINTER);
            }
            else
            {
                NLint received;

                if(nlLockSocket(socket, NL_READ) == NL_FALSE)
                {
                    return NL_INVALID;
                }
                received = driver->Read(socket, buffer, nbytes);

                if(received > 0)
                {
                    nlUpdateSocketInStats(socket, received, 1);
                    nlUpdateInStats(received, 1);
                }
                nlUnlockSocket(socket, NL_READ);
                return received;
            }
        }
        else
        {
            nlSetError(NL_INVALID_SOCKET);
        }
        return NL_INVALID;
    }
    nlSetError(NL_NO_NETWORK);
    return NL_INVALID;
}

/*
   Writes to a socket.
*/

HL_EXP NLint HL_APIENTRY nlWrite(NLsocket socket, const NLvoid *buffer, NLint nbytes)
{
    if(driver)
    {
        /* check for group */
        if(socket >= NL_FIRST_GROUP)
        {
            NLint       number = NL_MAX_GROUP_SOCKETS;
            NLsocket    s[NL_MAX_GROUP_SOCKETS];
            NLint       i;
            NLint       sent = nbytes;

            if(nlGroupGetSockets((NLint)socket, (NLsocket *)s, &number) == NL_FALSE)
            {
                return NL_INVALID;
            }

            for(i=0;i<number;i++)
            {
                NLint result;

                if(nlIsValidSocket(s[i]) == NL_TRUE)
                {
                    result = nlWrite(s[i], buffer, nbytes);
                    if(result < sent)
                    {
                        sent = result;
                    }
                }
            }
            return sent;
        }
        else
        {
            if(nlIsValidSocket(socket) == NL_TRUE)
            {
                if(buffer == NULL)
                {
                    nlSetError(NL_NULL_POINTER);
                }
                else
                {
                    NLint sent;

                    if(nlLockSocket(socket, NL_WRITE) == NL_FALSE)
                    {
                        return NL_INVALID;
                    }
                    sent = driver->Write(socket, buffer, nbytes);
                    if(sent > 0)
                    {
                        nlUpdateSocketOutStats(socket, sent, 1);
                        nlUpdateOutStats(sent, 1);
                    }
                    nlUnlockSocket(socket, NL_WRITE);
                    return sent;
                }
            }
            else
            {
                nlSetError(NL_INVALID_SOCKET);
            }
            return NL_INVALID;
        }
    }
    nlSetError(NL_NO_NETWORK);
    return NL_INVALID;
}

/*
   Polls all sockets in the group to see which have data waiting to be read.
   nlPollGroup uses select() on TCP or UDP sockets.
   Returns number of sockets waiting, and a list of those sockets, or NL_INVALID
   on an error.
*/

HL_EXP NLint HL_APIENTRY nlPollGroup(NLint group, NLenum name, /*@out@*/ NLsocket *sockets, NLint number, NLint timeout)
{
    if(driver)
    {
        if(sockets == NULL )
        {
            nlSetError(NL_NULL_POINTER);
            return 0;
        }
        return (driver->PollGroup(group, name, sockets, number, timeout));
    }
    nlSetError(NL_NO_NETWORK);
    return 0;
}

HL_EXP NLboolean HL_APIENTRY nlPollSocket(NLsocket socket, NLenum name, NLint timeout)
{
    if(driver)
    {
        if(nlIsValidSocket(socket) == NL_TRUE )
        {
            return (driver->PollSocket(socket, name, timeout));
        }
        else
        {
            nlSetError(NL_INVALID_SOCKET);
            return NL_FALSE;
        }
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

HL_EXP NLboolean HL_APIENTRY nlHint(NLenum name, NLint arg)
{
    if(driver)
    {
        return driver->Hint(name, arg);
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

HL_EXP NLboolean HL_APIENTRY nlSetSocketOpt(NLsocket socket, NLenum name, NLint arg)
{
    if(driver)
    {
        if(nlIsValidSocket(socket) == NL_TRUE )
        {
            return driver->SetSocketOpt(socket, name, arg);
        }
        else
        {
            nlSetError(NL_INVALID_SOCKET);
            return NL_FALSE;
        }
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

HL_EXP NLint HL_APIENTRY nlGetSocketOpt(NLsocket socket, NLenum name)
{
    if(driver)
    {
        if(nlIsValidSocket(socket) == NL_TRUE )
        {
            return driver->GetSocketOpt(socket, name);
        }
        else
        {
            nlSetError(NL_INVALID_SOCKET);
            return NL_INVALID;
        }
    }
    nlSetError(NL_NO_NETWORK);
    return NL_INVALID;
}

/*
   Converts the numeric address in the NLaddress structure to a string.
*/

HL_EXP /*@null@*/ NLchar* HL_APIENTRY nlAddrToString(const NLaddress *address, NLchar *string)
{
    if(driver)
    {
        if((string == NULL) || (address == NULL))
        {
            nlSetError(NL_NULL_POINTER);
            return NULL;
        }
        return (driver->AddrToString(address, string));
    }
    nlSetError(NL_NO_NETWORK);
    return NULL;
}

/*
   Takes a string that contains a full network address (ie, for IP 192.168.0.1:27000),
   and adds it to the NLaddress structure.
*/

HL_EXP NLboolean HL_APIENTRY nlStringToAddr(const NLchar *string, NLaddress *address)
{
    if(driver)
    {
        if((string == NULL) || (address == NULL))
        {
            nlSetError(NL_NULL_POINTER);
            return NL_FALSE;
        }
        if(isSafeString(string) == NL_FALSE)
        {
            nlSetError(NL_STRING_OVER_RUN);
            return NL_FALSE;
        }
        return driver->StringToAddr(string, address);
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

/*
   Gets the remote address of the socket if connected to a remote host,
   or the local address if not connected.
*/

/* Note: the drivers put a copy of address in nl_socket_t, so we just need to copy it */
HL_EXP NLboolean HL_APIENTRY nlGetRemoteAddr(NLsocket socket, NLaddress *address)
{
    if(driver)
    {
        if(address == NULL)
        {
            nlSetError(NL_NULL_POINTER);
            return NL_FALSE;
        }
        if(nlIsValidSocket(socket) == NL_TRUE)
        {
            nl_socket_t *sock = nlSockets[socket];

            if(nlLockSocket(socket, NL_READ) == NL_FALSE)
            {
                return NL_FALSE;
            }
            memcpy(address, &sock->addressin, sizeof(NLaddress));
			address->valid = NL_TRUE;
            nlUnlockSocket(socket, NL_READ);
        }
        else
        {
            nlSetError(NL_INVALID_SOCKET);
            memset(address, 0, sizeof(NLaddress));
            return NL_FALSE;
        }
        return NL_TRUE;
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

/*
   Sets the remote address of an unconnected UDP socket.
*/

HL_EXP NLboolean HL_APIENTRY nlSetRemoteAddr(NLsocket socket, const NLaddress *address)
{
    if(driver)
    {
        if(nlIsValidSocket(socket) == NL_TRUE)
        {
            if(address == NULL)
            {
                nlSetError(NL_NULL_POINTER);
                return NL_FALSE;
            }
            else
            {
                nl_socket_t *sock = nlSockets[socket];

                if(nlLockSocket(socket, NL_WRITE) == NL_FALSE)
                {
                    return NL_FALSE;
                }
                memcpy(&sock->addressout, address, sizeof(NLaddress));
                nlUnlockSocket(socket, NL_WRITE);
            }
        }
        else
        {
            nlSetError(NL_INVALID_SOCKET);
            return NL_FALSE;
        }
        return NL_TRUE;
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

/*
   Gets the local address.
*/

HL_EXP NLboolean HL_APIENTRY nlGetLocalAddr(NLsocket socket, NLaddress *address)
{
    if(driver)
    {
        if(nlIsValidSocket(socket) == NL_TRUE)
        {
            if(address == NULL)
            {
                nlSetError(NL_NULL_POINTER);
                return NL_FALSE;
            }
            if(nlLockSocket(socket, NL_READ) == NL_FALSE)
            {
                return NL_FALSE;
            }
            if(driver->GetLocalAddr(socket, address) != NL_TRUE)
            {
                nlUnlockSocket(socket, NL_READ);
                return NL_FALSE;
            }
            nlUnlockSocket(socket, NL_READ);
        }
        else
        {
            nlSetError(NL_INVALID_SOCKET);
            return NL_FALSE;
        }
        return NL_TRUE;
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

HL_EXP NLaddress * HL_APIENTRY nlGetAllLocalAddr(/*@out@*/ NLint *count)
{
    if(driver)
    {
        if(count == NULL)
        {
            nlSetError(NL_NULL_POINTER);
            return NULL;
        }
        return driver->GetAllLocalAddr(count);
    }
    nlSetError(NL_NO_NETWORK);
    return NULL;
}

HL_EXP NLboolean HL_APIENTRY nlSetLocalAddr(const NLaddress *address)
{
    if(driver)
    {
        if(address == NULL)
        {
            nlSetError(NL_NULL_POINTER);
            return NL_FALSE;
        }
        return driver->SetLocalAddr(address);
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

/*
   Resolves the name from the address.
*/

HL_EXP /*@null@*/ NLchar* HL_APIENTRY nlGetNameFromAddr(const NLaddress *address, NLchar *name)
{
    if(driver)
    {
        if((name == NULL) || (address == NULL))
        {
            nlSetError(NL_NULL_POINTER);
            return NULL;
        }
        return (driver->GetNameFromAddr(address, name));
    }
    nlSetError(NL_NO_NETWORK);
    return NULL;
}

/*
   Resolves the name from the address asynchronously.
*/

HL_EXP NLboolean HL_APIENTRY nlGetNameFromAddrAsync(const NLaddress *address, NLchar *name)
{
    if(driver)
    {
        if((name == NULL) || (address == NULL))
        {
            nlSetError(NL_NULL_POINTER);
            return NL_FALSE;
        }
        return driver->GetNameFromAddrAsync(address, name);
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

/*
   Get the address from a host name.
*/

HL_EXP NLboolean HL_APIENTRY nlGetAddrFromName(const NLchar *name, NLaddress *address)
{
    if(driver)
    {
        if((name == NULL) || (address == NULL))
        {
            nlSetError(NL_NULL_POINTER);
            return NL_FALSE;
        }
        if(isSafeString(name) == NL_FALSE)
        {
            nlSetError(NL_STRING_OVER_RUN);
            return NL_FALSE;
        }
        return driver->GetAddrFromName(name, address);
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

/*
   Get the address from a host name asynchronously.
*/

HL_EXP NLboolean HL_APIENTRY nlGetAddrFromNameAsync(const NLchar *name, NLaddress *address)
{
    if(driver)
    {
        if((name == NULL) || (address == NULL))
        {
            nlSetError(NL_NULL_POINTER);
            return NL_FALSE;
        }
        if(isSafeString(name) == NL_FALSE)
        {
            nlSetError(NL_STRING_OVER_RUN);
            return NL_FALSE;
        }
        return driver->GetAddrFromNameAsync(name, address);
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

/*
   Compare two addresses.
*/

HL_EXP NLboolean HL_APIENTRY nlAddrCompare(const NLaddress *address1, const NLaddress *address2)
{
    if(driver)
    {
        if((address1 == NULL) || (address2 == NULL))
        {
            nlSetError(NL_NULL_POINTER);
            return NL_FALSE;
        }
        return driver->AddrCompare(address1, address2);
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}

/*
   Get the port number from an address.
*/

HL_EXP NLushort HL_APIENTRY nlGetPortFromAddr(const NLaddress *address)
{
    if(driver)
    {
        if(address == NULL)
        {
            nlSetError(NL_NULL_POINTER);
            return 0;
        }
        return driver->GetPortFromAddr(address);
    }
    nlSetError(NL_NO_NETWORK);
    return 0;
}

/*
   Set the port number in the address.
*/

HL_EXP NLboolean HL_APIENTRY nlSetAddrPort(NLaddress *address, NLushort port)
{
    if(driver)
    {
        if(address == NULL)
        {
            nlSetError(NL_NULL_POINTER);
            return NL_FALSE;
        }
        driver->SetAddrPort(address, port);
        return NL_TRUE;
    }
    nlSetError(NL_NO_NETWORK);
    return NL_FALSE;
}


/*

  Non-socket functions

*/

/*
   Select the network to use.
*/

HL_EXP NLboolean HL_APIENTRY nlSelectNetwork(NLenum network)
{
    int i, found = 0;

    if(driver != NULL)
    {
        /* we cannot select a new network without shutting down first */
        nlSetError(NL_SELECT_NET_ERROR);
        return NL_FALSE;
    }

    for(i=0;i<MAX_NET_DRIVERS;i++)
    {
        if(netdrivers[i].name == NULL)
        {
            break;
        }
        if(netdrivers[i].type == network)
        {
            found++;
            if(netdrivers[i].initialized == NL_TRUE)
            {
                driver = &netdrivers[i];
                return NL_TRUE;
            }
        }
    }
    if(found > 0)
    {
        nlSetError(NL_INVALID_TYPE);
    }
    else
    {
        nlSetError(NL_INVALID_ENUM);
    }
    return NL_FALSE;
}

/*
   Returns a string corresponding to the NLenum.
*/

HL_EXP const /*@observer@*//*@null@*/ NLchar* HL_APIENTRY nlGetString(NLenum name)
{
	/* use seperate strings for thread safety */
    static NLchar   vstring[NL_MAX_STRING_LENGTH];
    static NLchar   tstring[NL_MAX_STRING_LENGTH];
    NLint i;

    nlSetError(NL_NO_ERROR);
    /* intitialize the version string */
    _tcsncpy(vstring, (NLchar *)TEXT(NL_VERSION_STRING), (size_t)NL_MAX_STRING_LENGTH);
    vstring[NL_MAX_STRING_LENGTH - 1] = (NLchar) '\0';
#ifdef _UNICODE
    /* add the UNICODE string */
    safecat(vstring, (NLchar *)TEXT(" UNICODE version"));
#endif
    /* intitialize the network types string */
    memset(tstring, 0, sizeof(NLchar) * NL_MAX_STRING_LENGTH);
    for(i=0;i<MAX_NET_DRIVERS;i++)
    {
        if(netdrivers[i].name == NULL)
        {
            break;
        }
        if(netdrivers[i].initialized == NL_TRUE)
        {
            safecat((NLchar *)tstring, (const NLchar *)netdrivers[i].name);
            safecat((NLchar *)tstring, (NLchar *)TEXT(" "));
        }
    }

    switch (name) {

    case NL_VERSION:
		return (const NLchar*)vstring;

    case NL_NETWORK_TYPES:
		return (const NLchar*)tstring;

    case NL_SOCKET_TYPES:
        if(driver != NULL)
        {
		    return (const NLchar*)(driver->socktypes);
        }
        else
        {
            nlSetError(NL_NO_NETWORK);
        }
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
    }

    return NULL;
}

/*
   Returns an integer corresponding to the NLenum.
*/

HL_EXP NLlong HL_APIENTRY nlGetInteger(NLenum name)
{
    switch (name) {

    case NL_PACKETS_SENT:
        return nlOutstats.packets;

    case NL_BYTES_SENT:
        return nlOutstats.bytes;

    case NL_AVE_BYTES_SENT:
        nlUpdateOutStats(0, 0);
        return nlOutstats.average;

    case NL_HIGH_BYTES_SENT:
        return nlOutstats.highest;

    case NL_PACKETS_RECEIVED:
        return nlInstats.packets;

    case NL_BYTES_RECEIVED:
        return nlInstats.bytes;

    case NL_AVE_BYTES_RECEIVED:
        nlUpdateInStats(0, 0);
        return nlInstats.average;

    case NL_HIGH_BYTES_RECEIVED:
        return nlInstats.highest;

    case NL_OPEN_SOCKETS:
        return nlNumsockets;

    default:
        nlSetError(NL_INVALID_ENUM);
    }
    return 0;
}

/*
   Clears the stat corresponding to the NLenum.
*/

HL_EXP NLboolean HL_APIENTRY nlClear(NLenum name)
{
    switch (name) {

    case NL_PACKETS_SENT:
        if(Util::Thread::acquireLock(&outstatlock) != 0){
            Util::Thread::releaseLock(&outstatlock);
            nlSetError(NL_SYSTEM_ERROR);
            return NL_FALSE;
        }
        nlOutstats.packets = 0;
        Util::Thread::releaseLock(&outstatlock);
        break;

    case NL_BYTES_SENT:
        if(Util::Thread::acquireLock(&outstatlock) != 0){
            Util::Thread::releaseLock(&outstatlock);
            nlSetError(NL_SYSTEM_ERROR);
            return NL_FALSE;
        }
        nlOutstats.bytes = 0;
        Util::Thread::releaseLock(&outstatlock);
        break;

    case NL_AVE_BYTES_SENT:
        if(Util::Thread::acquireLock(&outstatlock) != 0){
            Util::Thread::releaseLock(&outstatlock);
            nlSetError(NL_SYSTEM_ERROR);
            return NL_FALSE;
        }
        nlOutstats.average = 0;
        memset((NLbyte *)nlOutstats.bucket, 0, sizeof(NLlong) * NL_NUM_BUCKETS);
        Util::Thread::releaseLock(&outstatlock);
        break;

    case NL_HIGH_BYTES_SENT:
        if(Util::Thread::acquireLock(&outstatlock) != 0){
            Util::Thread::releaseLock(&outstatlock);
            nlSetError(NL_SYSTEM_ERROR);
            return NL_FALSE;
        }
        nlOutstats.highest = 0;
        Util::Thread::releaseLock(&outstatlock);
        break;

    case NL_PACKETS_RECEIVED:
        if(Util::Thread::acquireLock(&instatlock) != 0){
            Util::Thread::releaseLock(&instatlock);
            nlSetError(NL_SYSTEM_ERROR);
            return NL_FALSE;
        }
        nlInstats.packets = 0;
        Util::Thread::releaseLock(&instatlock);
        break;

    case NL_BYTES_RECEIVED:
        if(Util::Thread::acquireLock(&instatlock) != 0){
            Util::Thread::releaseLock(&instatlock);
            nlSetError(NL_SYSTEM_ERROR);
            return NL_FALSE;
        }
        nlInstats.bytes = 0;
        Util::Thread::releaseLock(&instatlock);
        break;

    case NL_AVE_BYTES_RECEIVED:
        if(Util::Thread::acquireLock(&instatlock) != 0){
            Util::Thread::releaseLock(&instatlock);
            nlSetError(NL_SYSTEM_ERROR);
            return NL_FALSE;
        }
        nlInstats.average = 0;
        memset((NLbyte *)nlInstats.bucket, 0, sizeof(NLlong) * NL_NUM_BUCKETS);
        Util::Thread::releaseLock(&instatlock);
        break;

    case NL_HIGH_BYTES_RECEIVED:
        if(Util::Thread::acquireLock(&instatlock) != 0){
            Util::Thread::releaseLock(&instatlock);
            nlSetError(NL_SYSTEM_ERROR);
            return NL_FALSE;
        }
        nlInstats.highest = 0;
        Util::Thread::releaseLock(&instatlock);
        break;

    case NL_ALL_STATS:
        (void)nlClear(NL_PACKETS_SENT);
        (void)nlClear(NL_BYTES_SENT);
        (void)nlClear(NL_AVE_BYTES_SENT);
        (void)nlClear(NL_HIGH_BYTES_SENT);
        (void)nlClear(NL_PACKETS_RECEIVED);
        (void)nlClear(NL_BYTES_RECEIVED);
        (void)nlClear(NL_AVE_BYTES_RECEIVED);
        (void)nlClear(NL_HIGH_BYTES_RECEIVED);
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
        return NL_FALSE;
    }
    return NL_TRUE;
}

/*
   Get the socket or system error.
*/

HL_EXP NLint HL_APIENTRY nlGetSystemError(void)
{
    if(driver)
    {
        return driver->GetSystemError();
    }
    return NL_NO_NETWORK;
}

HL_EXP NLboolean HL_APIENTRY nlEnable(NLenum name)
{
    switch (name) {

    case NL_BLOCKING_IO:
        nlBlocking = NL_TRUE;
        break;

    case NL_REUSE_ADDRESS:
        return nlHint(NL_REUSE_ADDRESS, (NLint) NL_TRUE);

    case NL_TCP_NO_DELAY:
        return nlHint(NL_TCP_NO_DELAY, (NLint)NL_TRUE);

    case NL_SOCKET_STATS:
        nlState.socketStats = NL_TRUE;
        break;

    case NL_BIG_ENDIAN_DATA:
        nlState.nl_big_endian_data = NL_TRUE;
        break;

    case NL_LITTLE_ENDIAN_DATA:
        nlState.nl_big_endian_data = NL_FALSE;
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
        return NL_FALSE;
    }
    return NL_TRUE;
}

HL_EXP NLboolean HL_APIENTRY nlDisable(NLenum name)
{
    switch (name) {

    case NL_BLOCKING_IO:
        nlBlocking = NL_FALSE;
        break;
    
    case NL_REUSE_ADDRESS:
        return nlHint(NL_REUSE_ADDRESS, (NLint) NL_FALSE);

    case NL_TCP_NO_DELAY:
        return nlHint(NL_TCP_NO_DELAY, NL_FALSE);

    case NL_SOCKET_STATS:
        nlState.socketStats = NL_FALSE;
        break;

    case NL_BIG_ENDIAN_DATA:
        nlState.nl_big_endian_data = NL_FALSE;
        break;

    case NL_LITTLE_ENDIAN_DATA:
        nlState.nl_big_endian_data = NL_TRUE;
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
        return NL_FALSE;
    }
    return NL_TRUE;
}

HL_EXP NLboolean HL_APIENTRY nlGetBoolean(NLenum name)
{
    switch (name) {

    case NL_BLOCKING_IO:
        return nlBlocking;

    case NL_SOCKET_STATS:
        return nlState.socketStats;

    case NL_BIG_ENDIAN_DATA:
        return nlState.nl_big_endian_data;

    case NL_LITTLE_ENDIAN_DATA:
        return (NLboolean)(nlState.nl_big_endian_data == NL_TRUE ? NL_FALSE:NL_TRUE);

    default:
        nlSetError(NL_INVALID_ENUM);
        return NL_FALSE;
    }
}

HL_EXP NLlong HL_APIENTRY nlGetSocketStat(NLsocket socket, NLenum name)
{
    nl_socket_t     *sock;
    NLlong          result = 0;

    if(nlIsValidSocket(socket) == NL_FALSE)
    {
        nlSetError(NL_INVALID_SOCKET);
        return 0;
    }
    if(nlLockSocket(socket, NL_BOTH) == NL_FALSE)
    {
        return 0;
    }
    sock = nlSockets[socket];

    switch (name) {

    case NL_PACKETS_SENT:
        result = sock->outstats.packets;
        break;

    case NL_BYTES_SENT:
        result = sock->outstats.bytes;
        break;

    case NL_AVE_BYTES_SENT:
        nlUpdateSocketOutStats(socket, 0, 0);
        result = sock->outstats.average;
        if(result == 0)
        {
            /* this corrects the stats for the first second */
            result = sock->outstats.curbytes;
        }
        break;

    case NL_HIGH_BYTES_SENT:
        result = sock->outstats.highest;
        break;

    case NL_PACKETS_RECEIVED:
        result = sock->instats.packets;
        break;

    case NL_BYTES_RECEIVED:
        result = sock->instats.bytes;
        break;

    case NL_AVE_BYTES_RECEIVED:
        nlUpdateSocketInStats(socket, 0, 0);
        result = sock->instats.average;
        if(result == 0)
        {
            /* this corrects the stats for the first second */
            result = sock->instats.curbytes;
        }
        break;

    case NL_HIGH_BYTES_RECEIVED:
        result = sock->instats.highest;
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
    }
    nlUnlockSocket(socket, NL_BOTH);
    return result;
}

HL_EXP NLboolean HL_APIENTRY nlClearSocketStat(NLsocket socket, NLenum name)
{
    nl_socket_t     *sock;

    if(nlIsValidSocket(socket) == NL_FALSE)
    {
        nlSetError(NL_INVALID_SOCKET);
        return NL_FALSE;
    }
    if(nlLockSocket(socket, NL_BOTH) == NL_FALSE)
    {
        return NL_FALSE;
    }
    sock = nlSockets[socket];

    switch (name) {

    case NL_PACKETS_SENT:
        sock->outstats.packets = 0;
        break;

    case NL_BYTES_SENT:
        sock->outstats.bytes = 0;
        break;

    case NL_AVE_BYTES_SENT:
        sock->outstats.average = 0;
        memset((NLbyte *)sock->outstats.bucket, 0, sizeof(NLlong) * NL_NUM_BUCKETS);
        break;

    case NL_HIGH_BYTES_SENT:
        sock->outstats.highest = 0;
        break;

    case NL_PACKETS_RECEIVED:
        sock->instats.packets = 0;
        break;

    case NL_BYTES_RECEIVED:
        sock->instats.bytes = 0;
        break;

    case NL_AVE_BYTES_RECEIVED:
        sock->instats.average = 0;
        memset((NLbyte *)sock->instats.bucket, 0, sizeof(NLlong) * NL_NUM_BUCKETS);
        break;

    case NL_HIGH_BYTES_RECEIVED:
        sock->instats.highest = 0;
        break;

    case NL_ALL_STATS:
        sock->outstats.packets = 0;
        sock->outstats.bytes = 0;
        sock->outstats.average = 0;
        memset((NLbyte *)sock->outstats.bucket, 0, sizeof(NLlong) * NL_NUM_BUCKETS);
        sock->outstats.highest = 0;
        sock->instats.packets = 0;
        sock->instats.bytes = 0;
        sock->instats.average = 0;
        memset((NLbyte *)sock->instats.bucket, 0, sizeof(NLlong) * NL_NUM_BUCKETS);
        sock->instats.highest = 0;
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
        nlUnlockSocket(socket, NL_BOTH);
        return NL_FALSE;
    }
    nlUnlockSocket(socket, NL_BOTH);
    return NL_TRUE;
}

HL_EXP NLushort  HL_APIENTRY nlSwaps(NLushort x)
{
    if(NL_SWAP_TRUE)
    {
        return (NLushort)(((((NLushort)x) & 0x00ff) << 8) | ((((NLushort)x) & 0xff00) >> 8));
    }
    else
    {
        return x;
    }
}

HL_EXP NLulong   HL_APIENTRY nlSwapl(NLulong x)
{
    if(NL_SWAP_TRUE)
    {
        return (NLulong)(((((NLulong)x) & 0x000000ff) << 24) | ((((NLulong)x) & 0x0000ff00) << 8) | ((((NLulong)x) & 0x00ff0000) >> 8) | ((((NLulong)x) & 0xff000000) >> 24));
    }
    else
    {
        return x;
    }
}

HL_EXP NLfloat   HL_APIENTRY nlSwapf(NLfloat f)
{
    if(NL_SWAP_TRUE)
    {
        NLulong temp = (NLulong)nlSwapl(*(NLulong *)&f);

        return *((float *)&temp);
    }
    else
    {
        return f;
    }
}

HL_EXP NLdouble  HL_APIENTRY nlSwapd(NLdouble d)
{
    if(NL_SWAP_TRUE)
    {
        union {NLulong l[2]; NLdouble d;} in, out;

        in.d = d;
        out.l[0] = nlSwapl(in.l[1]);
        out.l[1] = nlSwapl(in.l[0]);
        return out.d;
    }
    else
    {
        return d;
    }
}

#if defined (__LCC__)
BOOL WINAPI __declspec(dllexport) LibMain(/*@unused@*/HINSTANCE hinstDLL, /*@unused@*/DWORD fdwReason, /*@unused@*/LPVOID lpvReserved)
{
    return TRUE;
}
#endif /* WINDOWS APP */
