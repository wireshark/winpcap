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


#include <string.h>		// for strlen(), ...
#include <stdlib.h>		// for malloc(), free(), ...
#include <stdarg.h>		// for functions with variable number of arguments
#include <errno.h>		// for the errno variable
#include <pcap.h>
#include <pcap-int.h>
#include <pcap-remote.h>
#include <sockutils.h>


/*!
	\file pcap-remote.c
	
	This file keeps all the new funtions that are needed for the RPCAP protocol.
	Almost all the pcap functions need to be modified in order to become compatible
	with the RPCAP protocol. However, you can find here only the ones that are completely new.

	This file keeps also the functions that are 'private', i.e. are needed by the RPCAP
	protocol but are not exported to the user.

	\warning All the RPCAP functions that are allowed to return a buffer containing
	the error description can return max PCAP_ERRBUF_SIZE characters.
	However there is no guarantees that the string will be zero-terminated.
	Best practice is to define the errbuf variable as a char of size 'PCAP_ERRBUF_SIZE+1'
	and to insert manually a NULL character at the end of the buffer. This will
	guarantee that no buffer overflows occur even if we use the printf() to show
	the error on the screen.
*/



#define PCAP_STATS_STANDARD 0	/*!< Used by pcap_stats_remote to see if we want standard or extended statistics */
#define PCAP_STATS_EX 1			/*!< Used by pcap_stats_remote to see if we want standard or extended statistics */


/*
	\brief Global variable; needed to keep the message due to an error that we want to discard.
	
	This can happen, for instance, because we already have an error message and we want to keep 
	the first one.
*/
char fakeerrbuf[PCAP_ERRBUF_SIZE + 1];


//! Keeps a list of all the opened connections in the active mode.
struct activehosts *activeHosts;


/****************************************************
 *                                                  *
 * Locally defined functions                        *
 *                                                  *
 ****************************************************/
void rpcap_thrdatamain_stream(void *ptr);
void rpcap_thrdatamain_dgram(void *ptr);
int rpcap_checkver(SOCKET sock, struct rpcap_header *header, char *errbuf);
struct pcap_stat *rpcap_stats_remote(pcap_t *p, struct pcap_stat *ps, int mode);
int pcap_pack_bpffilter(pcap_t *fp, char *sendbuf, int *sendbufidx, struct bpf_program *prog);




/****************************************************
 *                                                  *
 * Function bodies                                 *
 *                                                  *
 ****************************************************/

/*!	\ingroup remote_pri_func
	\brief 	It traslates (i.e. de-serializes) a 'sockaddr_storage' structure from 
	the network byte order to the host byte order.

	It accepts a 'sockaddr_storage' structure as it is received from the network and it 
	converts it into the host byte order (by means of a set of ntoh() ).
	The function will allocate the 'sockaddrout' variable according to the address family
	in use. In case the address does not belong to the AF_INET nor AF_INET6 families,
	'sockaddrout' is not allocated and a NULL pointer is returned.
	This usually happens because that address does not exist on the other host, so the 
	RPCAP daemon sent a 'sockaddr_storage' structure containing all 'zero' values.

	\param sockaddrin: a 'sockaddr_storage' pointer to the variable that has to be
	de-serialized. 

	\param sockaddrout: a 'sockaddr_storage' pointer to the variable that will contain
	the de-serialized data. The structure returned can be either a 'sockaddr_in' or 'sockaddr_in6'.
	This variable will be allocated automatically inside this function.

	\param errbuf: a pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return '0' if everything is fine, '-1' if some errors occurred. Basically, the error
	can be only the fact that the malloc() failed to allocate memory.
	The error message is returned in the 'errbuf' variable, while the deserialized address
	is returned into the 'sockaddrout' variable.

	\warning This function supports only AF_INET and AF_INET6 address families.

	\warning The sockaddrout (if not NULL) must be deallocated by the user.
*/
int rpcap_deseraddr(struct sockaddr_storage *sockaddrin, struct sockaddr_storage **sockaddrout, char *errbuf)
{
	// Warning: we support only AF_INET and AF_INET6
	if ( ntohs(sockaddrin->ss_family) == AF_INET)
	{
	struct sockaddr_in *sockaddr;
	
		sockaddr= (struct sockaddr_in *) sockaddrin;
		sockaddr->sin_family= ntohs(sockaddr->sin_family);
		sockaddr->sin_port= ntohs(sockaddr->sin_port);

		(*sockaddrout)= (struct sockaddr_storage *) malloc ( sizeof(struct sockaddr_in) );
		if ( (*sockaddrout) == NULL)
		{
			snprintf(errbuf, PCAP_ERRBUF_SIZE, "malloc() failed: %s", pcap_strerror(errno));
			return -1;
		}
		memcpy( *sockaddrout, sockaddr, sizeof(struct sockaddr_in) );
		return 0;
	}
	if ( ntohs(sockaddrin->ss_family) == AF_INET6)
	{
	struct sockaddr_in6 *sockaddr;
	
		sockaddr= (struct sockaddr_in6 *) sockaddrin;
		sockaddr->sin6_family= ntohs(sockaddr->sin6_family);
		sockaddr->sin6_port= ntohs(sockaddr->sin6_port);
		sockaddr->sin6_flowinfo= ntohl(sockaddr->sin6_flowinfo);
		sockaddr->sin6_scope_id= ntohl(sockaddr->sin6_scope_id);

		(*sockaddrout)= (struct sockaddr_storage *) malloc ( sizeof(struct sockaddr_in6) );
		if ( (*sockaddrout) == NULL)
		{
			snprintf(errbuf, PCAP_ERRBUF_SIZE, "malloc() failed: %s", pcap_strerror(errno));
			return -1;
		}
		memcpy( *sockaddrout, sockaddr, sizeof(struct sockaddr_in6) );
		return 0;
	}

	// It is neither AF_INET nor AF_INET6
	*sockaddrout= NULL;
	return 0;
}



/*!	\ingroup remote_pri_func

	\brief It reads a packet from the network socket. This does not make used of 
	callback (hence the "nocb" string into its name).

	This function is called by the several pcap_next_ex() when they detect that 
	we have a remote capture and they are the client side. In that case, they need
	to read packets from the socket.

	Parameters and return values are exactly the same of the pcap_next_ex().

	\warning By choice, this function does not make use of semaphores. A smarter
	implementation should put a semaphore into the data thread, and a signal will
	be raised as soon as there is data into the socket buffer. 
	However this is complicated and it does not bring any advantages when reading
	from the network, in which network delays can be much more important than
	these optimizations. Therefore, we chose the following approach:
	- the 'timeout' chosen by the user is split in two (half on the server side,
	with the usual meaning, and half on the client side)
	- this function checks for packets; if there are no packets, it waits for
	timeout/2 and then it checks again. If packets are still missing, it returns,
	otherwise it reads packets.
*/
int pcap_read_nocb_remote(pcap_t *p, struct pcap_pkthdr **pkt_header, u_char **pkt_data)
{
int cc;
register u_char *ep, *bp;

	cc= p->cc;
	bp= p->bp;

	ep = bp + cc;

	if (bp < ep) 
	{
		register int caplen, hdrlen;
again:
		caplen = ((struct pcap_pkthdr *)bp)->caplen;
		hdrlen= sizeof(struct pcap_pkthdr);
			
		/*
		 * XXX A bpf_hdr matches a pcap_pkthdr.
		 */
		*pkt_header = (struct pcap_pkthdr *) bp;
		*pkt_data = bp + hdrlen;
		bp += BPF_WORDALIGN(caplen + hdrlen);
		
		p->bp = bp;
		p->cc = ep - bp;
		return (1);
	}
	else
	{
		p->cc = 0;
		// If there are no packets, the read thread must be suspended
// another horrible difference...
#ifdef linux
		pthread_suspend(p->md.timeout);
#endif
#ifdef WIN32
		pthread_suspend(p->timeout);
#endif
		// check if now if have data; otherwise returns
		cc= p->cc;
		bp= p->bp;

		ep = bp + cc;

		if (bp < ep) 
			goto again;

		return (0);
	}
}


