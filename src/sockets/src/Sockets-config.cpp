/**
 **	\file Sockets-config.cpp
 **	\date  2006-09-04
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2007  Anders Hedstrom

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <stdio.h>
#include <string.h>
#include "sockets-config.h"
#include "socket_include.h"

int main(int argc,char *argv[])
{
	if (argc > 1 && !strcmp(argv[1], "-info"))
	{
#ifdef HAVE_OPENSSL
		printf("SSL support\n");
#endif
#ifdef IPPROTO_IPV6
		printf("IPv6 support\n");
#endif
#ifdef USE_SCTP
#ifdef IPPROTO_SCTP
		printf("SCTP support\n");
#	ifdef HAVE_SCTP
		printf("  HAVE_SCTP: yes\n");
#	else
		printf("  HAVE_SCTP: no\n");
#	endif
#	ifdef HAVE_KERNEL_SCTP
		printf("  HAVE_KERNEL_SCTP: yes\n");
#	else
		printf("  HAVE_KERNEL_SCTP: no\n");
#	endif
#	ifdef HAVE_SCTP_PRSCTP
		printf("  HAVE_SCTP_PRSCTP: yes\n");
#	else
		printf("  HAVE_SCTP_PRSCTP: no\n");
#	endif
#	ifdef HAVE_SCTP_ADDIP
		printf("  HAVE_SCTP_ADDIP: yes\n");
#	else
		printf("  HAVE_SCTP_ADDIP: no\n");
#	endif
#	ifdef HAVE_SCTP_CANSET_PRIMARY
		printf("  HAVE_SCTP_CANSETPRIMARY: yes\n");
#	else
		printf("  HAVE_SCTP_CANSETPRIMARY: no\n");
#	endif
#	ifdef HAVE_SCTP_SAT_NETWORK_CAPABILITY
		printf("  HAVE_SCTP_SAT_NETWORK_CAPABILITY: yes\n");
#	else
		printf("  HAVE_SCTP_SAT_NETWORK_CAPABILITY: no\n");
#	endif
#	ifdef HAVE_SCTP_MULTIBUF
		printf("  HAVE_SCTP_MULTIBUF: yes\n");
#	else
		printf("  HAVE_SCTP_MULTIBUF: no\n");
#	endif
#	ifdef HAVE_SCTP_NOCONNECT
		printf("  HAVE_SCTP_NOCONNECT: yes\n");
#	else
		printf("  HAVE_SCTP_NOCONNECT: no\n");
#	endif
#	ifdef HAVE_SCTP_EXT_RCVINFO
		printf("  HAVE_SCTP_EXT_RCVINFO: yes\n");
#	else
		printf("  HAVE_SCTP_EXT_RCVINFO: no\n");
#	endif
#else
		printf("No SCTP support\n");
#endif
#endif
		return 0;
	}
	printf(" -D_VERSION='\"%s\"'", _VERSION);

#ifdef LINUX
	printf(" -DLINUX");
#endif
#ifdef MACOSX
	printf(" -DMACOSX");
#endif
#ifdef SOLARIS
	printf(" -DSOLARIS");
#endif
#ifdef SOLARIS8
	printf(" -DSOLARIS8");
#endif
#ifdef _WIN32
	printf(" -D_WIN32");
#endif
#ifdef __CYGWIN__
	printf(" -D__CYGWIN__");
#endif
#ifdef SOCKETS_NAMESPACE
	printf(" -DSOCKETS_NAMESPACE=%s", SOCKETS_NAMESPACE_STR);
#endif
#ifdef _DEBUG
	printf(" -D_DEBUG");
#endif

}


