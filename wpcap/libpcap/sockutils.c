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



#include "sockutils.h"
#include <string.h>	// for strerror
#include <errno.h>	// for the errno variable
#include <stdio.h>	// for the stderr file

/*!
	\file sockutils.c

	The goal of this file it to provide a common set of primitives for socket manipulation.
	Although the socket interface defined in the RFC 2553 (and its updates) is excellent, several
	minor issues are still hidden when we want to operate on several operating systems.

	These calls do not want to provide a better socket interface; vice versa, they want to
	provide a set of calls that is portable among several operating systems, hiding their
	differences.
*/



// WinSock Initialization
#ifdef WIN32
	#define WINSOCK_MAJOR_VERSION 2		/*!< Ask for Winsock 2.2 */
	#define WINSOCK_MINOR_VERSION 2		/*!< Ask for Winsock 2.2 */
	int sockcount= 0;					/*!< Variable that allows calling the WSAStartup() only one time */
#endif

// Some minor differences between UNIX and Win32
#ifdef WIN32
	#define SHUT_WR SD_SEND			/*!< The control code for shutdown() is different in Win32 */
	#define snprintf _snprintf		/*!< The snprintf is called _snprintf() in Win32 */
#endif



/****************************************************
 *                                                  *
 * Locally defined functions                        *
 *                                                  *
 ****************************************************/

int sock_ismcastaddr(const struct sockaddr *saddr);



/*
	\brief Global variable; needed to keep the message due to an error that we want to discard.
	
	This can happen, for instance, because we already have an error message and we want to keep 
	the first one.
*/
char fakeerrbuf[SOCK_ERRBUF_SIZE + 1];





/****************************************************
 *                                                  *
 * Function bodies                                 *
 *                                                  *
 ****************************************************/


/*!	\ingroup remote_pri_func
	\brief It retrieves the error message after an error occurred in the socket interface.

	This function is defined because of the different way errors are returned in UNIX
	and Win32. This function provides a consistent way to retrieve the error message
	(after a socket error occurred) on all the platforms.

	\param caller: a pointer to a user-allocated string which contains a message that has
	to be printed *before* the true error message. It could be, for example, 'this error
	comes from the recv() call at line 31'.
	
	\param string: a pointer to an user-allocated buffer that will contain the complete
	error message. This buffer has to be at least 'size' in length.

	\param size: the size of the buffer in which the error message will be copied.

	\return No return values. The error message is returned in the 'string' parameter.
*/
void sock_geterror(const char *caller, char *string, int size)
{
#ifdef WIN32
	int retval;
	int code;
	char message[SOCK_ERRBUF_SIZE];
	
		code= GetLastError();
	
		retval= FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
	                  FORMAT_MESSAGE_MAX_WIDTH_MASK,
	                  NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	                  (LPSTR) message, SOCK_ERRBUF_SIZE, NULL);
	
		if (retval == 0)
		{
			snprintf(string, size, "%sUnable to get the exact error message", caller);
			return;
		}
	
		snprintf(string, size, "%s%s (code %d)", caller, message, code);

#else
	char *message;
	
		message= strerror(errno);
		snprintf(string, size, "%s%s (code %d)", caller, message, errno);
#endif
}



/*!	\ingroup remote_pri_func
	\brief It initializes sockets.

	This function is pretty useless on UNIX, since socket initialization is not required.
	However it is required on Win32. In UNIX, this function appears to be completely empty.

	\param errbuf: a pointer to a user-allocated buffer (of size SOCK_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return '0' if everything is fine, '-1' if some errors occurred. The error message is returned
	in the 'errbuf' variable.
*/
int sock_init(char *errbuf)
{
#ifdef WIN32
	if (sockcount == 0)
	{
	WSADATA wsaData;			// helper variable needed to initialize Winsock

		// Ask for Winsock version 2.2.
		if ( WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			snprintf(errbuf, SOCK_ERRBUF_SIZE, "Failed to initialize Winsock\n");
			WSACleanup();
			return -1;
		}
	}

	sockcount++;
#endif

	return 0;
}



/*!	\ingroup remote_pri_func
	\brief It deallocates sockets.

	This function is pretty useless on UNIX, since socket deallocation is not required.
	However it is required on Win32. In UNIX, this function appears to be completely empty.

	\return No error values.
*/
void sock_cleanup()
{
#ifdef WIN32
	sockcount--;
	if (sockcount == 0)
		WSACleanup();
#endif
}



