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

#if !defined(PS3) && !defined(WII)
#include <memory.h>
#endif
#include <stdio.h>
#include <string.h>
#include <errno.h>


#ifndef FD_SETSIZE
#define FD_SETSIZE              8192
#endif


#if defined WIN32 || defined WIN64

#include "hawknl/wsock.h"

#ifdef _MSC_VER
#pragma warning (disable:4100) /* disable "unreferenced formal parameter" */
#endif

#else
/* Unix-style systems */
#include <sys/types.h>
#ifndef WII
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifdef PS3
#include <net/netdb.h>
#else
#include <netdb.h>
#endif
#endif
// #include <sys/ioctl.h>
#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKET int
/* define INADDR_NONE if not already */
#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long) -1)
#define sockerrno errno
#endif

#endif

#include "hawknl/nlinternal.h"
#include "hawknl/sock.h"


#ifdef NL_INCLUDE_IPX
#include "hawknl/ipx.h"

typedef struct
{
    NLaddress   /*@temp@*/*address;
    NLchar      /*@temp@*/*name;
    NLsocket    socket;
} NLaddress_ex_t;

extern SOCKET nlGroupGetFdset(NLint group, /*@out@*/ fd_set *fd);
extern NLint sock_Error(void);
extern NLboolean sock_SetBlocking(SOCKET socket, NLboolean arg);
extern NLboolean sock_SetBroadcast(SOCKET socket);
extern NLboolean sock_SetReuseAddr(nl_socket_t *sock);
extern void sock_Close(NLsocket socket);
extern NLsocket sock_OpenINT(NLushort port, NLenum type, NLenum driver);

#define MAXHOSTNAMELEN      256
#define NL_CONNECT_STRING   "HawkNL request connection."
#define NL_REPLY_STRING     "HawkNL connection OK."

static NLaddress ipx_ouraddress;
static NLaddress ipx_ouraddress_copy;
static NLint ipxport = 0;
extern int backlog;
extern NLboolean reuseaddress;


/* is there a better way to get our address? */
static NLboolean ipx_GetHostAddress(NLaddress *address)
{
    NLint               addrlen = (NLint)sizeof(NLaddress);
    SOCKET              sock;

    sock = socket(PF_IPX, SOCK_DGRAM, NSPROTO_IPX);

    if(sock == INVALID_SOCKET)
    {
        nlSetError(NL_NO_NETWORK);
        return NL_FALSE;
    }

    ((struct sockaddr_ipx *)address)->sa_family = AF_IPX;
    memset(((struct sockaddr_ipx *)address)->sa_netnum, 0, 4);
    memset(((struct sockaddr_ipx *)address)->sa_nodenum, 0, 6);
    ((struct sockaddr_ipx *)address)->sa_socket = 0;

    if(bind(sock, (struct sockaddr *)address, (int)sizeof(struct sockaddr_ipx)) == SOCKET_ERROR)
    {
        nlSetError(NL_NO_NETWORK);
        (void)closesocket(sock);
        return NL_FALSE;
    }

    if(getsockname(sock, (struct sockaddr *)address, &addrlen) != 0)
    {
        nlSetError(NL_NO_NETWORK);
        (void)closesocket(sock);
        return NL_FALSE;
    }

    (void)closesocket(sock);
    return NL_TRUE;
}

NLushort ipx_GetPort(SOCKET socket)
{
    struct sockaddr_ipx   addr;
    int                  len;

    len = (int)sizeof(struct sockaddr_ipx);
    if(getsockname(socket, (struct sockaddr *)&addr, &len) != 0)
    {
        return 0;
    }

    return ntohs(addr.sa_socket);
}


NLboolean ipx_Init(void)
{
#ifdef HL_WINDOWS_APP
    WSADATA libmibWSAdata;

    if(WSAStartup(0x101,&libmibWSAdata) != 0)
    {
        return NL_FALSE;
    }
#endif
    if(ipx_GetHostAddress(&ipx_ouraddress) == NL_FALSE)
    {
        ipx_Shutdown();
        return NL_FALSE;
    }

    return NL_TRUE;
}

void ipx_Shutdown(void)
{
#ifdef HL_WINDOWS_APP
    (void)WSACleanup();
#endif
}