/*!	\ingroup remote_pri_func

	\brief It reads a packet from the network socket.

	This function is called by the several pcap_read() when they detect that 
	we have a remote capture and they are the client side. In that case, they need
	to read packets from the socket.

	This function relies on the pcap_read_nocb_remote to deliver packets. The
	difference, here, is that as soon as a packet is read, it is delivered
	to the application by means of a callback function.

	Parameters and return values are exactly the same of the pcap_read().
*/
int pcap_read_remote(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
{
struct pcap_pkthdr *pkt_header;
u_char *pkt_data;
int n = 0;

	while ( (++n <= cnt) || (cnt < 0) )
	{
		if (pcap_read_nocb_remote(p, &pkt_header, &pkt_data) )
			(*callback)(user, pkt_header, pkt_data);
		else
			return n;
	}
	return n;
}


/*!	\ingroup remote_pri_func

	\brief It sends a CLOSE command to the capture server.

	This function is called when the user wants to close a pcap_t adapter.
	In case we're capturing from the network, it sends a command to the other 
	peer that says 'ok, let's stop capturing'.
	This function is called automatically when the user calls the pcap_close().

	Parameters and return values are exactly the same of the pcap_close().

	\warning Since we're closing the connection, we do not check for errors.
*/
void pcap_close_remote(pcap_t *fp)
{
struct rpcap_header header;		// header of the RPCAP packet
struct activehosts *temp;		// temp var needed to scan the host list chain, to detect if we're in active mode
int active= 0;					// active mode or not?

	// detect if we're in active mode
	temp= activeHosts;
	while (temp)
	{
		if (temp->sockctrl == fp->rmt_sockctrl)
		{
			active= 1;
			break;
		}
		temp= temp->next;
	}

	if (!active)
	{
		rpcap_createhdr( &header, RPCAP_MSG_CLOSE, 0, 0);

		// I don't check for errors, since I'm going to close everything
		sock_send(fp->rmt_sockctrl, (char *) &header, sizeof (struct rpcap_header), fakeerrbuf);
	}
	else
	{
		rpcap_createhdr( &header, RPCAP_MSG_ENDCAP_REQ, 0, 0);

		// I don't check for errors, since I'm going to close everything
		sock_send(fp->rmt_sockctrl, (char *) &header, sizeof (struct rpcap_header), fakeerrbuf);

		// wait for the answer
		// Don't check what we got, since the present libpcap does not uses this pcap_t anymore
		sock_recv(fp->rmt_sockctrl, (char *) &header, sizeof(struct rpcap_header), fakeerrbuf);

		if ( ntohl(header.plen) != 0)
			sock_discard(fp->rmt_sockctrl, ntohl(header.plen), fakeerrbuf);
	}


	if (fp->rmt_threaddata)
	{
		pthread_cancel(fp->rmt_threaddata);
		fp->rmt_threaddata= 0;
	}

	if (fp->rmt_sockdata)
	{
		sock_close(fp->rmt_sockdata, fakeerrbuf);
		fp->rmt_sockdata= 0;
	}

	if ( (!active) && (fp->rmt_sockctrl) )
		sock_close(fp->rmt_sockctrl, fakeerrbuf);

	fp->rmt_sockctrl= 0;

	// To avoid inconsistencies in the number of sock_init()
	sock_cleanup();
}



/*!	\ingroup remote_pri_func

	\brief It retrieves network statistics from the other peer.

	This function is just a void cointainer, since the work is done by the rpcap_stats_remote().
	See that funcion for more details.

	Parameters and return values are exactly the same of the pcap_stats().
*/
int pcap_stats_remote(pcap_t *p, struct pcap_stat *ps)
{
struct pcap_stat *retval;

	retval= rpcap_stats_remote(p, ps, PCAP_STATS_STANDARD);

	if (retval)
		return 0;
	else
		return -1;
}



/*!	\ingroup remote_pri_func

	\brief It retrieves network statistics from the other peer.

	This function is just a void cointainer, since the work is done by the rpcap_stats_remote().
	See that funcion for more details.

	Parameters and return values are exactly the same of the pcap_stats_ex().
*/
struct pcap_stat *pcap_stats_ex_remote(pcap_t *p)
{
	// '0' (third param) means 'standard pcap_stats()'
	return (rpcap_stats_remote(p, &(p->md.stat), PCAP_STATS_EX));
}



/*!	\ingroup remote_pri_func

	\brief It retrieves network statistics from the other peer.

	This function can be called in two modes:
	- PCAP_STATS_STANDARD: if we want just standard statistics (i.e. the pcap_stats() )
	- PCAP_STATS_EX: if we want extended statistics (i.e. the pcap_stats_ex() )

	This 'mode' parameter is needed because in the standard pcap_stats() the variable that keeps the
	statistics is allocated by the user. Unfortunately, this structure has been extended in order
	to keep new stats. However, if the user has a smaller structure and it passes it to the pcap_stats,
	thid function will try to fill in more data than the size of the structure, so that the application 
	goes in memory overflow.
	So, we need to know it we have to copy just the standard fields, or the extended fields as well.

	In case we want to copy the extended fields as well, the problem of memory overflow does no 
	longer exist because the structure pcap_stat is no longer allocated by the program;
	it is allocated by the library instead.

	\param p: the pcap_t structure related to the current instance.

	\param ps: a 'pcap_stat' structure, needed for compatibility with pcap_stat(), in which
	the structure is allocated by the user. In case of pcap_stats_ex, this structure and the
	function return value point to the same variable.
	
	\param mode: one of PCAP_STATS_STANDARD or PCAP_STATS_EX.

	\return The structure that keeps the statistics, or NULL in case of error.
	The error string is placed in the pcap_t structure.
*/
struct pcap_stat *rpcap_stats_remote(pcap_t *p, struct pcap_stat *ps, int mode)
{
struct rpcap_header header;			// header of the RPCAP packet
struct rpcap_stats netstats;		// statistics sent on the network
unsigned int nread= 0;				// number of bytes of the payload read from the socket
int retval;							// temp variable which stores functions return value

	rpcap_createhdr(&header, RPCAP_MSG_STATS_REQ, 0, 0);

	// Send the PCAP_STATS command
	if (sock_send(p->rmt_sockctrl, (char *) &header, sizeof (struct rpcap_header), p->errbuf) )
		goto error;

	// Receive the RPCAP stats reply message
	if (sock_recv(p->rmt_sockctrl, (char *) &header, sizeof(struct rpcap_header), p->errbuf) == -1)
		goto error;

	// Checks if the message is correct
	retval= rpcap_checkmsg(p->errbuf, p->rmt_sockctrl, &header, RPCAP_MSG_STATS_REPLY, RPCAP_MSG_ERROR, 0);

	if (retval != RPCAP_MSG_STATS_REPLY)		// the message is not the one expected
	{
		switch (retval)
		{
			case -3:		// Unrecoverable network error
			case -2:		// The other endpoint send a message that is not allowed here
			case -1:	// The other endpoint has a version number that is not compatible with our
				goto error;

			case RPCAP_MSG_ERROR:		// The other endpoint reported an error
				// Update nread, since the rpcap_checkmsg() already purged the buffer
				nread = ntohl(header.plen);

				// Do nothing; just exit; the error code is already into the errbuf
				goto error;

			default:
			{
				snprintf(p->errbuf, PCAP_ERRBUF_SIZE, "Internal error");
				goto error;
			};
		}
	}

	if ( (nread= sock_recv(p->rmt_sockctrl, (char *) &netstats, sizeof(struct rpcap_stats), p->errbuf)) == -1)
		goto error;

	if (mode == PCAP_STATS_STANDARD)
	{
		ps->ps_drop= ntohl(netstats.krnldrop);
		ps->ps_ifdrop= ntohl(netstats.ifdrop);
		ps->ps_recv= ntohl(netstats.ifrecv);
	}
	else
	{
		ps->ps_capt= p->md.TotCapt;
		ps->ps_drop= ntohl(netstats.krnldrop);
		ps->ps_ifdrop= ntohl(netstats.ifdrop);
		ps->ps_netdrop= p->md.TotNetDrops;
		ps->ps_recv= ntohl(netstats.ifrecv);
		ps->ps_sent= ntohl(netstats.svrcapt);
	}

	// Checks if all the data has been read; if not, discard the data in excess
	if (nread != ntohl(header.plen))
	{
		if (sock_discard(p->rmt_sockctrl, ntohl(header.plen) - nread, fakeerrbuf) == 1)
			goto error;
	}

	return ps;

error:
	if (nread != ntohl(header.plen))
		sock_discard(p->rmt_sockctrl, ntohl(header.plen) - nread, fakeerrbuf);

	return NULL;
}




/*!	\ingroup remote_pri_func

	\brief It opens a remote adapter by opening an RPCAP connection and so on.

	This function does basically the job of pcap_open_live() for a remote interface.
	In other words, we have a pcap_read for win32, which reads packets from NPF,
	another for LINUX, and so on. Now, we have a pcap_opensource_remote() as well.
	The difference, here, is the capture thread does not start until the 
	pcap_startcapture_remote() is called.

	This is because, in remote capture, we cannot start capturing data as soon ad the
	'open adapter' command is sent. Suppose the remote adapter is already overloaded;
	if we start a capture (which, by default, has a NULL filter) the new traffic can
	saturate the network.

	Instead, we want to "open" the adapter, then send a "start capture" command only
	when we're ready to start the capture.
	This funtion does this job: it sends a "open adapter" command (according to the
	RPCAP protocol), but it does not start the capture.

	Since the other libpcap functions do not share this way of life, we have to make
	some dirty things in order to make everyting working.

	\param source: see pcap_open().
	\param auth: see pcap_open().

	\param errbuf: a pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one). It could be either a network problem,
	an RPCAP problem (e.g. authentication failed), and more.

	\return A pointer to a 'pcap_t' which can be used as a parameter to the following
	calls (pcap_compile() and so on) and that specifies an opened WinPcap session. In case of 
	problems, it returns NULL and the 'errbuf' variable keeps the error message.

	\warning In case we call the pcap_compile() and the capture is not started, the filter
	will be saved into the pcap_t structure, and it will be sent to the other host later
	(when the pcap_startcapture_remote() is called).
*/
pcap_t *pcap_opensource_remote(const char *source, struct pcap_rmtauth *auth, char *errbuf)
{
char host[PCAP_BUF_SIZE], ctrlport[PCAP_BUF_SIZE], iface[PCAP_BUF_SIZE];

char sendbuf[RPCAP_NETBUF_SIZE];// temporary buffer in which data to be sent is buffered
int sendbufidx= 0;				// index which keeps the number of bytes currently buffered
pthread_t threaddata= 0;		// handle to the receiving thread
struct pcap *fp= NULL;			// pcap_t main variable
unsigned int nread= 0;			// number of bytes of the payload read from the socket
int retval;						// store the return value of the functions
int active= 0;						// '1' if we're in active mode

// socket-related variables
struct addrinfo hints;			// temp, needed to open a socket connection
struct addrinfo *addrinfo;		// temp, needed to open a socket connection
SOCKET sockctrl= 0;				// socket descriptor of the control connection

// RPCAP-related variables
struct rpcap_header header;			// header of the RPCAP packet
struct rpcap_openreply openreply;	// open reply message


	// determine the type of the source (NULL, file, local, remote)
	// You must have a valid source string even if we're in active mode, because otherwise
	// the call to the following funciton will fail.
	if (pcap_parsesrcstr(source, &retval, host, ctrlport, iface, errbuf) == -1)
		return NULL;

	if ( retval != PCAP_SRC_IFREMOTE)
	{
		snprintf(errbuf, PCAP_ERRBUF_SIZE, "This function is able to open only remote interfaces");
		return NULL;
	}

	addrinfo= NULL;

	// Warning: this call can be the first one called by the user.
	// For this reason, we have to initialize the WinSock support.
	if (sock_init(errbuf) == -1)
		return NULL;

	retval= rpcap_remoteact_getsock(host, errbuf);

	if (retval == -1)
		return NULL;

	// The capturing machine is in active mode
	if (retval)
	{
		sockctrl= retval;
		active= 1;
	}
	else
	{
		memset(&hints, 0, sizeof(struct addrinfo) );
		hints.ai_family = PF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		if ( (ctrlport == NULL) || (ctrlport[0] == 0) )
		{
			// the user chose not to specify the port
			if (sock_validaddr(host, RPCAP_DEFAULT_NETPORT, &hints, &addrinfo, errbuf) == -1)
				return NULL;
		}
		else
		{
			// the user chose not to specify the port
			if (sock_validaddr(host, ctrlport, &hints, &addrinfo, errbuf) == -1)
				return NULL;
		}

		if ( (sockctrl= sock_open(addrinfo, SOCKOPEN_CLIENT, 0, errbuf)) == -1)
			goto error;

		freeaddrinfo(addrinfo);
		addrinfo= NULL;

		if ( rpcap_sendauth(sockctrl, auth, errbuf) == -1)
			goto error;
	}


	// Now it's time to start playing with the RPCAP protocol
	// RPCAP open command: create the request message
	if ( sock_bufferize(NULL, sizeof(struct rpcap_header), NULL, &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errbuf) )
		goto error;

	rpcap_createhdr( (struct rpcap_header *) sendbuf, RPCAP_MSG_OPEN_REQ, 0, strlen(iface) );

	if ( sock_bufferize(iface, strlen(iface), sendbuf, &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_BUFFERIZE, errbuf) )
		goto error;

	if ( sock_send(sockctrl, sendbuf, sendbufidx, errbuf) )
		goto error;

	// Receive the RPCAP open reply message
	if (sock_recv(sockctrl, (char *) &header, sizeof(struct rpcap_header), errbuf) == -1)
		goto error;

	// Checks if the message is correct
	retval= rpcap_checkmsg(errbuf, sockctrl, &header, RPCAP_MSG_OPEN_REPLY, RPCAP_MSG_ERROR, 0);

	if (retval != RPCAP_MSG_OPEN_REPLY)		// the message is not the one expected
	{
		switch (retval)
		{
			case -3:		// Unrecoverable network error
			case -2:		// The other endpoint send a message that is not allowed here
			case -1:	// The other endpoint has a version number that is not compatible with our
				goto error;

			case RPCAP_MSG_ERROR:		// The other endpoint reported an error
				// Update nread, since the rpcap_checkmsg() already purged the buffer
				nread = ntohl(header.plen);
				// Do nothing; just exit; the error code is already into the errbuf
				goto error;

			default:
			{
				snprintf(errbuf, PCAP_ERRBUF_SIZE, "Internal error");
				goto error;
			};
		}
	}


	if ( (nread+= sock_recv(sockctrl, (char *) &openreply, sizeof(struct rpcap_openreply), errbuf)) == -1)
		goto error;

	// Allocates a pcap_t struct for this end of the connection
	fp = (pcap_t *) malloc( sizeof(pcap_t) );
	if (fp == NULL)
	{
		snprintf(errbuf, PCAP_ERRBUF_SIZE, "malloc() failed: %s", pcap_strerror(errno));
		goto error;
	}

	memset(fp, 0, sizeof(pcap_t));

	// Set proper fields into the pcap_t struct
	fp->linktype= ntohl(openreply.linktype);
	fp->tzoff= ntohl(openreply.tzoff);
	fp->rmt_sockctrl= sockctrl;
	fp->rmt_clientside= 1;


	// Checks if all the data has been read; if not, discard the data in excess
	if (nread != ntohl(header.plen))
	{
		if (sock_discard(sockctrl, ntohl(header.plen) - nread, fakeerrbuf) == 1)
			goto error;
	}
	return fp;

error:
// When the connection has been established, we have to close it. So, at the
// beginning of this function, if an error occur we return immediately with
// a return NULL; when the connection is established, we have to come here 
// ('goto error;') in order to close everything properly.

	// Checks if all the data has been read; if not, discard the data in excess
	if (nread != ntohl(header.plen))
		sock_discard(sockctrl, ntohl(header.plen) - nread, fakeerrbuf);

	if (addrinfo)
		freeaddrinfo(addrinfo);

	if (!active)
		sock_close(sockctrl, fakeerrbuf);

	if (fp)
	{
		pcap_close(fp);
		fp= NULL;
	}

	return NULL;
}







/*!	\ingroup remote_pri_func

	\brief It starts a remote capture.

	This function is requires since the RPCAP protocol decouples the 'open' from the
	'start capture' functions.
	This function takes all the parameters needed (which have been stored into the pcap_t structure)
	and sends them to the server.
	If everything is fine, it creates a new child thread that reads data from the network
	and puts data it into the user buffer.
	The pcap_read() will read data from the user buffer, as usual.

	The remote capture acts like a new "kernel", which puts packets directly into
	the buffer pointed by pcap_t.
	In fact, this function does not rely on a kernel that reads packets and put them
	into the user buffer; it has to do that on its own.

	\param fp: the pcap_t descriptor of the device currently open.

	\return '0' if everything is fine, '-1' otherwise. The error message (if one)
	is returned into the 'errbuf' field of the pcap_t structure.
*/
int pcap_startcapture_remote(pcap_t *fp)
{
char sendbuf[RPCAP_NETBUF_SIZE];// temporary buffer in which data to be sent is buffered
int sendbufidx= 0;				// index which keeps the number of bytes currently buffered
char portdata[PCAP_BUF_SIZE];	// temp variable needed to keep the network port for the the data connection
pthread_t threaddata= 0;		// handle to the receiving thread
unsigned int nread= 0;			// number of bytes of the payload read from the socket
int retval;						// store the return value of the functions
int active= 0;					// '1' if we're in active mode
struct activehosts *temp;		// temp var needed to scan the host list chain, to detect if we're in active mode
char host[INET6_ADDRSTRLEN + 1];	// numeric name of the other host

// socket-related variables
struct addrinfo hints;			// temp, needed to open a socket connection
struct addrinfo *addrinfo;		// temp, needed to open a socket connection
SOCKET sockdata= 0;				// socket descriptor of the data connection
struct sockaddr_storage saddr;	// temp, needed to retrieve the network data port chosen on the local machine
socklen_t saddrlen;				// temp, needed to retrieve the network data port chosen on the local machine
int ai_family;					// temp, keeps the address family used by the control connection

// RPCAP-related variables
struct rpcap_header header;					// header of the RPCAP packet
struct rpcap_startcapreq *startcapreq;		// start capture request message
struct rpcap_startcapreply startcapreply;	// start capture reply message


	// detect if we're in active mode
	temp= activeHosts;
	while (temp)
	{
		if (temp->sockctrl == fp->rmt_sockctrl)
		{
			active= 1;
			break;
		}
		temp= temp->next;
	}

	addrinfo= NULL;

	// Gets the complete sockaddr structure used in the ctrl connection
	// This is needed to get the address family of the control socket
	// Tip: I cannot save the ai_family of the ctrl sock in the pcap_t struct,
	// since the ctrl socket can already be open in case of active mode;
	// so I would have to call getpeername() anyway
	saddrlen = sizeof(struct sockaddr_storage);
	if (getpeername(fp->rmt_sockctrl, (struct sockaddr *) &saddr, &saddrlen) == -1)
	{
		sock_geterror("getsockname(): ", fp->errbuf, PCAP_ERRBUF_SIZE);
		goto error;
	}
	ai_family= ((struct sockaddr_storage *) &saddr)->ss_family;

	// Get the numeric address of the remote host we are connected to
	if (getnameinfo( (struct sockaddr *) &saddr, saddrlen, host, 
			sizeof(host), NULL, 0, NI_NUMERICHOST) )
	{
		sock_geterror("getnameinfo(): ", fp->errbuf, PCAP_ERRBUF_SIZE);
		goto error;
	}

	/*
		Data connection is opened by the server toward the client if:
		- we're using TCP, and the user wants us to be in active mode
		- we're using UDP
	*/
	if ( (active) || (fp->rmt_flags & PCAP_OPENFLAG_UDP_DP) )
	{
		// We have to create a new socket to receive packets
		// We have to do that immediately, since we have to tell the other 
		// end which network port we picked up
		memset(&hints, 0, sizeof(struct addrinfo) );
		// TEMP addrinfo is NULL in case of active
		hints.ai_family = ai_family;	// Use the same address family of the control socket
		hints.ai_socktype = (fp->rmt_flags & PCAP_OPENFLAG_UDP_DP) ? SOCK_DGRAM : SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;	// Data connection is opened by the server toward the client

		// Let's the server pick up a free network port for us
		if (sock_validaddr(NULL, "0", &hints, &addrinfo, fp->errbuf) == -1)
			goto error;

		if ( (sockdata= sock_open(addrinfo, SOCKOPEN_SERVER, 1 /* max 1 connection in queue */, fp->errbuf)) == -1)
			goto error;

		// addrinfo is no longer used
		freeaddrinfo(addrinfo);
		addrinfo= NULL;

		// get the complete sockaddr structure used in the data connection
		saddrlen = sizeof(struct sockaddr_storage);
		if (getsockname(sockdata, (struct sockaddr *) &saddr, &saddrlen) == -1)
		{
			sock_geterror("getsockname(): ", fp->errbuf, PCAP_ERRBUF_SIZE);
			goto error;
		}

		// Get the local port the system picked up
		if (getnameinfo( (struct sockaddr *) &saddr, saddrlen, NULL, 
				0, portdata, sizeof(portdata), NI_NUMERICSERV) )
		{
			sock_geterror("getnameinfo(): ", fp->errbuf, PCAP_ERRBUF_SIZE);
			goto error;
		}
	}

	// Now it's time to start playing with the RPCAP protocol
	// RPCAP start ca[ture command: create the request message
	if ( sock_bufferize(NULL, sizeof(struct rpcap_header), NULL, &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, fp->errbuf) )
		goto error;

	rpcap_createhdr( (struct rpcap_header *) sendbuf, RPCAP_MSG_STARTCAP_REQ, 0,
		sizeof(struct rpcap_startcapreq) + sizeof(struct rpcap_filter) + fp->fcode.bf_len * sizeof(struct rpcap_filterbpf_insn) );

	// Fill the structure needed to open an adapter remotely
	startcapreq= (struct rpcap_startcapreq *) &sendbuf[sendbufidx];

	if ( sock_bufferize(NULL, sizeof(struct rpcap_startcapreq), NULL, &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, fp->errbuf) )
		goto error;

	memset(startcapreq, 0, sizeof(struct rpcap_startcapreq) );

	// By default, apply half the timeout on one side, half of the other
#ifdef linux
	fp->md.timeout= fp->md.timeout/2;
	startcapreq->read_timeout= htonl(fp->md.timeout);
#endif
#ifdef WIN32
	fp->timeout= fp->timeout/2;
	startcapreq->read_timeout= htonl(fp->timeout);
#endif

	// portdata on the openreq is meaningful only if we're in active mode
	if ( (active) || (fp->rmt_flags & PCAP_OPENFLAG_UDP_DP) )
	{
		sscanf(portdata, "%d", &(startcapreq->portdata));
		startcapreq->portdata= htons(startcapreq->portdata);
	}

	startcapreq->snaplen= htonl(fp->snapshot);
	startcapreq->flags= 0;
	if (fp->rmt_flags & PCAP_OPENFLAG_PROMISCUOUS)
		startcapreq->flags|= RPCAP_STARTCAPREQ_FLAG_PROMISC;
	if (fp->rmt_flags & PCAP_OPENFLAG_UDP_DP)
		startcapreq->flags|= RPCAP_STARTCAPREQ_FLAG_DGRAM;
	if (active)
		startcapreq->flags|= RPCAP_STARTCAPREQ_FLAG_SERVEROPEN;

	startcapreq->flags= htons(startcapreq->flags);

	// Pack the capture filter
	if (pcap_pack_bpffilter(fp, &sendbuf[sendbufidx], &sendbufidx, &fp->fcode) )
		goto error;

	if ( sock_send(fp->rmt_sockctrl, sendbuf, sendbufidx, fp->errbuf) )
		goto error;


	// Receive the RPCAP start capture reply message
	if (sock_recv(fp->rmt_sockctrl, (char *) &header, sizeof(struct rpcap_header), fp->errbuf) == -1)
		goto error;

	// Checks if the message is correct
	retval= rpcap_checkmsg(fp->errbuf, fp->rmt_sockctrl, &header, RPCAP_MSG_STARTCAP_REPLY, RPCAP_MSG_ERROR, 0);

	if (retval != RPCAP_MSG_STARTCAP_REPLY)		// the message is not the one expected
	{
		switch (retval)
		{
			case -3:		// Unrecoverable network error
			case -2:		// The other endpoint send a message that is not allowed here
			case -1:	// The other endpoint has a version number that is not compatible with our
				goto error;

			case RPCAP_MSG_ERROR:		// The other endpoint reported an error
				// Update nread, since the rpcap_checkmsg() already purged the buffer
				nread = ntohl(header.plen);
				// Do nothing; just exit; the error code is already into the errbuf
				goto error;

			default:
			{
				snprintf(fp->errbuf, PCAP_ERRBUF_SIZE, "Internal error");
				goto error;
			};
		}
	}


	if ( (nread+= sock_recv(fp->rmt_sockctrl, (char *) &startcapreply, sizeof(struct rpcap_startcapreply), fp->errbuf)) == -1)
		goto error;

	// Data connection is opened by the client toward the server
	// This happens because (a) we're not in active mode, and (b) we're not using UDP
	// So, let's start the connection
	if ( (!active) && !(fp->rmt_flags & PCAP_OPENFLAG_UDP_DP) )
	{
		memset(&hints, 0, sizeof(struct addrinfo) );
		hints.ai_family = ai_family;		// Use the same address family of the control socket
		hints.ai_socktype = (fp->rmt_flags & PCAP_OPENFLAG_UDP_DP) ? SOCK_DGRAM : SOCK_STREAM;

		sprintf(portdata, "%d", ntohs(startcapreply.portdata) );

		// Let's the server pick up a free network port for us
		if (sock_validaddr(host, portdata, &hints, &addrinfo, fp->errbuf) == -1)
			goto error;

		if ( (sockdata= sock_open(addrinfo, SOCKOPEN_CLIENT, 0, fp->errbuf)) == -1)
			goto error;

		// addrinfo is no longer used
		freeaddrinfo(addrinfo);
		addrinfo= NULL;
	}


	// Allocates WinPcap/libpcap user buffer
	// It has the same size of the one used on the other side of the connection
	fp->bufsize= ntohl(startcapreply.bufsize);

	/*
		Warning: on some kernels (e.g. linux), the size of the user buffer does not take
		into account the pcap_header and such, and it is set equal to the snaplen.
		In our view, this is wrong (the meaning of the bufsize becames a bit strange).
		So, in our view, bufsize is the whole size of the user buffer.
		In case the bufsize returned is too small, adjust it accordingly.
	*/
	if (fp->bufsize <= fp->snapshot)
		fp->bufsize+= sizeof (struct pcap_pkthdr);

	fp->buffer = (u_char *) malloc(fp->bufsize);
	if (fp->buffer == NULL)
	{
		snprintf(fp->errbuf, PCAP_ERRBUF_SIZE, "malloc: %s", pcap_strerror(errno));
		goto error;
	}

	// The server is trying to connect to me; so it have to block on an accept() call
	// However, if the data connection is UDP, I don't have to call accept()
	if ( (active) && !(fp->rmt_flags & PCAP_OPENFLAG_UDP_DP) )
	{
	SOCKET socktemp;	// We need another socket, since we're going to accept() a connection

		// Connection creation
		saddrlen = sizeof(struct sockaddr_storage);

		socktemp= accept(sockdata, (struct sockaddr *) &saddr, &saddrlen);
		
		if (socktemp == -1)
		{
			sock_geterror("accept(): ", fp->errbuf, PCAP_ERRBUF_SIZE);
			goto error;
		}

		// Now that I accepted the connection, the server socket is no longer needed
		sock_close(sockdata, fp->errbuf);
		sockdata= socktemp;
	}

	fp->rmt_sockdata= sockdata;

	// Now we have to create a new thread to receive packets
	if (fp->rmt_flags & PCAP_OPENFLAG_UDP_DP)
	{
		if ( pthread_create( &threaddata, NULL, (void *) &rpcap_thrdatamain_dgram, (void *) fp) )
		{
			snprintf(fp->errbuf, PCAP_ERRBUF_SIZE, "Error creating the data thread");
			goto error;
		}
	}
	else
	{
		if ( pthread_create( &threaddata, NULL, (void *) &rpcap_thrdatamain_stream, (void *) fp) )
		{
			snprintf(fp->errbuf, PCAP_ERRBUF_SIZE, "Error creating the data thread");
			goto error;
		}
	}

	fp->rmt_threaddata= threaddata;

	// Checks if all the data has been read; if not, discard the data in excess
	if (nread != ntohl(header.plen))
	{
		if (sock_discard(fp->rmt_sockctrl, ntohl(header.plen) - nread, fakeerrbuf) == 1)
			goto error;
	}
	return 0;

error:
// When the connection has been established, we have to close it. So, at the
// beginning of this function, if an error occur we return immediately with
// a return NULL; when the connection is established, we have to come here 
// ('goto error;') in order to close everything properly.

	// Checks if all the data has been read; if not, discard the data in excess
	if (nread != ntohl(header.plen))
		sock_discard(fp->rmt_sockctrl, ntohl(header.plen) - nread, fakeerrbuf);

	if (threaddata)
		pthread_cancel(threaddata);

	if ((sockdata) && (sockdata != -1))		// we can be here because sockdata said 'error'
		sock_close(sockdata, fakeerrbuf);

	if (!active)
		sock_close(fp->rmt_sockctrl, fakeerrbuf);

	if (fp)
	{
		pcap_close(fp);
		fp= NULL;
	}

	return -1;
}





/*!
	\brief Takes a bpf program and sends it to the other host.

	This function can be called in two cases:
	- the pcap_startcapture() is called (we have to send the filter along with
	the 'start capture' command)
	- we want to udpate the filter during a capture (i.e. the pcap_setfilter()
	is called when the capture is still on)

	This function serializes the filter into the sending buffer ('sendbuf', passed
	as a parameter) and return back. It does not send anything on the network.

	\param fp: the pcap_t descriptor of the device currently opened.

	\param sendbuf: the buffer on which the serialized data has to copied.

	\param sendbufidx: it is used to return the abounf of bytes copied into the buffer.

	\param prog: the bpf prgoram we hve to copy.

	\return '0' if everything is fine, '-1' otherwise. The error message (if one)
	is returned into the 'errbuf' field of the pcap_t structure.
*/
int pcap_pack_bpffilter(pcap_t *fp, char *sendbuf, int *sendbufidx, struct bpf_program *prog)
{
struct rpcap_filter *filter;
struct rpcap_filterbpf_insn *insn;
struct bpf_insn *bf_insn;
unsigned int i;

	filter= (struct rpcap_filter *) sendbuf;

	if ( sock_bufferize(NULL, sizeof(struct rpcap_filter), NULL, sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, fp->errbuf) )
		return -1;

	filter->filtertype= htons(RPCAP_UPDATEFILTER_BPF);
	filter->nitems= htonl( (int32) prog->bf_len);

	if ( sock_bufferize(NULL, prog->bf_len * sizeof(struct rpcap_filterbpf_insn), NULL, sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, fp->errbuf) )
		return -1;
	
	insn= (struct rpcap_filterbpf_insn *) (filter + 1);
	bf_insn= prog->bf_insns;

	for (i= 0; i < prog->bf_len; i++)
	{
		insn->code= htons(bf_insn->code);
		insn->jf= bf_insn->jf;
		insn->jt= bf_insn->jt;
		insn->k= htonl(bf_insn->k);

		insn++;
		bf_insn++;
	}
	return 0;
}





/*!	\ingroup remote_pri_func

	\brief Update a filter on a remote host.

	This function is called when the user wants to update a filter.
	In case we're capturing from the network, it sends the filter to the other peer.
	This function is *not* called automatically when the user calls the pcap_setfilter().
	There will be two cases:
	- the capture is already on: in this case, pcap_setfilter() calls pcap_updatefilter_remote()
	- the capture has not started yet: in this case, pcap_setfilter() stores the filter into
	the pcap_t structure, and then the filter is sent with the pcap_startcap().

	Parameters and return values are exactly the same of the pcap_setfilter().

	\warning This function *does not* clear the packet currently into the buffers. Therefore,
	the user has to expect to receive some packets that are related to the previous filter.
	If you want to discard all the packets before applying a new filter, you have to close 
	the current capture session and start a new one.
*/
int pcap_updatefilter_remote(pcap_t *fp, struct bpf_program *prog)
{
int retval;						// general variable used to keep the return value of other functions
char sendbuf[RPCAP_NETBUF_SIZE];// temporary buffer in which data to be sent is buffered
int sendbufidx= 0;				// index which keeps the number of bytes currently buffered
struct rpcap_header header;		// To keep the reply message


	if ( sock_bufferize(NULL, sizeof(struct rpcap_header), NULL, &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, fp->errbuf) )
		return -1;

	rpcap_createhdr( (struct rpcap_header *) sendbuf, RPCAP_MSG_UPDATEFILTER_REQ, 0,
		sizeof(struct rpcap_filter) + prog->bf_len * sizeof(struct rpcap_filterbpf_insn));

	if (pcap_pack_bpffilter(fp, &sendbuf[sendbufidx], &sendbufidx, prog) )
		return -1;

	if ( sock_send(fp->rmt_sockctrl, sendbuf, sendbufidx, fp->errbuf) )
		return -1;

	// Waits for the answer
	if (sock_recv(fp->rmt_sockctrl, (char *) &header, sizeof(struct rpcap_header), fp->errbuf) == -1)
		return -1;

	// Checks if the message is correct
	retval= rpcap_checkmsg(fp->errbuf, fp->rmt_sockctrl, &header, RPCAP_MSG_UPDATEFILTER_REPLY, 0);

	if (retval != RPCAP_MSG_UPDATEFILTER_REPLY)		// the message is not the one expected
	{
		switch (retval)
		{
			case -3:		// Unrecoverable network error
			case -2:		// The other endpoint sent a message that is not allowed here
			case -1:	// The other endpoint has a version number that is not compatible with our
				// Do nothing; just exit from here; the error code is already into the errbuf
				return -1;

			default:
			{
				SOCK_ASSERT("Internal error", 0);
				return -1;
			};
		}
	}

	if (ntohl(header.plen) != 0)	// the message has an unexpected size
	{
		if (sock_discard(fp->rmt_sockctrl, ntohl(header.plen), fp->errbuf) == -1)
			return -1;
	}

	return 0;
}




/*!	\ingroup remote_pri_func

	\brief Send a filter to a remote host.

	This function is called when the user wants to set a filter.
	In case we're capturing from the network, it sends the filter to the other peer.
	This function is called automatically when the user calls the pcap_setfilter().

	Parameters and return values are exactly the same of the pcap_setfilter().
*/
int pcap_setfilter_remote(pcap_t *fp, struct bpf_program *prog)
{
	if (!fp->rmt_capstarted)
	{
		// copy filter into the pcap_t structure
		if (install_bpf_program(fp, prog) == -1)
			return -1;
		return 0;
	}

	// we have to update a filter during run-time
	if (pcap_updatefilter_remote(fp, prog) )
		return -1;

	return 0;
}



/*!	\ingroup remote_pri_func
	\brief Suspends a pthread for msec milliseconds.

	This fucntion is provided since pthreads do not have a suspend() call.
*/
void pthread_suspend(int msec)
{
#ifdef WIN32
	Sleep(msec);
#else
struct timespec abstime;
struct timeval now;

	pthread_cond_t cond;
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&mutex, &attr);
	pthread_mutex_lock(&mutex);

	pthread_cond_init(&cond, NULL);

	gettimeofday(&now, NULL);
	
	abstime.tv_sec = now.tv_sec + msec/1000;
	abstime.tv_nsec = now.tv_usec * 1000 + (msec%1000) * 1000 * 1000;

	pthread_cond_timedwait(&cond, &mutex, &abstime);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
#endif
}



