/*
 * Copyright (c) 2002 - 2003
 * NetGroup, Politecnico di Torino (Italy)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the 
 * documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Politecnico di Torino nor the names of its 
 * contributors may be used to endorse or promote products derived from 
 * this software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */


#ifndef __SOCKUTILS_H__
#define __SOCKUTILS_H__



// Definition for Microsoft Visual Studio
#if _MSC_VER > 1000
#pragma once
#endif

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>	/* DNS lookup */
#include <unistd.h>	/* close() */
#endif



// Some minor differences between UNIX and Win32
#ifdef WIN32
/*! \ingroup remote_struct
	\brief In Win32, sockets use unsigned integers; in UNIX, they use signed integer.
	
	So, we define a generic SOCKET in order to be cross-platform compatible.
*/
	#define SOCKET unsigned int	
#else
/*! \ingroup remote_struct
	\brief In Win32, sockets use unsigned integers; in UNIX, they use signed integer.
	
	So, we define a generic SOCKET in order to be cross-platform compatible.
*/

	#define SOCKET int

/*! \ingroup remote_struct
	\brief In Win32, the close() call cannot be used for socket.
	
	So, we define a generic closesocket() call in order to be cross-platform compatible.
*/
	#define closesocket(a) close(a) 
#endif





// All this stuff is private
/*! \addtogroup remote_pri_struct
	\{
*/


/*!
	\brief DEBUG facility: it prints an error message on the screen (stderr)
	
	This macro prints the error on the standard error stream (stderr);
	if we are working in debug mode (i.e. there is no NDEBUG defined) and we are in
	Microsoft Visual C++, the error message will appear on the MSVC console as well.

	When NDEBUG is defined, this macro is empty.

	\param msg: the message you want to print.

	\param expr: 'false' if you want to abort the program, 'true' it you want
	to print the message and continue.

	\return No return values.
*/
#ifdef NDEBUG
#define SOCK_ASSERT(msg, expr) ((void)0)
#else
	#include <assert.h>
	#if (defined(WIN32) && defined(_MSC_VER))
		#include <crtdbg.h>				// for _CrtDbgReport
		// Use MessageBox(NULL, msg, "warning", MB_OK)' instead of the other calls if you want to debug a Win32 service
		// Remember to activate the 'allow service to interact with desktop' flag of the service
		#define SOCK_ASSERT(msg, expr) { _CrtDbgReport(_CRT_WARN, NULL, 0, NULL, "%s\n", msg); fprintf(stderr, "%s\n", msg); assert(expr); }
	#else
		#define SOCK_ASSERT(msg, expr) { fprintf(stderr, "%s\n", msg); assert(expr); }
	#endif
#endif




/****************************************************
 *                                                  *
 * Exported functions / definitions                 *
 *                                                  *
 ****************************************************/

#define SOCKBUF_CHECKONLY 1		/*!< 'checkonly' flag, into the rpsock_bufferize() */
#define SOCKBUF_BUFFERIZE 0		/*!< no 'checkonly' flag, into the rpsock_bufferize() */

#define SOCKOPEN_CLIENT 0		/*!< no 'server' flag; it opens a client socket */
#define SOCKOPEN_SERVER 1		/*!< 'server' flag; it opens a server socket */

#define SOCK_ERRBUF_SIZE 256	/*!< Size of the buffer that has to keep error messages */

/*!
	\}
*/ // end of private documentation


#ifdef __cplusplus
extern "C" {
#endif


void sock_geterror(const char *caller, char *string, int size);
int sock_init(char *errbuf);
void sock_cleanup();
int sock_validaddr(const char *address, const char *port,
						struct addrinfo *hints, struct addrinfo **addrinfo, char *errbuf);
int sock_recv(SOCKET socket, char *buffer, int size, char *errbuf);
int sock_recv_dgram(SOCKET sock, char *buffer, int size, char *errbuf);
int sock_open(struct addrinfo *addrinfo, int server, int nconn, char *errbuf);
int sock_close(SOCKET sock, char *errbuf);

int sock_send(SOCKET socket, const char *buffer, int size, char *errbuf);
int sock_bufferize(const char *buffer, int size, char *tempbuf, int *offset, int totsize, int checkonly, char *errbuf);
int sock_discard(SOCKET socket, int size, char *errbuf);
int	sock_check_hostlist(char *hostlist, const char *sep, struct sockaddr_storage *from, char *errbuf);
int sock_cmpaddr(struct sockaddr_storage *first, struct sockaddr_storage *second);


#ifdef __cplusplus
}
#endif


#endif