static SOCKET ipx_AcceptIPX(NLsocket nlsocket, struct sockaddr_ipx /*@out@*/ *newaddr)
{
    nl_socket_t         *sock = nlSockets[nlsocket];
    struct sockaddr_ipx  ouraddr;
    SOCKET              newsocket;
    NLushort            localport;
    NLbyte              buffer[NL_MAX_STRING_LENGTH];
    socklen_t           len = (socklen_t)sizeof(struct sockaddr_ipx);
    NLint               slen = (NLint)sizeof(NL_CONNECT_STRING);
    NLbyte              reply = (NLbyte)0x00;
    NLint               count = 0;
    
    /* Get the packet and remote host address */
    if(recvfrom((SOCKET)sock->realsocket, buffer, (int)sizeof(buffer), 0,
        (struct sockaddr *)newaddr, &len) < (int)sizeof(NL_CONNECT_STRING))
    {
        nlSetError(NL_NO_PENDING);
        return INVALID_SOCKET;
    }
    /* Let's check for the connection string */
    buffer[slen - 1] = (NLbyte)0; /* null terminate for peace of mind */
    if(strcmp(buffer, NL_CONNECT_STRING) != 0)
    {
        nlSetError(NL_NO_PENDING);
        return INVALID_SOCKET;
    }
    /* open up a new socket on this end */
    newsocket = socket(PF_IPX, SOCK_DGRAM, NSPROTO_IPX);
    if(newsocket == INVALID_SOCKET)
    {
        nlSetError(NL_SYSTEM_ERROR);
        (void)closesocket(newsocket);
        return INVALID_SOCKET;
    }
    
    ouraddr.sa_family = AF_IPX;
    memset(&ouraddr.sa_netnum, 0, 4);
    memset(&ouraddr.sa_nodenum, 0, 6);
    ouraddr.sa_socket = 0;
    
    if(bind(newsocket, (struct sockaddr *)&ouraddr, len) == SOCKET_ERROR)
    {
        nlSetError(NL_SYSTEM_ERROR);
        (void)closesocket(newsocket);
        return INVALID_SOCKET;
    }
    /* get the new port */
    localport = ipx_GetPort(newsocket);
    
    /* create the return message */
    writeShort(buffer, count, localport);
    writeString(buffer, count, (NLchar *)TEXT(NL_REPLY_STRING));
    
    /* send back the reply with our new port */
    if(sendto((SOCKET)sock->realsocket, buffer, count, 0, (struct sockaddr *)newaddr,
        (int)sizeof(struct sockaddr_ipx)) < count)
    {
        nlSetError(NL_SYSTEM_ERROR);
        (void)closesocket(newsocket);
        return INVALID_SOCKET;
    }
    /* send back a 0 length packet from our new port, needed for firewalls */
    if(sendto(newsocket, &reply, 0, 0,
        (struct sockaddr *)newaddr,
        (int)sizeof(struct sockaddr_ipx)) < 0)
    {
        nlSetError(NL_SYSTEM_ERROR);
        (void)closesocket(newsocket);
        return INVALID_SOCKET;
    }
    /* connect the socket */
    if(connect(newsocket, (struct sockaddr *)newaddr,
        (int)sizeof(struct sockaddr_ipx)) == SOCKET_ERROR)
    {
        nlSetError(NL_SYSTEM_ERROR);
        (void)closesocket(newsocket);
        return INVALID_SOCKET;
    }
    
    return newsocket;
}

NLsocket ipx_AcceptConnection(NLsocket socket)
{
    nl_socket_t         *sock = nlSockets[socket];
    nl_socket_t         *newsock;
    NLsocket            newsocket;
    SOCKET              realsocket;
    struct sockaddr_ipx newaddr;

    if(sock->listen == NL_FALSE)
    {
        nlSetError(NL_NOT_LISTEN);
        return NL_INVALID;
    }

    if(sock->type == NL_RELIABLE || sock->type == NL_RELIABLE_PACKETS)
    {
        NLint   len = (NLint)sizeof(newaddr);

        realsocket = accept((SOCKET)sock->realsocket,
                                    (struct sockaddr *)&newaddr, &len);

        if(realsocket == INVALID_SOCKET)
        {
            if(sockerrno == (int)EWOULDBLOCK || errno == EAGAIN)/* yes, we need to use errno here */
            {
                nlSetError(NL_NO_PENDING);
            }
            else
            {
                nlSetError(NL_SYSTEM_ERROR);
            }
            return NL_INVALID;
        }
    }
    else if(sock->type == NL_UNRELIABLE)
    {
        realsocket = ipx_AcceptIPX(socket, &newaddr);

        if(realsocket == INVALID_SOCKET)
        {
            /* error is already set in sock_AcceptUDP */
            return NL_INVALID;
        }
    }
    else
    {
        nlSetError(NL_WRONG_TYPE);
        return NL_INVALID;
    }

    newsocket = nlGetNewSocket();
    if(newsocket == NL_INVALID)
    {
        return NL_INVALID;
    }
    if(nlLockSocket(newsocket, NL_BOTH) == NL_FALSE)
    {
        return NL_INVALID;
    }
    newsock = nlSockets[newsocket];

    /* update the remote address */
    memcpy((char *)&newsock->addressin, (char *)&newaddr, sizeof(struct sockaddr_ipx));
    newsock->realsocket = (NLint)realsocket;
    newsock->localport = ipx_GetPort(realsocket);
    newsock->remoteport = ipx_GetPortFromAddr((NLaddress *)&newsock->addressin);

    if(newsock->blocking == NL_FALSE)
    {
        if(sock_SetBlocking((SOCKET)newsock->realsocket, NL_FALSE) == NL_FALSE)
        {
            nlSetError(NL_SYSTEM_ERROR);
            sock_Close(newsocket);
            return NL_INVALID;
        }
    }

    newsock->reliable = sock->reliable;

    return newsocket;
}