/*!	\ingroup remote_pri_func
	\brief It checks if the sockaddr variable contains a multicast address.

	\return '0' if the address is multicast, '-1' if it is not. 
*/
int sock_ismcastaddr(const struct sockaddr *saddr)
{
	if (saddr->sa_family == PF_INET)
	{
		struct sockaddr_in *saddr4 = (struct sockaddr_in *) saddr;
		if (IN_MULTICAST(ntohl(saddr4->sin_addr.s_addr))) return 0;
		else return -1;
	}
	else
	{
		struct sockaddr_in6 *saddr6 = (struct sockaddr_in6 *) saddr;
		if (IN6_IS_ADDR_MULTICAST(&saddr6->sin6_addr)) return 0;
		else return -1;
	}
}



/*!	\ingroup remote_pri_func
	\brief It initializes a network connection both from the client and the server side.

	In case of a client socket, this function calls socket() and connect().
	In the meanwhile, it checks for any socket error.
	If an error occurs, it writes the error message into 'errbuf'.

	In case of a server socket, the function calls socket(), bind() and listen().

	In no cases this fucntion will authenticate the user on the remote host. This function
	has to be done in the rpcap_sendauth().

	\param addrinfo: pointer to an addrinfo variable which will be used to
	open the socket and such. This variable is the one returned by the previous call to
	sockvalidateaddr().

	\param server: '1' if this is a server socket, '0' otherwise.

	\param nconn: number of the connections that are allowed to wait into the listen() call.
	This value has no meanings in case of a client socket.

	\param errbuf: a pointer to a user-allocated buffer (of size SOCK_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return the socket that has been opened (that has to be used in the following sockets calls)
	if everything is fine, '-1' if some errors occurred. The error message is returned
	in the 'errbuf' variable.
*/
int sock_open(struct addrinfo *addrinfo, int server, int nconn, char *errbuf)
{
SOCKET sock;

	sock = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);
	if (sock == -1)
	{
		sock_geterror("socket(): ", errbuf, SOCK_ERRBUF_SIZE);
		return -1;
	}


	// This is a server socket
	if (server)
	{
#ifdef BSD
		// Force the use of IPv6-only addresses; in BSD you can accept both v4 and v6
		// connections if you have a "NULL" pointer as the nodename in the getaddrinfo()
		// This behaviour is not clear in the RFC 2553, so each system implements the
		// bind() differently from this point of view

		if (addrinfo->ai_family == PF_INET6)
		{
		int on;

			if (setsockopt(sock, IPPROTO_IPV6, IPV6_BINDV6ONLY, (char *)&on, sizeof (int)) == -1)
			{
				snprintf(errbuf, SOCK_ERRBUF_SIZE, "setsockopt(IPV6_BINDV6ONLY)");
				return -1;
			}
		} 
#endif

		// WARNING: if the address is a mcast one, I should place the proper Win32 code here
		if (bind(sock, addrinfo->ai_addr, addrinfo->ai_addrlen) != 0)
		{
			sock_geterror("bind(): ", errbuf, SOCK_ERRBUF_SIZE);
			return -1;
		}

		if (addrinfo->ai_socktype == SOCK_STREAM)
			if (listen(sock, nconn) == -1)
			{
				sock_geterror("listen(): ", errbuf, SOCK_ERRBUF_SIZE);
				return -1;
			}

		// server side ended
		return sock;
	}
	else	// we're the client
	{
		if (connect(sock, addrinfo->ai_addr, addrinfo->ai_addrlen) == -1)
		{
			sock_geterror("Is libpcap/WinPcap properly installed on the other host? connect() failed: ", errbuf, SOCK_ERRBUF_SIZE);
			closesocket(sock);
			return -1;
		}

		return sock;
	}
}