/*!	\ingroup remote_pri_func
	\brief Main function of the thread which waits for data packets (i.e.
	packets which carry a captured packet) in case of a TCP data connection.

	This function does basically the job of the Operating System kernel in the local
	capture: it takes packets and it puts them into the user buffer.

	\param ptr: it is a void pointer that will be casted into a pcap_t structure.
	This parameter is needed to retrieve the the socket (i.e. sockdata) we are currently 
	using for the data connection, the control socket (in case we want to send error messages),
	and so on.

	\return Nothing.
*/
void rpcap_thrdatamain_stream(void *ptr)
{
char errbuf[PCAP_ERRBUF_SIZE + 1];	// error buffer
pcap_t *fp;							// pointer to a 'pcap' structure
int retval;							// general variable used to keep the return value of other functions
struct rpcap_pkthdr net_pkt_header;	// header of the packet
struct pcap_pkthdr *pkt_header;		// pointer to the buffer that contains the header of the current packet
struct rpcap_header header;			// general header according to the RPCAP format
int cc;								// number of bytes of data available into the user buffer
unsigned int nread;					// number of bytes (of payload) currently read from the network (referred to the current pkt)
int pending= 0;						// '1' if we've already read the pkt header, but there was not enough space to read the pkt data

// Structures needed for the select() call
fd_set rfds;						// set of socket descriptors we have to check
struct timeval tv;					// maximum time the select() can block waiting for data

	fp= (pcap_t *) ptr;

	*errbuf= 0;	// Initialize errbuf

	// Modify thread params so that it can be killed at any time
	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) )
		goto error;
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) )
		goto error;

	// Watch out sockdata to see if it has input
	FD_ZERO(&rfds);

	// We do not have to block on the select()
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	/*
		The algorithm chosen to manage ths user buffer is quite simple, although
		not completely optimized. For instance, this is the same algorithm that
		is implemented when a packet is read from the kernel.

		The producer (i.e. this thread) reads data from the network only when the
		user has already consumed all the data into the buffer.
		When data is read, it is copied at the beginning of the buffer.
		So, this is not a circular buffer.
		This job ends when (1) there is no data coming from the network, (2)
		the user buffer is completely full.

		The consumer (i.e. the thread that calls pcap_next_ex) can read only when
		the producer finished its job. In other words, even if there is already 
		data into the user buffer, the consumer cannot read it because the producer
		updates the status variables (fp->cc, fp->bp) only when it ends its job.
		This is the reason we do not need spinlocks.

		This algorithm has two main drawbacks:
		- even if there is data into the user buffer, the application could not be 
		able to see it
		- the producer reads the data from the network all at once, so the resulting
		behaviour is that all of a sudden the socket buffer is emptied and the TCP
		starts sending several acknowledges at the same time. This could lead to
		bursty traffic patterns.
	*/
	while (1)
	{
		if (fp->cc == 0) 
		{
			u_char *bp= fp->buffer;
			cc= 0;

			/*
				We will exit from this cycle while when:
				- the socket does not have data in it
				- the buffer does not have space left

				In all cases, the pcap status variables will be updated
				before calling 'break'.
			*/
			while (1)
			{
again:
				// 'fp->rmt_sockdata' has always to be set before calling the select(),
				// since it is cleared by the select()
				FD_SET(fp->rmt_sockdata, &rfds);

				retval = select(fp->rmt_sockdata + 1, &rfds, NULL, NULL, &tv);
				if (retval == -1)
				{
					sock_geterror("select(): ", errbuf, PCAP_ERRBUF_SIZE);
						goto error;
				}

				// There is no data waiting
				if (retval == 0)
				{
					// Update the pcap_t pointers so that the user application can read from the buffer
					fp->bp= fp->buffer;
					fp->cc= cc;

					pthread_suspend(100);	// Suspend 100 ms
					break;
				}

				if (!pending)
				{
					// I'm going to read a new packet; so I reset the number of bytes (payload only) read
					nread= 0;

					if (sock_recv(fp->rmt_sockdata, (char *) &header, sizeof(struct rpcap_header), errbuf) == -1)
						goto error;

					// Checks if the message is correct
					retval= rpcap_checkmsg(errbuf, fp->rmt_sockdata, &header, RPCAP_MSG_PACKET, 0);

					if (retval != RPCAP_MSG_PACKET)		// the message is not the one expected
					{
						switch (retval)
						{
							case -3:		// Unrecoverable network error									
								goto error;	// Do nothing; just exit from here; the error code is already into the errbuf

							case -2:		// The other endpoint sent a message that is not allowed here
							case -1:		// The other endpoint has a version number that is not compatible with our
								goto again;

							default:
							{
								SOCK_ASSERT("Internal error", 1);
								goto error;
							};
						}
					}

					// Read the RPCAP packet header from the network
					if ( (nread= sock_recv(fp->rmt_sockdata, (char *) &net_pkt_header, sizeof(struct rpcap_pkthdr), errbuf)) == -1)
						goto error;
				}

				if ( (cc + ntohl(net_pkt_header.caplen) + sizeof(struct pcap_pkthdr) ) <= ( (unsigned) fp->bufsize) )
				{
					pending= 0;

					pkt_header= (struct pcap_pkthdr *) bp;
					pkt_header->caplen= ntohl(net_pkt_header.caplen);
					pkt_header->len= ntohl(net_pkt_header.len);
					pkt_header->ts.tv_sec= ntohl(net_pkt_header.timestamp_sec);
					pkt_header->ts.tv_usec= ntohl(net_pkt_header.timestamp_usec);

					// I don't update the counter of the packets dropped by the network since we're using TCP;
					// therefore no packets are dropped. Just update the number of packets received correctly
					fp->md.TotCapt++;

					// Copies the packet into the data buffer
					if ( (nread+= sock_recv(fp->rmt_sockdata, (char *) bp + sizeof(struct pcap_pkthdr), pkt_header->caplen, errbuf)) == -1)
						goto error;

					bp += BPF_WORDALIGN(pkt_header->caplen + sizeof(struct pcap_pkthdr));
					cc += BPF_WORDALIGN(pkt_header->caplen + sizeof(struct pcap_pkthdr));

					// Checks if all the data has been read; if not, discard the data in excess
					if (nread != ntohl(header.plen))
						sock_discard(fp->rmt_sockdata, ntohl(header.plen) - nread, fakeerrbuf);

				}
				else
				{
					pending= 1;

					// Update the pcap_t pointers so that the user application can read from the buffer
					fp->bp= fp->buffer;
					fp->cc= cc;

					break;
				}
			} // end while (1)
		}
		else
		{
			pthread_suspend(100);	// Suspend 100 ms
		}

	}	// end main while(1)