NLsocket ipx_Open(NLushort port, NLenum type)
{
    return sock_OpenINT(port, type, NL_IPX);
}

NLchar *ipx_AddrToString(const NLaddress *address, NLchar *string)
{
    _stprintf(string, TEXT("%02x%02x%02x%02x:%02x%02x%02x%02x%02x%02x:%u"),
        (unsigned int)((struct sockaddr_ipx *)address)->sa_netnum[0] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_netnum[1] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_netnum[2] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_netnum[3] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_nodenum[0] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_nodenum[1] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_nodenum[2] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_nodenum[3] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_nodenum[4] & 0xff,
        (unsigned int)((struct sockaddr_ipx *)address)->sa_nodenum[5] & 0xff,
        ntohs(((struct sockaddr_ipx *)address)->sa_socket));
    return string;
}

NLboolean ipx_StringToAddr(const NLchar *string, NLaddress *address)
{
    int  val = 0;
    NLchar buffer[3];

    buffer[2] = (NLchar)0;
    memset(address, 0, sizeof(NLaddress));
    ((struct sockaddr_ipx *)address)->sa_family = AF_IPX;
    address->valid = NL_FALSE;

#define DO(src,dest)    \
    buffer[0] = string[(sizeof(NLchar) * src)];    \
    buffer[1] = string[src + 1];    \
    if(_stscanf (buffer, (const NLchar *)TEXT("%x"), &val) != 1)   \
        return NL_FALSE; \
    ((struct sockaddr_ipx *)address)->dest = (char)val

    DO(0, sa_netnum[0]);
    DO(2, sa_netnum[1]);
    DO(4, sa_netnum[2]);
    DO(6, sa_netnum[3]);
    DO(9, sa_nodenum[0]);
    DO(11, sa_nodenum[1]);
    DO(13, sa_nodenum[2]);
    DO(15, sa_nodenum[3]);
    DO(17, sa_nodenum[4]);
    DO(19, sa_nodenum[5]);
#undef DO

    (void)_stscanf(&string[(sizeof(NLchar) * 22)], (const NLchar *)TEXT("%d"), &val);
    ((struct sockaddr_ipx *)address)->sa_socket = htons((unsigned short)val);
    address->valid = NL_TRUE;
    return NL_TRUE;
}

NLboolean ipx_GetLocalAddr(NLsocket socket, NLaddress *address)
{
    nl_socket_t *sock = nlSockets[socket];

    memcpy(address, &ipx_ouraddress, sizeof(NLaddress));
	ipx_SetAddrPort(address, sock->localport);
    address->valid = NL_TRUE;
    return NL_TRUE;
}

NLaddress *ipx_GetAllLocalAddr(NLint *count)
{
    *count = 1;
    memcpy(&ipx_ouraddress_copy, &ipx_ouraddress, sizeof(NLaddress));
    ipx_ouraddress_copy.valid = NL_TRUE;
    return &ipx_ouraddress_copy;
}

NLboolean ipx_SetLocalAddr(const NLaddress *address)
{
    memcpy(&ipx_ouraddress, address, sizeof(NLaddress));
    return NL_TRUE;
}

NLchar *ipx_GetNameFromAddr(const NLaddress *address, NLchar *name)
{
    return ipx_AddrToString(address, name);
}