/*!	\ingroup remote_pri_func
	\brief Closes the present (TCP and UDP) socket connection.

	This function sends a shutdown() on the socket in order to disable send() calls
	(while recv() ones are still allowed). Then, it closes the socket.

	\param sock: the socket identifier of the connection that has to be closed.

	\param errbuf: a pointer to a user-allocated buffer (of size SOCK_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return '0' if everything is fine, '-1' if some errors occurred. The error message is returned
	in the 'errbuf' variable.
*/
int sock_close(SOCKET sock, char *errbuf)
{
	// SHUT_WR: subsequent calls to the send function are disallowed. 
	// For TCP sockets, a FIN will be sent after all data is sent and 
	// acknowledged by the Server.
	if (shutdown(sock, SHUT_WR) )
	{
		sock_geterror("shutdown(): ", errbuf, SOCK_ERRBUF_SIZE);
		// close the socket anyway
		closesocket(sock);
		return -1;
	}

	closesocket(sock);
	return 0;
}






/*!	\ingroup remote_pri_func
	\brief Checks that the address, port and flags given are valids and it returns an 'addrinfo' stucture.

	This function basically calls the getaddrinfo() calls, and it performs a set of sanity checks
	to control that everything is fine (e.g. a TCP socket cannot have a mcast address, and such).
	If an error occurs, it writes the error message into 'errbuf'.

	\param address: a pointer to a user-allocated buffer containing the network address to check.
	It could be both a numeric - literal address, and it can be NULL or "" (useful in case of a server
	socket which has to bind to all addresses).

	\param port: a pointer to a user-allocated buffer containing the network port to use.

	\param hints: an addrinfo variable (passed by reference) containing the flags needed to create the 
	addrinfo structure appropriately.

	\param addrinfo: it represents the true returning value. This is a pointer to an addrinfo variable
	(passed by reference), which will be allocated by this function and returned back to the caller.
	This variable will be used in the next sockets calls.

	\param errbuf: a pointer to a user-allocated buffer (of size SOCK_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return '0' if everything is fine, '-1' if some errors occurred. The error message is returned
	in the 'errbuf' variable. The addrinfo variable that has to be used in the following sockets calls is 
	returned into the addrinfo parameter.

	\warning The 'addrinfo' variable has to be deleted by the programmer by calling freeaddrinfo() when
	it is no longer needed.
*/
int sock_validaddr(const char *address, const char *port,
							struct addrinfo *hints, struct addrinfo **addrinfo, char *errbuf)
{
int retval;
	
	retval = getaddrinfo(address, port, hints, addrinfo);
	if (retval != 0)
	{
		// if the getaddrinfo() fails, you have to use gai_strerror(), instead of using the standard
		// error routines (WSAGetLastError() in Win32 anderrono in UNIX)
		snprintf(errbuf, SOCK_ERRBUF_SIZE, "getaddrinfo() %s", gai_strerror(retval));
		return -1;
	}
/*!
	\warning SOCKET: I should check all the accept() in order to bind to all addresses (in case
	addrinfo has more han one pointers, and all connect() to use all addresses (in the case the firs one fails)
*/

	// This software only supports PF_INET and PF_INET6.
	if (( (*addrinfo)->ai_family != PF_INET) && ( (*addrinfo)->ai_family != PF_INET6))
	{
		snprintf(errbuf, SOCK_ERRBUF_SIZE, "getaddrinfo(): socket type not supported");
		return -1;
	}

	if ( ( (*addrinfo)->ai_socktype == SOCK_STREAM) && (sock_ismcastaddr( (*addrinfo)->ai_addr) == 0) )
	{
		snprintf(errbuf, SOCK_ERRBUF_SIZE, "getaddrinfo(): multicast addresses are not valid when using TCP streams");
		return -1;
	}

	return 0;
}



/*!	\ingroup remote_pri_func
	\brief It sends the amount of data contained into 'buffer' on the given socket.

	This function basically calls the send() socket function and it checks that all
	the data specified in 'buffer' (of size 'size') will be sent. If an error occurs, 
	it writes the error message into 'errbuf'.
	In case the socket buffer does not have enough space, it loops until all data 
	has been sent.

	\param socket: the connected socket currently opened.

	\param buffer: a char pointer to a user-allocated buffer in which data is contained.

	\param size: number of bytes that have to be sent.

	\param errbuf: a pointer to a user-allocated buffer (of size SOCK_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return '0' if everything is fine, '-1' if some errors occurred. The error message is returned
	in the 'errbuf' variable.
*/
int sock_send(SOCKET socket, const char *buffer, int size, char *errbuf)
{
int nsent;

send:
#ifdef linux
/*
	Another pain... in Linux there's this flag
	MSG_NOSIGNAL
		Requests not to send SIGPIPE on  errors  on  stream
		oriented sockets when the other end breaks the con­
		nection. The EPIPE error is still returned.
*/
	nsent = send(socket, buffer, size, MSG_NOSIGNAL);
#else
	nsent = send(socket, buffer, size, 0);
#endif

	if (nsent == -1)
	{
		sock_geterror("send(): ", errbuf, SOCK_ERRBUF_SIZE);
		return -1;
	}

	if (nsent != size)
	{
		size-= nsent;
		buffer+= nsent;
		goto send;
	}

	return 0;
}