error:
	SOCK_ASSERT("Exiting from the child data thread", 1);
	SOCK_ASSERT(errbuf, 1);

	// Removes the descriptor s from set
	FD_CLR(fp->rmt_sockdata, &rfds);

 	closesocket(fp->rmt_sockdata);
	fp->rmt_sockdata= 0;
	fp->rmt_threaddata= 0;
}



/*!	\ingroup remote_pri_func
	\brief Main function of the thread which waits for data packets (i.e.
	packets which carry a captured packet) in case of a UDP data connection.

	This function does basically the job of the Operating System kernel in the local
	capture: it takes packet and it puts them into the user buffer.

	This function is different from the rpcap_thrdatamain_stream(), because UDP requires
	that a message coming from the network is received using a single recv() call.
	Vice versa, TCP sockets allows you reading only a few bytes each time, and the remaining part 
	of the message is retained.

	From this point of view, UDP sockets makes this thread more epensive, because the message is 
	first copied into a temporary buffer (allocated by the thread itself), then, if the 
	WinPcap/libpcap user buffer has enough space, the data is copied into that buffer.

	\param ptr: it is a void pointer that will be casted into a pcap_t structure.
	This parameter is needed to retrieve the the socket (i.e. sockdata) we are currently 
	using for the data connection, the control socket (in case we want to send error messages),
	and so on.

	\return Nothing.
*/
void rpcap_thrdatamain_dgram(void *ptr)
{
char errbuf[PCAP_ERRBUF_SIZE + 1];	// error buffer
char netbuf[RPCAP_NETBUF_SIZE];		// size of the network buffer in which the packet is copied
pcap_t *fp;							// pointer to a 'pcap' structure
int retval;							// general variable used to keep the return value of other functions
struct rpcap_header *header;		// general header according to the RPCAP format
struct rpcap_pkthdr *net_pkt_header;// header of the packet
struct pcap_pkthdr *pkt_header;		// pointer to the buffer that contains the header of the current packet
int cc;								// number of bytes of data available into the user buffer
int pending= 0;						// '1' if we've already read the pkt header, but there was not enough space to read the pkt data

// Structures needed for the select() call
fd_set rfds;						// set of socket descriptors we have to check
struct timeval tv;					// maximum time the select() can block waiting for data


	// See the rpcap_thrdatamain_stream() for any comment about the code
	fp= (pcap_t *) ptr;

	*errbuf= 0;	// Initialize errbuf

	// Modify thread params so that it can be killed at any time
	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) )
		goto error;
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) )
		goto error;

	// Watch out sockdata to see if it has input
	FD_ZERO(&rfds);

	// We do not have to block here
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	while (1)
	{
		if (fp->cc == 0) 
		{
			u_char *bp= fp->buffer;
			cc= 0;

			while (1)
			{
again:
				FD_SET(fp->rmt_sockdata, &rfds);

				retval = select(1, &rfds, NULL, NULL, &tv);
				if (retval == -1)
				{
					sock_geterror("select(): ", errbuf, PCAP_ERRBUF_SIZE);
						goto error;
				}

				// There is no data waiting
				if (retval == 0)
				{
					// Update the pcap_t pointers so that the user application can read from the buffer
					fp->bp= fp->buffer;
					fp->cc= cc;

					pthread_suspend(100);	// Suspend 100 ms
					break;
				}

				if (!pending)
				{
					// Read the entire message from the network
					if (sock_recv_dgram(fp->rmt_sockdata, netbuf, RPCAP_NETBUF_SIZE, errbuf) == -1)
						goto error;
					header= (struct rpcap_header *) netbuf;

					// Checks if the message is correct
					retval= rpcap_checkmsg(errbuf, fp->rmt_sockdata, (struct rpcap_header *) netbuf, RPCAP_MSG_PACKET, 0);

					if (retval != RPCAP_MSG_PACKET)		// the message is not the one expected
					{
						switch (retval)
						{
							case -3:		// Unrecoverable network error							
								goto error;	// Do nothing; just exit from here; the error code is already into the errbuf

							case -2:		// The other endpoint sent a message that is not allowed here
							case -1:	// The other endpoint has a version number that is not compatible with our
								goto again;

							default:
							{
								SOCK_ASSERT("Internal error", 0);
								goto error;
							};
						}
					}

					// Set a pointer to the RPCAP packet header
					net_pkt_header= (struct rpcap_pkthdr *) (netbuf + sizeof(struct rpcap_header));
				}

				if ( (cc + ntohl(net_pkt_header->caplen) + sizeof(struct pcap_pkthdr) ) <= ( (unsigned) fp->bufsize) )
				{
					unsigned int npkt;
					pending= 0;

					pkt_header= (struct pcap_pkthdr *) bp;
					pkt_header->caplen= ntohl(net_pkt_header->caplen);
					pkt_header->len= ntohl(net_pkt_header->len);
					pkt_header->ts.tv_sec= ntohl(net_pkt_header->timestamp_sec);
					pkt_header->ts.tv_usec= ntohl(net_pkt_header->timestamp_usec);

					// Update the number of packets received correctly
					fp->md.TotCapt++;

					// We're using UDP, so I need to update the counter of the packets dropped by the network
					npkt= ntohl(net_pkt_header->npkt);

					if (fp->md.TotCapt != npkt)
					{
						fp->md.TotNetDrops+= (npkt - fp->md.TotCapt);
						fp->md.TotCapt= npkt;
					}

					// Copies the packet into the data buffer
					memcpy( bp + sizeof(struct pcap_pkthdr), netbuf + sizeof(struct rpcap_header), pkt_header->caplen);

					bp += BPF_WORDALIGN(pkt_header->caplen + sizeof(struct pcap_pkthdr));
					cc += BPF_WORDALIGN(pkt_header->caplen + sizeof(struct pcap_pkthdr));
				}
				else
				{
					pending= 1;

					// Update the pcap_t pointers so that the user application can read from the buffer
					fp->bp= fp->buffer;
					fp->cc= cc;

					break;
				}
			} // end while (1)
		}
		else
		{
			pthread_suspend(100);	// Suspend 100 ms
		}
	}	// end main while(1)