NLboolean ipx_GetNameFromAddrAsync(const NLaddress *address, NLchar *name)
{
    (void)ipx_AddrToString(address, name);
    return NL_TRUE;
}

NLboolean ipx_GetAddrFromName(const NLchar *name, NLaddress *address)
{
    NLint n;
    NLchar buffer[(sizeof(NLchar) * 32)];

    address->valid = NL_TRUE;
    n = (NLint)_tcslen(name);

    if(n == (NLint)(sizeof(NLchar) * 12))
    {
        _stprintf(buffer, TEXT("00000000:%s:%d"), name, ipxport);
        return ipx_StringToAddr (buffer, address);
    }
    if(n == (NLint)(sizeof(NLchar) * 21))
    {
        _stprintf(buffer, TEXT("%s:%d"), name, ipxport);
        return ipx_StringToAddr (buffer, address);
    }
    if((n > (NLint)(sizeof(NLchar) * 21)) && (n <= (NLint)(sizeof(NLchar) * 27)))
    {
        return ipx_StringToAddr (name, address);
    }
    memset(address, 0, sizeof(NLaddress));
    address->valid = NL_FALSE;
    return NL_FALSE;
}

NLboolean ipx_GetAddrFromNameAsync(const NLchar *name, NLaddress *address)
{
    return ipx_GetAddrFromName(name, address);
}

NLboolean ipx_AddrCompare(const NLaddress *address1, const NLaddress *address2)
{
    if(((struct sockaddr_ipx *)address1)->sa_family != ((struct sockaddr_ipx *)address2)->sa_family)
        return NL_FALSE;

    if(memcmp(((struct sockaddr_ipx *)address1)->sa_netnum, ((struct sockaddr_ipx *)address2)->sa_netnum, 4) != 0)
        return NL_FALSE;
    if(memcmp(((struct sockaddr_ipx *)address1)->sa_nodenum, ((struct sockaddr_ipx *)address2)->sa_nodenum, 6) != 0)
        return NL_FALSE;

    if(((struct sockaddr_ipx *)address1)->sa_socket != ((struct sockaddr_ipx *)address2)->sa_socket)
        return NL_FALSE;

    return NL_TRUE;
}

NLushort ipx_GetPortFromAddr(const NLaddress *address)
{
    return ntohs(((struct sockaddr_ipx *)address)->sa_socket);
}

void ipx_SetAddrPort(NLaddress *address, NLushort port)
{
    ((struct sockaddr_ipx *)address)->sa_socket = htons((NLushort)port);
}

NLboolean ipx_Hint(NLenum name, NLint arg)
{
    switch(name) {

    case NL_LISTEN_BACKLOG:
        backlog = arg;
        break;

    case NL_REUSE_ADDRESS:
        reuseaddress = (NLboolean)(arg != 0 ? NL_TRUE : NL_FALSE);
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
        return NL_FALSE;
    }
    return NL_TRUE;
}

NLboolean ipx_SetSocketOpt(NLsocket socket, NLenum name, NLint arg)
{
    nl_socket_t *sock = nlSockets[socket];

    switch (name) {

    case NL_BLOCKING_IO:
        sock->blocking = (NLboolean)(arg != 0 ? NL_TRUE : NL_FALSE);
        if(sock_SetBlocking((SOCKET)sock->realsocket, sock->blocking) == NL_FALSE)
        {
            nlSetError(NL_SYSTEM_ERROR);
            return NL_FALSE;
        }
        break;
    case NL_REUSE_ADDRESS:
        if(sock_SetReuseAddr(sock) == NL_FALSE)
        {
            nlSetError(NL_SYSTEM_ERROR);
            return NL_FALSE;
        }
        break;

    default:
        nlSetError(NL_INVALID_ENUM);
        return NL_FALSE;
    }
    return NL_TRUE;
}

NLint ipx_GetSocketOpt(NLsocket socket, NLenum name)
{
    nl_socket_t *sock = nlSockets[socket];

    switch (name) {

    case NL_BLOCKING_IO:
        return (NLint)sock->blocking;

    case NL_REUSE_ADDRESS:
        return (NLint)sock->reuseaddr;

    default:
        nlSetError(NL_INVALID_ENUM);
        return NL_INVALID;
    }
}

#else  /* NL_INCLUDE_IPX */
/* include dummy functions so that sock.c will compile */
NLushort ipx_GetPort(SOCKET socket)
{
    return 0;
}

NLushort ipx_GetPortFromAddr(const NLaddress *address)
{
    return 0;
}

#endif /* NL_INCLUDE_IPX */