/*!	\ingroup remote_pri_func
	\brief It copies the amount of data contained into 'buffer' into 'tempbuf'.
	and it checks for buffer overflows.

	This function basically copies 'size' bytes of data contained into 'buffer'
	into 'tempbuf', starting at offset 'offset'. Before that, it checks that the 
	resulting buffer will not be larger	than 'totsize'. Finally, it updates 
	the 'offset' variable in order to point to the first empty location of the buffer.

	In case the function is called with 'checkonly' equal to 1, it does not copy
	the data into the buffer. It only checks for buffer overflows and it updates the
	'offset' variable. This mode can be useful when the buffer already contains the 
	data (maybe because the producer writes directly into the target buffer), so
	only the buffer overflow check has to be made.
	In this case, both 'buffer' and 'tempbuf' can be NULL values.

	\param buffer: a char pointer to a user-allocated buffer that keeps the data
	that has to be copied.

	\param size: number of bytes that have to be copied.

	\param tempbuf: user-allocated buffer (of size 'totsize') in which data
	has to be copied.

	\param offset: an index into 'tempbuf' which keeps the location of its first
	empty location.

	\param totsize: total size of the buffer in which data is being copied.

	\param checkonly: '1' if we do not want to copy data into the buffer and we
	want just do a buffer ovreflow control, '0' if data has to be copied as well.

	\param errbuf: a pointer to a user-allocated buffer (of size SOCK_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return '0' if everything is fine, '-1' if some errors occurred. The error message
	is returned in the 'errbuf' variable. When the function returns, 'tempbuf' will 
	have the new string appended, and 'offset' will keep the length of that buffer.
	In case of 'checkonly == 1', data is not copied, but 'offset' is updated in any case.

	\warning This function assumes that the buffer in which data has to be stored is
	large 'totbuf' bytes.

	\warning In case of 'checkonly', be carefully to call this function *before* copying
	the data into the buffer. Otherwise, the control about the buffer overflow is useless.
*/
int sock_bufferize(const char *buffer, int size, char *tempbuf, int *offset, int totsize, int checkonly, char *errbuf)
{

	if ((*offset + size) > totsize)
	{
		snprintf(errbuf, SOCK_ERRBUF_SIZE, "Not enough space in the temporary send buffer.");
		return -1;
	};

	if (!checkonly)
		memcpy(tempbuf + (*offset), buffer, size);

	(*offset)+= size;

	return 0;
}



/*!	\ingroup remote_pri_func
	\brief It waits on a connected socket and it manages to receive exactly 'size' bytes.

	This function basically calls the recv() socket function and it checks that no
	error occurred. If that happens, it writes the error message into 'errbuf'.
	In case the socket does not have enough data available, it cycles on the recv()
	util the requested data (of size 'size') is arrived.

	\param sock: the connected socket currently opened.

	\param buffer: a char pointer to a user-allocated buffer in which data has to be stored

	\param size: size of the allocated buffer. WARNING: this indicates the number of bytes
	that we are expecting to be read.
	This function (differenctly from the rpcap_recv_dgram() ) block until the number of
	bytes read is equal to 'size'.

	\param errbuf: a pointer to a user-allocated buffer (of size SOCK_ERRBUF_SIZE)
	that will contain the error message (in case there is one), due to a network problem.

	\return the number of bytes read if everything is fine, '-1' if some errors occurred.
	The error message is returned in the 'errbuf' variable.
*/
int sock_recv(SOCKET sock, char *buffer, int size, char *errbuf)
{
int nread;
int totread= 0;
	// We can obtain the same result using the MSG_WAITALL flag
	// However, this is not supported by recv() in Win32

	if (size == 0)
	{
		SOCK_ASSERT("I have been requested to read zero bytes", 1);
		return 0;
	}

again:
	nread= recv(sock, &(buffer[totread]), size - totread, 0);

	if (nread == -1)
	{
		sock_geterror("recv(): ", errbuf, SOCK_ERRBUF_SIZE);
		return -1;
	}

	if (nread == 0)
	{
		snprintf(errbuf, SOCK_ERRBUF_SIZE, "The other host terminated the connection.");
		return -1;
	}

	totread+= nread;

	if (totread != size)
		goto again;

	return totread;
}