error:
	SOCK_ASSERT("Exiting from the child data thread", 1);
	SOCK_ASSERT(errbuf, 1);

	// Removes the descriptor s from set
	FD_CLR(fp->rmt_sockdata, &rfds);

 	closesocket(fp->rmt_sockdata);
	fp->rmt_sockdata= 0;
	fp->rmt_threaddata= 0;
}






/*********************************************************
 *                                                       *
 * Miscellaneous functions                               *
 *                                                       *
 *********************************************************/


/*!	\ingroup remote_pri_func
	\brief It sends a RPCAP error to the other peer.

	This function has to be called when the main program detects an error. This funcion
	will send on the other peer the 'buffer' specified by the user.
	This function *does not* request a RPCAP CLOSE connection. A CLOSE command must be sent
	explicitely by the program, since we do not know it the error can be recovered in some
	way or it is a non-recoverable one.

	\param sock: the socket we are currently using.

	\param error: an user-allocated (and '0' termined) buffer that contains the error 
	description thas has to be transmitted on the other peer. The error message cannot
	be longer than PCAP_ERRBUF_SIZE.

	\param errcode: a integer which tells the other party the type of error we had;
	currently is is not too much used.

	\param errbuf: a pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one). It could be network problem.

	\return '0' if everything is fine, '-1' if some errors occurred. The error message is returned
	in the 'errbuf' variable.
*/
int rpcap_senderror(SOCKET sock, char *error, unsigned short errcode, char *errbuf)
{
char sendbuf[RPCAP_NETBUF_SIZE];			// temporary buffer in which data to be sent is buffered
int sendbufidx= 0;							// index which keeps the number of bytes currently buffered
uint16 length;

	length= (uint16) strlen(error);

	if (length > PCAP_ERRBUF_SIZE)
		length= PCAP_ERRBUF_SIZE;

	rpcap_createhdr( (struct rpcap_header *) sendbuf, RPCAP_MSG_ERROR, errcode, length);

	if ( sock_bufferize(NULL, sizeof(struct rpcap_header), NULL, &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errbuf) )
		return -1;

	if ( sock_bufferize(error, length, sendbuf, &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_BUFFERIZE, errbuf) )
		return -1;

	if ( sock_send(sock, sendbuf, sendbufidx, errbuf) )
		return -1;

	return 0;
}