/*!	\ingroup remote_pri_func
	\brief It waits on a connected socket and it manages to receive one message.

	There is a difference here between this function and the sock_recv(): the
	previous one will loop until 'size' bytes are read (i.e. until a message 
	with the expected size has arrived); here just one recv() is done, and all what 
	is waiting in the socket bufer is read all at once.

	This function is required for UDP sockets, in which the message has to
	be read all at once, otherwise it is discarded.
	Therefore we have to call this function with a large buffer, in order to
	be sure no data is lost. It follow that the number of bytes received
	is usually less than the size buffer: therefore we must exit anyway, even
	if the number of bytes read is less than the size of the buffer.

	\param sock: the connected socket currently opened.

	\param buffer: a char pointer to a user-allocated buffer in which data has to 
	be stored.

	\param size: size of the allocated buffer. WARNING: this indicates the maximum
	number of bytes that we can read.
	This function (differenctly from the rpcap_recv_dgram() ) does not block if 
	the number of bytes read is equal to 'size'.

	\param errbuf: a pointer to a user-allocated buffer (of size SOCK_ERRBUF_SIZE)
	that will contain the error message (in case there is one), due to a network problem.

	\return the number of bytes read if everything is fine, '-1' if some errors occurred.
	The error message is returned in the 'errbuf' variable.
*/
int sock_recv_dgram(SOCKET sock, char *buffer, int size, char *errbuf)
{
int nread;

	nread= recv(sock, buffer, size, 0);

	if (nread == -1)
	{
		sock_geterror("recv(): ", errbuf, SOCK_ERRBUF_SIZE);
		return -1;
	}

	return nread;
}


/*!	\ingroup remote_pri_func
	\brief It discards N bytes that are currently waiting to be read on the current socket.

	This function is useful in case we receive a message we cannot undestand (e.g.
	wrong version number when receiving a network packet), so that we have to discard all 
	data before reading a new message.

	This function will read 'size' bytes from the socket and discard them.
	It defines an internal buffer in which data will be copied; however, in case
	this buffer is not large enough, it will cycle in order to read everything as well.

	\param sock: the connected socket currently opened.

	\param size: number of bytes that have to be discarded.

	\param errbuf: a pointer to a user-allocated buffer (of size SOCK_ERRBUF_SIZE)
	that will contain the error message (in case there is one), due to a network problem.

	\return '0' if everything is fine, '-1' if some errors occurred.
	The error message is returned in the 'errbuf' variable.
*/
int sock_discard(SOCKET sock, int size, char *errbuf)
{
#define TEMP_BUF_SIZE 65536

char buffer[TEMP_BUF_SIZE];		// network buffer, to be used when the message is discarded

	// A static allocation avoids the need of a 'malloc()' each time we want to discard a message
	// Our feeling is that a buffer if 65536 is enough for mot of the application;
	// in case this is not enough, the "while" loop discards the message by calling the 
	// sockrecv() several times.

	while (size > TEMP_BUF_SIZE)
	{
		if (sock_recv(sock, buffer, TEMP_BUF_SIZE, errbuf) == -1)
			return -1;
		size-= TEMP_BUF_SIZE;
	}

	// If there is still data to be discarded
	// In this case, the data can fit into the temporaty buffer
	if (size)
	{
		if (sock_recv(sock, buffer, size, errbuf) == -1)
			return -1;
	}

	SOCK_ASSERT("I'm currently discarding data\n", 1);

	return 0;
}