/*!	\ingroup remote_pri_func
	\brief Sends the authentication message.

	It sends the authentication parameters on the control socket.
	Ths function is required in order to open the connection with the other end party.

	\param sock: the socket we are currently using.

	\param auth: authentication parameters that have to be sent.

	\param errbuf: a pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one). It could be network problem
	ot the fact that the authorization failed.

	\return '0' if everything is fine, '-1' if some errors occurred. The error message is returned
	in the 'errbuf' variable.
	The error message could be also 'the authentication failed'.
*/
int rpcap_sendauth(SOCKET sock, struct pcap_rmtauth *auth, char *errbuf)
{
char sendbuf[RPCAP_NETBUF_SIZE];	// temporary buffer in which data that has to be sent is buffered
int sendbufidx= 0;					// index which keeps the number of bytes currently buffered
uint16 length;						// length of the payload of this message
struct rpcap_auth *rpauth;
uint16 auth_type;
struct rpcap_header header;
int retval;							// temp variable which stores functions return value

	if (auth)
	{
	auth_type= auth->type;

		switch (auth->type)
		{
			case RPCAP_RMTAUTH_NULL:
				length= sizeof (struct rpcap_auth);
				break;

			case RPCAP_RMTAUTH_PWD:
				length= sizeof (struct rpcap_auth);
				if (auth->username) length+= strlen(auth->username);
				if (auth->password) length+= strlen(auth->password);
				break;

			default:
				snprintf(errbuf, PCAP_ERRBUF_SIZE, "Authentication type not recognized.");
				return -1;
		}
	}
	else
	{
		auth_type= RPCAP_RMTAUTH_NULL;
		length= sizeof (struct rpcap_auth);
	}


	if ( sock_bufferize(NULL, sizeof(struct rpcap_header), NULL, &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errbuf) )
		return -1;

	rpcap_createhdr( (struct rpcap_header *) sendbuf, RPCAP_MSG_AUTH_REQ, 0, length);

	rpauth= (struct rpcap_auth *) &sendbuf[sendbufidx];

	if ( sock_bufferize(NULL, sizeof(struct rpcap_auth), NULL, &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_CHECKONLY, errbuf) )
		return -1;

	memset(rpauth, 0, sizeof (struct rpcap_auth) );

	rpauth->type= htons(auth_type);

	if (auth_type == RPCAP_RMTAUTH_PWD)
	{

		if (auth->username)
			rpauth->slen1= strlen(auth->username);
		else
			rpauth->slen1= 0;

		if ( sock_bufferize(auth->username, rpauth->slen1, sendbuf, &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_BUFFERIZE, errbuf) )
			return -1;

		if (auth->password)
			rpauth->slen2= strlen(auth->password);
		else
			rpauth->slen2= 0;

		if ( sock_bufferize(auth->password, rpauth->slen2, sendbuf, &sendbufidx, RPCAP_NETBUF_SIZE, SOCKBUF_BUFFERIZE, errbuf) )
			return -1;

		rpauth->slen1= htons (rpauth->slen1);
		rpauth->slen2= htons (rpauth->slen2);
	}

	if ( sock_send(sock, sendbuf, sendbufidx, errbuf) )
		return -1;

	if ( sock_recv(sock, (char *) &header, sizeof(struct rpcap_header), errbuf) == -1)
		return -1;

	retval= rpcap_checkmsg(errbuf, sock, &header, RPCAP_MSG_AUTH_REPLY, RPCAP_MSG_ERROR, 0);

	if (retval != RPCAP_MSG_AUTH_REPLY)		// the message is not the one expected
	{
		switch (retval)
		{
			case -3:		// Unrecoverable network error
			case -2:		// The other endpoint sent a message that is not allowed here
			case -1:	// The other endpoint has a version number that is not compatible with our
				// Do nothing; just exit from here; the error code is already into the errbuf
				return -1;

			case RPCAP_MSG_ERROR:
			{
				return -1;
			};

			default:
			{
				SOCK_ASSERT("Internal error", 0);
				return -1;
			};
		}
	}

	if (ntohl(header.plen) )
	{
		if (sock_discard(sock, ntohl(header.plen), errbuf) )
			return -1;
	}
	
	return 0;
}



/*!	\ingroup remote_pri_func
	\brief Creates a structure of type rpcap_header.

	This function is provided just because the creation of an rpcap header is quite a common
	task. It accepts all the values that appears into an rpcap_header, and it puts them in
	place using the proper hton() calls.

	\param header: a pointer to a user-allocated buffer which will contain the serialized
	header, ready to be sent on the network.

	\param type: a value (in the host by order) which will be placed into the header.type
	field and that represents the type of the current message.

	\param value: a value (in the host by order) which will be placed into the header.value
	field and that has a message-dependent meaning.

	\param length: a value (in the host by order) which will be placed into the header.length
	field and that represents the payload length of the message.

	\return Nothing. The serialized header is returned into the 'header' variable.
*/
void rpcap_createhdr(struct rpcap_header *header, uint8 type, uint16 value, uint32 length)
{
	memset(header, 0, sizeof (struct rpcap_header) );

	header->ver= RPCAP_VERSION;
	header->type= type;
	header->value= htons(value);
	header->plen= htonl(length);
}



/*!	\ingroup remote_pri_func
	\brief Checks if the header of the received message is correct.
	
	This function is a way to easily check if the message received, in a certain 
	state of the RPCAP protocol Finite State Machine, is valid. This function accepts,
	as a parameter, the list of message types that are allowed in a certain situation,
	and it returns the one which occurs.

	\param errbuf: a pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one). It could be either problem
	occurred inside this function (e.g. a network problem in case it tries to send an
	error on the other peer and the send() call fails), an error message which has been 
	sent to us from the other party, or a version error (the message receive has a version 
	number that is incompabile with our).

	\param sock: the socket that has to be used to receive data. This function can
	read data from socket in case the version contained into the message is not compatible
	with our. In that case, all the message is purged from the socket, so that the following
	recv() calls will return a new message.

	\param header: a pointer to and 'rpcap_header' structure that keeps the data received from
	the network (still in network byte order) and that has to be checked.

	\param first: this function has a variable number of parameters. From this point on, 
	all the messages that are valid in this context must be passed as parameters.
	The message type list must be terminated with a '0' value, the null message type, 
	which means 'no more types to check'. The RPCAP protocol does not define anything with 
	message type equal to zero, so there is no ambiguity in using this value as a list terminator.

	\return The message type of the message that has been detected. In case of errors (e.g. the
	header contains a type that is not listed among the allowed types), this function will 
	return the following codes:
	- (-1) if the version is incompatible.
	- (-2) if the code is not among the one listed into the parameters list
	- (-3) if a network error (connection reset, ...)
	- RPCAP_MSG_ERROR if the message is an error message (it follow that the RPCAP_MSG_ERROR
	could not be present in the allowed message-types list, beucase this function checks
	for errors anyway)

	In case either the version is incompatible or nothing matches (i.e. it returns '-1' or '-2'),
	it discards the message body (i.e. it reads the remaining part of the message from the 
	network and it discards it) so that the application is ready to receive a new message.
*/
int rpcap_checkmsg(char *errbuf, SOCKET sock, struct rpcap_header *header, uint8 first, ...)
{
va_list ap;
uint8 type;
int32 len;

	va_start(ap, first);

	// Check if the present version of the protocol can handle this message
	if ( rpcap_checkver(sock, header, errbuf) )
	{
		SOCK_ASSERT(errbuf, 1);

		va_end(ap);
		return -1;
	}

	type= first;

	while ( type != 0 )
	{
		// The message matches with one of the types listed
		// There is no need of conversions since both values are uint8

		// Check if the other side reported an error.
		// If yes, it retrieves it and it returns it back to the caller
		if (header->type == RPCAP_MSG_ERROR)
		{
			len= ntohl(header->plen);

			if (len >= PCAP_ERRBUF_SIZE)
			{
				if (sock_recv(sock, errbuf, PCAP_ERRBUF_SIZE -1, errbuf) )
					return -3;
				sock_discard(sock, len - (PCAP_ERRBUF_SIZE - 1), fakeerrbuf);

				// Put '\0' at the end of the string
				errbuf[PCAP_ERRBUF_SIZE - 1]= 0;
			}
			else
			{
				if (sock_recv(sock, errbuf, len, errbuf) == -1)
					return -3;

				// Put '\0' at the end of the string
				errbuf[len]= 0;
			}


			va_end(ap);
			return header->type;
		}

		if (header->type == type)
		{
			va_end(ap);
			return header->type;
		}
		
		// get next argument
		type= va_arg(ap, int);
	}

	// we already have an error, so please discard this one
	sock_discard(sock, ntohl(header->plen), fakeerrbuf);

	snprintf(errbuf, PCAP_ERRBUF_SIZE, "The other endpoint sent a message that is not allowed here.");
	SOCK_ASSERT(errbuf, 1);

	va_end(ap);
	return -2;
}