/*!	\ingroup remote_pri_func
	\brief Checks that one host (identified by the sockaddr_storage structure) belongs to an 'allowed list'.

	This function is useful after an accept() call in order to check if the connecting
	host is allowed to connect to me. To do that, we have a buffer that keeps the list of the
	allowed host; this function checks the sockaddr_storage structure of the connecting host 
	against this host list, and it returns '0' is the host is included in this list.

	\param hostlist: a string that contains the list of the allowed host.

	\param sep: a string that keeps the separators used between the hosts (for example the 
	space character) in the host list.

	\param from: a sockaddr_storage structure, as it is returned by the accept() call.

	\param errbuf: a pointer to a user-allocated buffer (of size SOCK_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return '0' if everything is fine, '-1' if some errors occurred.
	The error message is returned in the 'errbuf' variable.
*/
int	sock_check_hostlist(char *hostlist, const char *sep, struct sockaddr_storage *from, char *errbuf)
{
	// checks if the connecting host is among the ones allowed
	if (hostlist[0])
	{
	char *token;					// temp, needed to separate items into the hostlist
	struct addrinfo *addrinfo, *ai_next;
	char *temphostlist;

		temphostlist= (char *) malloc (strlen(hostlist) + 1);
		if (temphostlist == NULL)
		{
			sock_geterror("sock_check_hostlist(), malloc() failed", errbuf, SOCK_ERRBUF_SIZE);
			return -1;
		}
		
		// The problem is that strtok modifies the original variable by putting '0' at the end of each token
		// So, we have to create a new temporary string in which the original content is kept
		strcpy(temphostlist, hostlist);

		token= strtok(temphostlist, sep);

		while( token != NULL )
		{
		struct addrinfo hints;
		int retval;

			addrinfo = NULL;
			memset(&hints, 0, sizeof (struct addrinfo) );
			hints.ai_family = PF_UNSPEC;
			hints.ai_socktype= SOCK_STREAM;
	
			retval = getaddrinfo(token, "0", &hints, &addrinfo);
			if (retval != 0)
			{
				snprintf(errbuf, SOCK_ERRBUF_SIZE, "getaddrinfo() %s", gai_strerror(retval));
				SOCK_ASSERT(errbuf, 1);

				// Get next token
				token = strtok( NULL, sep);
				continue;
			}

			// ai_next is required to preserve the content of addrinfo, in order to deallocate it properly
			ai_next= addrinfo;
			while(ai_next)
			{
				if (sock_cmpaddr(from, (struct sockaddr_storage *) ai_next->ai_addr) == 0)
				{
					free(temphostlist);
					return 0;
				}

				// If we are here, it means that the current address does not matches
				// Let's try with the next one in the header chain
				ai_next= ai_next->ai_next;
			}

			freeaddrinfo(addrinfo);
			addrinfo= NULL;
			// Get next token
			token = strtok( NULL, sep);
		}

		if (addrinfo)
		{
			freeaddrinfo(addrinfo);
			addrinfo= NULL;
		}

		snprintf(errbuf, SOCK_ERRBUF_SIZE, "The host is not in the allowed host list. Connection refused.");
		free(temphostlist);
		return -1;
	}

	return 0;
}


/*!	\ingroup remote_pri_func
	\brief Compares two addresses contained into two sockaddr_storage structures.

	This function is useful to compare two addresses, given their internal representation,
	i.e. an sockaddr_storage structure.

	The two structures do not need to be sockaddr_storage; you can have both 'sockaddr_in' and
	sockaddr_in6, properly acsted in order to be compliant to the function interface.

	This function will return '0' if the two addresses matches, '-1' if not.

	\param first: a sockaddr_storage structure, (for example the one that is returned by an 
	accept() call), containing the first address to compare.

	\param second: a sockaddr_storage structure containing the second address to compare.

	\return '0' if the addresses are equal, '-1' if they are different.
*/
int sock_cmpaddr(struct sockaddr_storage *first, struct sockaddr_storage *second)
{
	if (first->ss_family == second->ss_family)
	{
		if (first->ss_family == AF_INET)
		{
			if (memcmp(		&(((struct sockaddr_in *) first)->sin_addr), 
							&(((struct sockaddr_in *) second)->sin_addr),
							sizeof(struct in_addr) ) == 0)
								return 0;
		}
		else // address family is AF_INET6
		{
			if (memcmp(		&(((struct sockaddr_in6 *) first)->sin6_addr), 
							&(((struct sockaddr_in6 *) second)->sin6_addr),
							sizeof(struct in6_addr) ) == 0)
								return 0;
		}
	}

	return -1;
}