/*!	\ingroup remote_pri_func
	\brief Checks if the version contained into the message is compatible with
	the one handled by this implementation.

	Right now, this function does not have any sophisticated task: if the versions
	are different, it returns -1 and it discards the message.
	It is expected that in the future this message will become more complex.

	\param sock: the socket that has to be used to receive data. This function can
	read data from socket in case the version contained into the message is not compatible
	with our. In that case, all the message is purged from the socket, so that the following
	recv() calls will return a new (clean) message.

	\param header: a pointer to and 'rpcap_header' structure that keeps the data received from
	the network (still in network byte order) and that has to be checked.

	\param errbuf: a pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one). The error message is
	"incompatible version".

	\return '0' if everything is fine, '-1' if some errors occurred. The error message is returned
	in the 'errbuf' variable.
*/
int rpcap_checkver(SOCKET sock, struct rpcap_header *header, char *errbuf)
{
	/*
	This is a sample function.

	In the real world, you have to check at the type code, 
	and decide accordingly.
	*/

	if (header->ver != RPCAP_VERSION)
	{
		snprintf(errbuf, PCAP_ERRBUF_SIZE, "Incompatible version number: message discarded.");

		// we already have an error, so please discard this one
		sock_discard(sock, ntohl(header->plen), fakeerrbuf);
		return -1;
	}

	return 0;
}



/*!	\ingroup remote_pri_func

	\brief It returns the socket currently used for this active connection (active mode only).

	This function is just for internal use; it returns the socket ID of the active connection 
	currently opened.

	\param host: a string that keeps the host name of the host for which we want to
	get the socket ID for that active connection.

	\param errbuf: a pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return the socket identifier if everything is fine, '0' if this host is not in the active 
	host list. It returns '-1' in case of error. The error message is returned into the errbuf variable.

	\warning Win32: be carefully not to assign the returning value of this call to a SOCKET
	directly. It should lead to wrong results, since Win32 sockets are unsigned int; therefore
	a negative value could not be handled correctly.
*/
int rpcap_remoteact_getsock(const char *host, char *errbuf)
{
struct activehosts *temp;					// temp var needed to scan the host list chain
struct addrinfo hints, *addrinfo, *ai_next;	// temp var needed to translate between hostname to its address
int retval;

	// retrieve the network address corresponding to 'host'
	addrinfo = NULL;
	memset(&hints, 0, sizeof (struct addrinfo) );
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype= SOCK_STREAM;

	retval = getaddrinfo(host, "0", &hints, &addrinfo);
	if (retval != 0)
	{
		snprintf(errbuf, PCAP_ERRBUF_SIZE, "getaddrinfo() %s", gai_strerror(retval));
		return -1;
	}

	temp= activeHosts;

	while (temp)
	{
		ai_next= addrinfo;
		while(ai_next)
		{
			if (sock_cmpaddr(&temp->host, (struct sockaddr_storage *) ai_next->ai_addr ) == 0)
				return (temp->sockctrl );

			ai_next= ai_next->ai_next;
		}
		temp= temp->next;
	}

	if (addrinfo)
		freeaddrinfo(addrinfo);

	// The host you want to get the socket ID does not have an active connection
	return 0;
}





