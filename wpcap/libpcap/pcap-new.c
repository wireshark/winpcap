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

#include <pcap-int.h>	// for the details of the pcap_t structure
#include <pcap-remote.h>
#include <sockutils.h>
#include <errno.h>	// for the errno variable
#include <stdlib.h>		// for malloc(), free(), ...
#include <string.h>		// for strstr, etc



/*
	\brief Global variable; needed to keep the message due to an error that we want to discard.
	
	This can happen, for instance, because we already have an error message and we want to keep 
	the first one.
*/
char fakeerrbuf[PCAP_ERRBUF_SIZE + 1];


//! Keeps a list of all the opened connections in the active mode.
extern struct activehosts *activeHosts;


/*!
	\brief Keeps the main socket identifier when we want to accept a new remote connection (active mode only).

	See the documentation of pcap_remoteact_accept() and pcap_remoteact_cleanup() for more details.
*/
SOCKET sockmain;






/****************************************************
 *                                                  *
 * Function bodies                                  *
 *                                                  *
 ****************************************************/


/*! \ingroup remote_func
	\brief It creates a list of network devices that can be opened with pcap_open().
	
	This function is a superset of the old 'pcap_findalldevs()', which is obsolete, and which
	allows listing only the devices present on the local machine.
	Vice versa, pcap_findalldevs_ex() allows listing the devices present on a remote 
	machine as well. Moreover, pcap_findalldevs_ex() is platform independent, since it
	relies on the standard pcap_findalldevs() to get addresses on the local machine.

	In case the function has to list the interfaces on a remote machine, it opens a new control
	connection toward that machine, it retrieves the interfaces, and it drops the connection.
	However, if this function detects that the remote machine is in 'active' mode,
	the connection is not dropped (see the 'sockctrl' parameters for more details).
	In the same way, if we're in active mode and the connection is already opened, it
	uses the existing socket.

	This function can rely on the pcap_createsrcstr() to create the string that keeps
	the capture device according to	the new syntax, and the pcap_parsesrcstr() for the
	other way round.

	\param host: a char* buffer that keeps the address of the remote host on which 
	we want to see the interface list.
	It can be NULL: in this case the function queries the local host for the locally
	installed interfaces. The address can be both numeric (e.g. '10.11.12.13', '1:2:3::4')
	and literal (e.g. 'foo.bar.com').

	\param auth: a pointer to a pcap_rmtauth structure. This pointer keeps the information
	required to authenticate the RPCAP connection to the remote host.
	This parameter is not meaningful in case of a query to the local host: in that case
	it can be NULL.

	\param sockctrl: Socket to be used for the control connection.
	This parameter is meaningful only if the control connection is already open when the 
	pcap_findalldevs() is called. This can be the case in which the 'ative' mode is used, 
	in which the capturing machine opens a control connection toward the client in order 
	to bypass in-middle firewalls. In that case, the control connection is already open, 
	and we have to use this one instead of opening a new one.

	In case this parameter is non-zero, the 'host' and 'port' parameters are meaningless:
	the software will always try to retrieve the network adapters using this control
	connection, despite the value assumed by 'host' and 'port'.
	In case this parameter is non-zero, the socket is not closed at the end of the function
	and it remains open for future use.

	\param port: a char* buffer (e.g. "2003") that keeps the network port on which we 
	want to connect to.
	It can be NULL: in this case the function uses the standard port, defined in 
	RPCAP_DEFAULT_NETPORT.

	\param alldevs: a 'struct pcap_if_t' pointer, which will be properly allocated inside
	this function. When the function returns, it is set to point to the first element 
	of the interface list; each element of the list is of type 'struct pcap_if_t'.

	\param errbuf: a pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return '0' if everything is fine, '-1' if some errors occurred. The list of the devices 
	is returned in the 'alldevs' variable.
	When the function returns correctly, 'alldevs' cannot be NULL. In other words, this 
	function returns '-1' also in case the system does not have any interface to list.

	The error message is returned in the 'errbuf' variable. An error could be due to 
	several reasons:
	- libpcap/WinPcap was not installed on the local/remote host
	- the user does not have enough privileges to list the devices
	- a network problem
	- the RPCAP version negotiation failed
	- other errors (not enough memory and others).
	
	\warning There may be network devices that cannot be opened with pcap_open() by the process
	calling pcap_findalldevs(), because, for example, that process might not have
	sufficient privileges to open them for capturing; if so, those devices will not 
	appear on the list.

	\warning The interface list must be deallocated manually by using the pcap_freealldevs().
*/
int pcap_findalldevs_ex(char *host, char *port, SOCKET sockctrl, struct pcap_rmtauth *auth, pcap_if_t **alldevs, char *errbuf) 
{
unsigned int nread= 0;		// number of bytes of the payload read from the socket
struct addrinfo hints;		// temp variable needed to resove hostnames into to socket representation
struct addrinfo *addrinfo;	// temp variable needed to resove hostnames into to socket representation
struct rpcap_header header;	// structure that keeps the general header of the rpcap protocol
int i,j;		// temp variables
int naddr;		// temp var needed to avoid problems with IPv6 addresses
int retval;		// store the return value of the functions
int nif;		// Number of interfaces listed
int active= 0;	// 'true' if we the other end-party is in active mode


	// We have to perform two controls here, because the user can call this function in two ways:
	//   pcap_findalldevs_ex(NULL, ...) ==> address == NULL
	//   pcap_findalldevs_ex("", ...) ==> address[0] == 0
	if ( (host == NULL) || (host[0] == 0) )
	{
		// The user wants to retrieve adapters from a local host
		if (pcap_findalldevs(alldevs, errbuf) == -1)
			return -1;
		if ( (alldevs == NULL) || (*alldevs == NULL) )
		{
			snprintf(errbuf, PCAP_ERRBUF_SIZE,
				"No interfaces found! Make sure libpcap/WinPcap is properly installed"
				" on the local machine.");
			return -1;
		}
		return 0;
	}

	// Warning: this call can be the first one called by the user.
	// For this reason, we have to initialize the WinSock support.
	if (sock_init(errbuf) == -1)
		return -1;

	// Check for active mode
	if ( (retval= rpcap_remoteact_getsock(host, errbuf)) == -1)
		return -1;

	if (retval)
	{
		sockctrl= retval;
		active= 1;
	}
	else	// we're not in active mode; let's opening a new control connection (if needed)
	{
		addrinfo= NULL;

		memset(&hints, 0, sizeof(struct addrinfo) );
		hints.ai_family = PF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		if ( (port == NULL) || (port[0] == 0) )
		{
			// the user chose not to specify the port
			if (sock_validaddr(host, RPCAP_DEFAULT_NETPORT, &hints, &addrinfo, errbuf) == -1)
				return -1;
		}
		else
		{
			if (sock_validaddr(host, port, &hints, &addrinfo, errbuf) == -1)
				return -1;
		}

		if ( (sockctrl= sock_open(addrinfo, SOCKOPEN_CLIENT, 0, errbuf)) == -1)
			goto error;

		// addrinfo is no longer used
		freeaddrinfo(addrinfo);
		addrinfo= NULL;

		if ( rpcap_sendauth(sockctrl, auth, errbuf) == -1)
		{
			// Control connection has to be closed only in case the remote machine is in passive mode
			if (!active)
				sock_close(sockctrl, fakeerrbuf);
			return -1;
		}
	}

	// RPCAP findalldevs command
	rpcap_createhdr(&header, RPCAP_MSG_FINDALLIF_REQ, 0, 0);

	if ( sock_send(sockctrl, (char *) &header, sizeof(struct rpcap_header), errbuf) == -1 )
		goto error;

	if ( sock_recv(sockctrl, (char *) &header, sizeof(struct rpcap_header), errbuf) == -1)
		goto error;

	// Checks if the message is correct
	retval= rpcap_checkmsg(errbuf, sockctrl, &header, RPCAP_MSG_FINDALLIF_REPLY, RPCAP_MSG_ERROR, 0);

	if (retval != RPCAP_MSG_FINDALLIF_REPLY)		// the message is not the one expected
	{
		switch (retval)
		{
			case -3:	// Unrecoverable network error
			case -2:	// The other endpoint send a message that is not allowed here
			case -1:	// The other endpoint has a version number that is not compatible with our
				break;

			case RPCAP_MSG_ERROR:		// The other endpoint reported an error
				break;

			default:
			{
				snprintf(errbuf, PCAP_ERRBUF_SIZE, "Internal error");
				break;
			};
		}

		if (!active)
			sock_close(sockctrl, fakeerrbuf);

		return -1;
	}

	// read the number of interfaces
	nif= ntohs(header.value);

	// loop until all interfaces have been received
	for (i= 0; i < nif; i++)
	{
	struct rpcap_findalldevs_if findalldevs_if;
	pcap_if_t *dev;		// Previous device into the pcap_if_t chain

		// receive the findalldevs structure from remote hsot
		if ( (nread+= sock_recv(sockctrl, (char *) &findalldevs_if, 
			sizeof(struct rpcap_findalldevs_if), errbuf) ) == -1)
			goto error;

		findalldevs_if.namelen= ntohs(findalldevs_if.namelen);
		findalldevs_if.desclen= ntohs(findalldevs_if.desclen);
		findalldevs_if.naddr= ntohs(findalldevs_if.naddr);

		// allocate the main structure
		if (i == 0)
		{
			(*alldevs)= (pcap_if_t *) malloc(sizeof(pcap_if_t) );
			dev= (*alldevs);
		}
		else
		{
			dev->next= (pcap_if_t *) malloc(sizeof(pcap_if_t) );
			dev= dev->next;
		}

		// check that the malloc() didn't fail
		if (dev == NULL)
		{
			snprintf(errbuf, PCAP_ERRBUF_SIZE, "malloc() failed: %s", pcap_strerror(errno));
			goto error;
		}
		dev->next= NULL;
		dev->addresses= NULL;

		// allocate mem for name and description
		if (findalldevs_if.namelen)
		{
			dev->name= (char *) malloc(findalldevs_if.namelen + 1);
			if (dev->name == NULL)
			{
				snprintf(errbuf, PCAP_ERRBUF_SIZE, "malloc() failed: %s", pcap_strerror(errno));
				goto error;
			}
			// Retrieve adapter name and description
			if ( (nread+= sock_recv(sockctrl, dev->name, findalldevs_if.namelen, errbuf) ) == -1)
				goto error;
			dev->name[findalldevs_if.namelen]= 0;
		}
		else
			dev->name= NULL;

		if (findalldevs_if.desclen)
		{
			dev->description= (char *) malloc(findalldevs_if.desclen + 1);
			if (dev->description == NULL)
			{
				snprintf(errbuf, PCAP_ERRBUF_SIZE, "malloc() failed: %s", pcap_strerror(errno));
				goto error;
			}
			// Retrieve adapter name and description
			if ( (nread+= sock_recv(sockctrl, dev->description, findalldevs_if.desclen, errbuf) ) == -1)
				goto error;
			dev->description[findalldevs_if.desclen]= 0;
		}
		else
			dev->description= NULL;

		dev->flags= ntohl(findalldevs_if.flags);

		naddr= 0;
		// loop until all addresses have been received
		for (j= 0; j < findalldevs_if.naddr; j++)
		{
		struct rpcap_findalldevs_ifaddr ifaddr;

			// Retrieve the interface addresses
			if (  (nread+= sock_recv(sockctrl, (char *) &ifaddr, 
				sizeof(struct rpcap_findalldevs_ifaddr), errbuf) ) == -1)
				goto error;

			// WARNING libpcap bug: the address listing is available only for AF_INET
			if ( ntohs(ifaddr.addr.ss_family) == AF_INET)
			{
			struct pcap_addr *addr;

				if (naddr == 0)
				{
					dev->addresses= (struct pcap_addr *) malloc ( sizeof(struct pcap_addr) );
					addr= dev->addresses;
				}
				else
				{
					addr->next= (struct pcap_addr *) malloc ( sizeof(struct pcap_addr) );
					addr= addr->next;
				}
				naddr++;

				if (addr == NULL)
				{
					snprintf(errbuf, PCAP_ERRBUF_SIZE, "malloc() failed: %s", pcap_strerror(errno));
					goto error;
				}
				addr->next= NULL;

				if (rpcap_deseraddr( (struct sockaddr_storage *) &ifaddr.addr, 
					(struct sockaddr_storage **) &addr->addr, errbuf) == -1)
					goto error;
				if (rpcap_deseraddr( (struct sockaddr_storage *) &ifaddr.netmask, 
					(struct sockaddr_storage **) &addr->netmask, errbuf) == -1)
					goto error;
				if (rpcap_deseraddr( (struct sockaddr_storage *) &ifaddr.broadaddr, 
					(struct sockaddr_storage **) &addr->broadaddr, errbuf) == -1)
					goto error;
				if (rpcap_deseraddr( (struct sockaddr_storage *) &ifaddr.dstaddr, 
					(struct sockaddr_storage **) &addr->dstaddr, errbuf) == -1)
					goto error;

				if ( (addr->addr == NULL) && (addr->netmask == NULL) && 
					(addr->broadaddr == NULL) && (addr->dstaddr == NULL) )
				{
					free(addr);
					addr= NULL;
					if (naddr == 1)
						naddr= 0;	// the first item of the list had NULL addresses
				}
			}
		}
	}

	// Checks if all the data has been read; if not, discard the data in excess
	if (nread != ntohl(header.plen))
	{
		if (sock_discard(sockctrl, ntohl(header.plen) - nread, errbuf) == 1)
			return -1;
	}

	// Control connection has to be closed only in case the remote machine is in passive mode
	if (!active)
	{
		// DO not send RPCAP_CLOSE, since we did not open a pcap_t; no need to free resources
		if ( sock_close(sockctrl, errbuf) )
			return -1;
	}

	// To avoid inconsistencies in the number of sock_init()
	sock_cleanup();

	return 0;

error:
	// In case there has been an error, I don't want to overwrite it with a new one
	// if the following call fails. I want to return always the original error.
	//
	// Take care: this connection can already be closed when we try to close it.
	// This happens because a previous error in the rpcapd, which requested to
	// closed the connection. In that case, we already recognized that into the
	// rpspck_isheaderok() and we already acknowledged the closing.
	// In that sense, this call is useless here (however it is needed in case
	// the client generates the error).

	// Checks if all the data has been read; if not, discard the data in excess
	if (nread != ntohl(header.plen))
	{
		if (sock_discard(sockctrl, ntohl(header.plen) - nread, fakeerrbuf) == 1)
			return -1;
	}

	// Control connection has to be closed only in case the remote machine is in passive mode
	if (!active)
		sock_close(sockctrl, fakeerrbuf);

	// To avoid inconsistencies in the number of sock_init()
	sock_cleanup();

	return -1;
}




/*! \ingroup remote_func
	\brief Accepts a set of strings (host name, port, ...), and it returns the complete 
	source string according to the new format (e.g. 'rpcap://1.2.3.4/eth0').

	This function is provided in order to help the user to create the source string
	according to the new format.
	An unique source string is used in order to make easy for old applications to use the
	remote facilities. Think about tcpdump, for example, which has only one way to specify
	the interface on which the capture has to be started.
	However, GUI-based programs can find more useful to specify hostname, port and
	interface name separately. In that case, they can use this function to create the 
	source string before passing it to the pcap_open() function.

	\param source: a user-allocated buffer that will contain the complete source string
	wen the function returns.
	This function assumes that the allocated buffer is at least PCAP_BUF_SIZE bytes.

	\param type: its value tells the type of the source we want to create. It can assume 
	the following values:
	- PCAP_SRC_FILE: if we want a local file
	- PCAP_SRC_IFLOCAL: if we want a local interface
	- PCAP_SRC_IFREMOTE: if we want a remote interface

	\param host: an user-allocated buffer that keeps the host (e.g. "foo.bar.com") we 
	want to connect to.
	It can be NULL in case we want to open an interface on a local host.

	\param port: an user-allocated buffer that keeps the network port (e.g. "2002") we 
	want to use for the RPCAP protocol.
	It can be NULL in case we want to open an interface on a local host.

	\param name: an user-allocated buffer that keeps the interface name we want to use
	(e.g. "eth0").

	\param errbuf: a pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return '0' if everything is fine, '-1' if some errors occurred. The string containing
	the complete source is returned in the 'source' variable.

	\warning If the source is longer than PCAP_BUF_SIZE, the excess characters are truncated.
*/
int pcap_createsrcstr(char *source, int type, const char *host, const char *port, const char *name, char *errbuf)
{
	switch (type)
	{
		case PCAP_SRC_FILE:
		{
			strncpy(source, PCAP_SRC_FILE_KEY, PCAP_BUF_SIZE);
			if ((name) && (*name) )
			{
				strncat(source, name, PCAP_BUF_SIZE);
				return 0;
			}
			else
			{
				snprintf(errbuf, PCAP_ERRBUF_SIZE, "The file name cannot be NULL.");
				return -1;
			}
		}
		case PCAP_SRC_IFREMOTE:
		{
			strncpy(source, PCAP_SRC_IF_KEY, PCAP_BUF_SIZE);
			if ((host) && (*host) )
			{
				if ( (strcspn(host, "aAbBcCdDeEfFgGhHjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ")) == strlen(host) )
				{
					// the host name does not contains alphabetic chars. So, it is a numeric address
					// In this case we have to include it between square brackets
					strncat(source, "[", PCAP_BUF_SIZE);
					strncat(source, host, PCAP_BUF_SIZE);
					strncat(source, "]", PCAP_BUF_SIZE);
				}
				else
					strncat(source, host, PCAP_BUF_SIZE);

				if ((port) && (*port) )
				{
					strncat(source, ":", PCAP_BUF_SIZE);
					strncat(source, port, PCAP_BUF_SIZE);
				}

				strncat(source, "/", PCAP_BUF_SIZE);
			}
			else
			{
				snprintf(errbuf, PCAP_ERRBUF_SIZE, "The host name cannot be NULL.");
				return -1;
			}

			if ((name) && (*name) )
			{
				strncat(source, name, PCAP_BUF_SIZE);
				return 0;
			}
			else
			{
				snprintf(errbuf, PCAP_ERRBUF_SIZE, "The interface name cannot be NULL.");
				return -1;
			}
		}
		case PCAP_SRC_IFLOCAL:
		{
			strncpy(source, PCAP_SRC_IF_KEY, PCAP_BUF_SIZE);
			if ((name) && (*name) )
			{
				strncat(source, name, PCAP_BUF_SIZE);
				return 0;
			}
			else
			{
				snprintf(errbuf, PCAP_ERRBUF_SIZE, "The interface name cannot be NULL.");
				return -1;
			}
		}
		default:
		{
			snprintf(errbuf, PCAP_ERRBUF_SIZE, "The interface type is not valid.");
			return -1;
		}
	}
}




/*! \ingroup remote_func
	\brief Parses the source string and returns the pieces in which the source can be split.

	This call is the other way round of pcap_createsrcstr().
	It accepts a null-terminated string and it returns the parameters related 
	to the source. This includes:
	- the type of the source (file, winpcap on a remote adapter, winpcap on local adapter),
	which is determined by the source prefix (PCAP_SRC_IF_KEY and so on)
	- the host on which the capture has to be started (only for remote captures)
	- the 'raw' name of the source (file name, name of the remote adapter, name 
	of the local adapter), without the source prefix. The string returned does not 
	include the type of the source itself (i.e. the string returned does not include "file://" 
	or rpcap:// or such).

	The user can omit some parameters in case it is not interested in them.

	\param source: a null-terminated string containing the WinPcap source

	\param type: pointer to an integer, which is used to return the code corrisponding to the 
	selected source. The code will be one of the following:
		- PCAP_SRC_FILE
		- PCAP_SRC_IFLOCAL
		- PCAP_SRC_IFREMOTE
	In case the source string does not exists (i.e. 'source == NULL') or it is empty
	('*source == NULL'), it returns PCAP_SRC_IF_LOCAL (i.e. you are ready to 
	call pcap_open_live() ). This behavior is kept only for compatibility with older 
	applications (e.g. tcpdump); therefore we suggest to move to the new syntax for sources.

	This parameter can be NULL in case the user is not interested in that.

	\param host: user-allocated buffer (of size PCAP_BUF_SIZE) that is used to return 
	the host name on which the capture has to be started.
	This value is meaningful only in case of remote capture; otherwise, the returned 
	string will be empty ("").
	This parameter can be NULL in case the user is not interested in that.

	\param port: user-allocated buffer (of size PCAP_BUF_SIZE) that is used to return 
	the port that has to be used by the RPCAP protocol to contact the other host.
	This value is meaningful only in case of remote capture and if the user wants to use
	a non-standard port; otherwise, the returned string will be empty ("").
	In case of remote capture, an emply string means "use the standard RPCAP port".
	This parameter can be NULL in case the user is not interested in that.

	\param name: user-allocated buffer (of size PCAP_BUF_SIZE) that is used to return 
	the source name, without the source prefix.
	If the name does not exist (for example because source contains 'rpcap://' that means 
	'default local adapter'), it returns NULL.
	This parameter can be NULL in case the user is not interested in that.

	\param errbuf: pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one).
	This parameter can be NULL in case the user is not interested in that.

	\return '0' if everything is fine, '-1' if some errors occurred. The requested values
	(host name, network port, type of the source) are returned into the proper variables
	passed by reference.
*/
int pcap_parsesrcstr(const char *source, int *type, char *host, char *port, char *name, char *errbuf)
{
char *ptr;
int ntoken;
char tmpname[PCAP_BUF_SIZE];
char tmphost[PCAP_BUF_SIZE];
char tmpport[PCAP_BUF_SIZE];
int tmptype;

	// Initialization stuff
	tmpname[0]= 0;
	tmphost[0]= 0;
	tmpport[0]= 0;

	if (host)
		*host= 0;
	if (port) 
		*port= 0;
	if (name)
		*name= 0;

	// Look for a 'rpcap://' identifier
	if ( (ptr= strstr(source, PCAP_SRC_IF_KEY)) != NULL)
	{
		ptr+= strlen(PCAP_SRC_IF_KEY);

		if (strchr(ptr, '[')) // This is probably a numeric address
		{
			ntoken= sscanf(ptr, "[%[1234567890:.]]:%[^/]/%s", tmphost, tmpport, tmpname);

			if (ntoken == 1)	// probably the port is missing
				ntoken= sscanf(ptr, "[%[1234567890:.]]/%s", tmphost, tmpname);

			tmptype= PCAP_SRC_IFREMOTE;
		}
		else
		{
			ntoken= sscanf(ptr, "%[^/:]:%[^/]/%s", tmphost, tmpport, tmpname);

			if (ntoken == 1)
			{
				// This can be due to two reasons:
				// - we want a remote capture, but the network port is missing
				// - we want to do a local capture
				// To distinguish between the two, we look for the '/' char
				if (strchr(ptr, '/'))
				{
					// We're on a remote capture
					sscanf(ptr, "%[^/]/%s", tmphost, tmpname);
					tmptype= PCAP_SRC_IFREMOTE;
				}
				else
				{
					// We're on a local capture
					if (*ptr)
						strncpy(tmpname, ptr, PCAP_BUF_SIZE);

					// Clean the host name, since it is a remote capture
					// NOTE: the host name has been assigned in the previous "ntoken= sscanf(...)" line
					tmphost[0]= 0;

					tmptype= PCAP_SRC_IFLOCAL;
				}
			}
			else
				tmptype= PCAP_SRC_IFREMOTE;
		}

		if (tmpname[0])
		{
			if (host)
				strcpy(host, tmphost);
			if (port) 
				strcpy(port, tmpport);
			if (name)
				strcpy(name, tmpname);
			if (type)
				*type= tmptype;

			return 0;
		}
		else
		{
			if (errbuf)
				snprintf(errbuf, PCAP_ERRBUF_SIZE, "The interface name has not been specified.");

			return -1;
		}
	}

	// Look for a 'file://' identifier
	if ( (ptr= strstr(source, PCAP_SRC_FILE_KEY)) != NULL)
	{
		ptr+= strlen(PCAP_SRC_FILE_KEY);
		if (*ptr)
		{
			if (name)
				strncpy(name, ptr, PCAP_BUF_SIZE);

			if (type)
				*type= PCAP_SRC_FILE;

			return 0;
		}
		else
		{
			if (errbuf)
				snprintf(errbuf, PCAP_ERRBUF_SIZE, "The file name has not been specified.");

			return -1;
		}

	}

	// Backward compatibility; the user didn't use the 'rpcap://, file://'  specifiers
	if ( (source) && (*source) )
	{
		if (name)
			strncpy(name, source, PCAP_BUF_SIZE);

		if (type)
			*type= PCAP_SRC_IFLOCAL;

		return 0;
	}
	else
	{
		if (errbuf)
			snprintf(errbuf, PCAP_ERRBUF_SIZE, "The interface name has not been specified.");

		return -1;
	}
};



/*!	\ingroup remote_func

	\brief It opens a generic source in order to capture / send (WinPcap only) traffic.
	
	The pcap_open() replaces all the pcap_open_xxx() functions with a single call.

	This function hides the differences between the different pcap_open_xxx() functions
	so that the programmer does not have to manage different opening function.
	In this way, the 'true' open function is decided according to the source type,
	which is included into the source string (in the form of source prefix).

	This function can rely on the pcap_createsrcstr() to create the string that keeps
	the capture device according to	the new syntax, and the pcap_parsesrcstr() for the
	other way round.

	\param source: zero-terminated string containing the source name to open.
	The source name has to include the format prefix according to the 
	syntax proposed by WinPcap. It cannot be NULL.
	On on Linux systems with 2.2 or later kernels, a device argument of "any"
	 (i.e. rpcap://any) can be used to capture packets from all interfaces.
	 <br>
	In case the pcap_createsrcstr() is not used, remember that the new source 
	syntax allows for these formats to be used in the pcap_open():
	- file://filename [we want to open a local file]
	- rpcap://host.foo.bar/adaptername [everything literal, no port number]
	- rpcap://host.foo.bar:1234/adaptername [everything literal, with port number]
	- rpcap://10.11.12.13/adaptername [IPv4 numeric, no port number]
	- rpcap://10.11.12.13:1234/adaptername [IPv4 numeric, with port number]
	- rpcap://[10.11.12.13]:1234/adaptername [IPv4 numeric with IPv6 format, with port number]
	- rpcap://[1:2:3::4]/adaptername [IPv6 numeric, no port number]
	- rpcap://[1:2:3::4]:1234/adaptername [IPv6 numeric, with port number]
	- rpcap://adaptername [local adapter, opened without using the RPCAP protocol]
	- adaptername [to open a local adapter; kept for compability, but it is strongly discouraged]
	- (NULL) [to open the first local adapter; kept for compability, but it is strongly discouraged]
	
	The following formats are not allowed:
	- rpcap:// [to open the first local adapter]
	- rpcap://hostname/ [to open the first remote adapter]

	\param snaplen: length of the packet that has to be retained.	
	For each packet received by the filter, only the first 'snaplen' bytes are stored 
	in the buffer and passed to the user application. For instance, snaplen equal to 
	100 means that only the first 100 bytes of each packet are stored.

  	\param flags: keeps several flags that can be needed for capturing packets.
	The allowed flags are the following:
	- PCAP_OPENFLAG_PROMISCUOUS: if the adapter has to go in promiscuous mode.		
	It is '1' if you have to open the adapter in promiscuous mode, '0' otherwise.
	Note that even if this parameter is false, the interface could well be in promiscuous
	mode for some other reason (for example because another capture process with 
	promiscuous mode enabled is currently using that interface).
	On on Linux systems with 2.2 or later kernels (that have the "any" device), this
	flag does not work on the "any" device; if an argument of "any" is supplied,
	the 'promisc' flag is ignored.
	- PCAP_OPENFLAG_SERVEROPEN_DP: it specifies who is responsible for opening the data
	connection in case of a remote capture (it means 'server open data path').
	If it is '1', it specifies if the data connection has to be intitiated 
	by the capturing device (which becomes like 'active'). If '0', the connection 
	will be initiated by the client workstation.
	This flag is used to overcome the problem of firewalls, which allow
	only outgoing connections. In that case, the capturing device can open
	a connection toward the client workstation in order to allow the
	data trasfer.
	In fact, the data connection is opened using a random port (while the
	control connection uses a standard port), so it is hard to configure
	a firewall to permit traffic on the data path.
	This flag is meaningless if the source is not a remote interface.
	Addictionally, it is meaningless if the data connection is done using
	the UDP protocol, since in this case the connection wil always be opened
	by the server.
	In these cases, it is simply ignored.
	- PCAP_OPENFLAG_UDP_DP: it specifies if the data trasfer (in case of a remote
	capture) has to be done with UDP protocol.
	If it is '1' if you want a UDP data connection, '0' if you want
	a TCP data connection; control connection is always TCP-based.
	A UDP connection is much lighter, but it does not guarantee that all
	the captured packets arrive to the client workstation. Moreover, 
	it could be harmful in case of network congestion.
	This flag is meaningless if the source is not a remote interface.
	In that case, it is simply ignored.

	\param read_timeout: read timeout in milliseconds.
	The read timeout is used to arrange that the read not necessarily return
	immediately when a packet is seen, but that it waits for some amount of 
	time to allow more packets to arrive and to read multiple packets from 
	the OS kernel in one operation. Not all platforms support a read timeout;
	on platforms that don't, the read timeout is ignored.

	\param auth: a pointer to a 'struct pcap_rmtauth' that keeps the information required to
	authenticate the user on a remote machine. In case this is not a remote capture, this
	pointer can be set to NULL.

	\param errbuf: a pointer to a user-allocated buffer which will contain the error
	in case this function fails. The pcap_open() and findalldevs() are the only two
	functions which have this parameter, since they do not have (yet) a pointer to a
	pcap_t structure, which reserves space for the error string. Since these functions
	do not have (yet) a pcap_t pointer (the pcap_t pointer is NULL in case of errors),
	they need an explicit 'errbuf' variable.
	'errbuf' may also be set to warning text when pcap_open_live() succeds; 
	to detect this case the caller should store a  zero-length string in  
	'errbuf' before calling pcap_open_live() and display the warning to the user 
	if 'errbuf' is no longer a zero-length string.

	\return A pointer to a 'pcap_t' which can be used as a parameter to the following
	calls (pcap_compile() and so on) and that specifies an opened WinPcap session. In case of 
	problems, it returns NULL and the 'errbuf' variable keeps the error message.

	\warning The source cannot be larger than PCAP_BUF_SIZE.
*/
pcap_t *pcap_open(const char *source, int snaplen, int flags, int read_timeout, struct pcap_rmtauth *auth, char *errbuf)
{
char host[PCAP_BUF_SIZE], port[PCAP_BUF_SIZE], name[PCAP_BUF_SIZE];
int type;
pcap_t *fp;

	if (strlen(source) > PCAP_BUF_SIZE)
	{
		snprintf(errbuf, PCAP_ERRBUF_SIZE, "The source string is too long. Cannot handle it correctly.");
		return NULL;
	}

	// determine the type of the source (NULL, file, local, remote)
	if (pcap_parsesrcstr(source, &type, host, port, name, errbuf) == -1)
		return NULL;


	switch (type) 
	{
		case PCAP_SRC_FILE:
			fp = pcap_open_offline(name, NULL);
			break;

		case PCAP_SRC_IFREMOTE:
			// Although we already have host, port and iface, we prefer TO PASS only 'pars' to the 
			// pcap_open_remote() so that it has to call the pcap_parsesrcstr() again.
			// This is less optimized, but much clearer.
			fp= pcap_opensource_remote(source, auth, errbuf);

			if (fp == NULL)
				return NULL;

			fp->snapshot= snaplen;
#ifdef WIN32
			fp->timeout= read_timeout;
#else
			fp->md.timeout= read_timeout;
#endif
			fp->rmt_flags= flags;
			break;

		case PCAP_SRC_IFLOCAL:
			fp= pcap_open_live(name, snaplen, (flags & PCAP_OPENFLAG_PROMISCUOUS), read_timeout, errbuf);
			break;

		default:
			strcpy(errbuf, "Source type not supported");
			return NULL;
	}
	return fp;
}



/*!	\ingroup remote_func

	\brief It blocks until a network connection is accepted (active mode only).

	This function has been defined to allow the client dealing with the 'active mode'.
	In other words, in the 'active mode' the server opens the connection toward the
	client, so that the client has to open a socket in order to wait for connections.
	When a new connection is accepted, the RPCAP protocol starts as usual; the only 
	difference is that the connection is initiated by the server.

	This function accepts only ONE connection, then it closes the waiting socket. This means
	that if some error occurs, the application has to call it again in order to accept another
	connection.

	This function returns when a new connection (coming from a valid host 'connectinghost')
	is accepted; it returns error otherwise.

	\param address: a string that keeps the network address we have to bind to; 
	usually it is NULL (it means 'bind on all local addresses').

	\param port: a string that keeps the network port on which we have to bind to; usually
	it is NULL (it means 'bind on the predefined port', i.e. RPCAP_DEFAULT_NETPORT_ACTIVE).

	\param hostlist: a string that keeps the host name of the host from whom we are
	expecting a connection; it can be NULL (it means 'accept connection from everyone').
	Host names are separated by a whatever character in the RPCAP_HOSTLIST_SEP list.

	\param connectinghost: a user-allocated buffer that will contain the name of the host
	is trying to connect to us.
	This variable must be at least RPCAP_HOSTLIST_SIZE bytes..

	\param auth: a pointer to a pcap_rmtauth structure. This pointer keeps the information
	required to authenticate the RPCAP connection to the remote host.

	\param errbuf: a pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return The SOCKET identifier of the new control connection if everything is fine,
	a negative number if some errors occurred. The error message is returned into the errbuf variable.
	In case it returns '-1', this means 'everything is fine', but the host cannot be admitted.
	In case it returns '-2', in means 'unrecoverable error' (for example it is not able to bind the 
	socket, or something like that).
	In case it returns '-3', it means 'authentication failed'. The authentication check is performed
	only if the connecting host is among the ones that are allowed to connect to this host.

	The host that is connecting to us is returned into the hostlist variable, which ust be allocated
	by the user. This variable contains the host name both in case the host is allowed, 
	and in case the connection is refused.

	\warning Although this function returns the socket established by the new control connection,
	this value should not be used. This value will be stored into some libpcap internal
	variables and it will be managed automatically by the library. In other words, all the
	following calls to findalldevs() and pcap_open() will check if the host is among one that
	already has a control connection in place; if so, that one will be used.

	\warning This function has several problems if used inside a thread, which is stopped
	when this call is blocked into the accept(). In this case, the socket on which we accept
	connections is not freed (thread termination is a very dirty job), so that we are no
	longer able to accept other connections until the program (i.e. the process) stops.
	In order to solve the problem, call the pcap_remoteact_cleanup().
*/
int pcap_remoteact_accept(const char *address, const char *port, const char *hostlist, char *connectinghost, struct pcap_rmtauth *auth, char *errbuf)
{
// socket-related variables
struct addrinfo hints;			// temporary struct to keep settings needed to open the new socket
struct addrinfo *addrinfo;		// keeps the addrinfo chain; required to open a new socket
struct sockaddr_storage from;	// generic sockaddr_storage variable
socklen_t fromlen;				// keeps the length of the sockaddr_storage variable
SOCKET sockctrl;				// keeps the main socket identifier
struct activehosts *temp, *prev;	// temp var needed to scan he host list chain

	*connectinghost= 0;		// just in case

	// Prepare to open a new server socket
	memset(&hints, 0, sizeof(struct addrinfo));
									// WARNING Currently it supports only ONE socket family among ipv4 and IPv6 
	hints.ai_family = AF_INET;		// PF_UNSPEC to have both IPv4 and IPv6 server
	hints.ai_flags = AI_PASSIVE;	// Ready to a bind() socket
	hints.ai_socktype = SOCK_STREAM;

	// Warning: this call can be the first one called by the user.
	// For this reason, we have to initialize the WinSock support.
	if (sock_init(errbuf) == -1)
		return -1;

	// Do the work
	if ((port == NULL) || (port[0] == 0) )
	{	
		if (sock_validaddr(address, RPCAP_DEFAULT_NETPORT_ACTIVE, &hints, &addrinfo, errbuf) == -1)
		{
			SOCK_ASSERT(errbuf, 1);
			return -2;
		}
	}
	else
	{
		if (sock_validaddr(address, port, &hints, &addrinfo, errbuf) == -1)
		{
			SOCK_ASSERT(errbuf, 1);
			return -2;
		}
	}


	if ( (sockmain= sock_open(addrinfo, SOCKOPEN_SERVER, 1, errbuf)) == -1)
	{
		SOCK_ASSERT(errbuf, 1);
		return -2;
	}

	// Connection creation
	fromlen = sizeof(struct sockaddr_storage);

	sockctrl= accept(sockmain, (struct sockaddr *) &from, &fromlen);

	// We're not using sock_close, since we do not want to send a shutdown
	// (which is not allowed on a non-connected socket)
	closesocket(sockmain);
	sockmain= 0;

	if (sockctrl == -1)
	{
		sock_geterror("accept(): ", errbuf, PCAP_ERRBUF_SIZE);
		return -2;
	}

	// Get the numeric for of the name of the connecting host
	if (getnameinfo( (struct sockaddr *) &from, fromlen, connectinghost, 
			RPCAP_HOSTLIST_SIZE, NULL, 0, NI_NUMERICHOST) )
	{
		sock_geterror("getnameinfo(): ", errbuf, PCAP_ERRBUF_SIZE);
		rpcap_senderror(sockctrl, errbuf, PCAP_ERR_REMOTEACCEPT, fakeerrbuf);
		sock_close(sockctrl, fakeerrbuf);
		return -1;
	}

	// checks if the connecting host is among the ones allowed
	if ( (hostlist) && (hostlist[0]) )
	{
		if (sock_check_hostlist((char *) hostlist, RPCAP_HOSTLIST_SEP, &from, errbuf) )
		{
			rpcap_senderror(sockctrl, errbuf, PCAP_ERR_REMOTEACCEPT, fakeerrbuf);
			sock_close(sockctrl, fakeerrbuf);
			return -1;
		}
	}

	// Send authentication to the remote machine
	if ( rpcap_sendauth(sockctrl, auth, errbuf) == -1)
	{
		rpcap_senderror(sockctrl, errbuf, PCAP_ERR_REMOTEACCEPT, fakeerrbuf);
		sock_close(sockctrl, fakeerrbuf);
		return -3;
	}

	// Checks that this host does not already have a cntrl connection in place

	// Initialize pointers
	temp= activeHosts;
	prev= NULL;
	
	while (temp)
	{
		// This host already has an active connection in place, so I don't have to update the host list
		if (sock_cmpaddr(&temp->host, &from) == 0)
			return sockctrl;

		prev= temp;
		temp= temp->next;
	}

	// The host does not exist in the list; so I have to update the list
	if (prev)
	{
		prev->next= (struct activehosts *) malloc (sizeof (struct activehosts) );
		temp= prev->next;
	}
	else
	{
		activeHosts= (struct activehosts *) malloc (sizeof (struct activehosts) );
		temp= activeHosts;
	}

	if (temp == NULL)
	{
		snprintf(errbuf, PCAP_ERRBUF_SIZE, "malloc() failed: %s", pcap_strerror(errno));
		rpcap_senderror(sockctrl, errbuf, PCAP_ERR_REMOTEACCEPT, fakeerrbuf);
		sock_close(sockctrl, fakeerrbuf);
		return -1;
	}

	memcpy(&temp->host, &from, fromlen);
	temp->sockctrl= sockctrl;
	temp->next= NULL;

	return sockctrl;
}



/*!	\ingroup remote_func

	\brief It drops an active connection (active mode only).

	This function has been defined to allow the client dealing with the 'active mode'.
	This function closes an active connection that is still in place and it purges
	the host name from the 'activeHost' list.
	From this point on, the client will not have any connection with that host in place.

	\param host: a string that keeps the host name of the host for which we want to
	close the active connection.

	\param errbuf: a pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return '0' if everything is fine, '-1' if some errors occurred. The error message is 
	returned into the errbuf variable.
*/
int pcap_remoteact_close(const char *host, char *errbuf)
{
struct activehosts *temp, *prev;	// temp var needed to scan the host list chain
struct addrinfo hints, *addrinfo, *ai_next;	// temp var needed to translate between hostname to its address
int retval;

	temp= activeHosts;
	prev= NULL;

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

	while (temp)
	{
		ai_next= addrinfo;
		while(ai_next)
		{
			if (sock_cmpaddr(&temp->host, (struct sockaddr_storage *) ai_next->ai_addr ) == 0)
			{
			struct rpcap_header header;

				// Close this connection
				rpcap_createhdr( &header, RPCAP_MSG_CLOSE, 0, 0);

				// I don't check for errors, since I'm going to close everything
				sock_send(temp->sockctrl, (char *) &header, sizeof (struct rpcap_header), errbuf);

				if (sock_close(temp->sockctrl, errbuf) )
				{
					// To avoid inconsistencies in the number of sock_init()
					sock_cleanup();

					return -1;
				}

				if (prev)
					prev->next= temp->next;
				else
					activeHosts= temp->next;

				freeaddrinfo(addrinfo);

				free(temp);

				// To avoid inconsistencies in the number of sock_init()
				sock_cleanup();

				return 0;
			}

			ai_next= ai_next->ai_next;
		}
		prev= temp;
		temp= temp->next;
	}

	if (addrinfo)
		freeaddrinfo(addrinfo);

	// To avoid inconsistencies in the number of sock_init()
	sock_cleanup();

	snprintf(errbuf, PCAP_ERRBUF_SIZE, "The host you want to close the active connection is not known");
	return -1;
}


/*!	\ingroup remote_func
	\brief Cleans the socket that is currently used in waiting active connections.

	This function does a very dirty job. The fact is that is the waiting socket is not
	freed if the pcap_remoteaccept() is killed inside a new thread. This function is
	able to clean the socket in order to allow the next calls to pcap_remoteact_accept() to work.
	
	This function is useful *only* if you launch pcap_remoteact_accept() inside a new thread,
	and you stops (not very gracefully) the thread (for example because the user changed idea,
	and it does no longer want to wait for an active connection).
	So, basically, the flow should be the following:
	- launch a new thread
	- call the pcap_remoteact_accept
	- if this new thread is killed, call pcap_remoteact_cleanup().
	
	This function has no effects in other cases.

	\return None.
*/
void pcap_remoteact_cleanup()
{
	// Very dirty, but it works
	if (sockmain)
	{
		closesocket(sockmain);

		// To avoid inconsistencies in the number of sock_init()
		sock_cleanup();
	}

}


/*!	\ingroup remote_func

	\brief Returns the hostname of the host that have an active connection with us (active mode only).

	This function has been defined to allow the client dealing with the 'active mode'.
	This function returns the list of hosts that are currently having an active connection
	with us. This function is useful in order to delete an active connection that is still
	in place.

	\param hostlist: a user-allocated string that will keep the list of host that are 
	currently connected with us.

	\param sep: the character that has to be sued as a separator between the hosts (','  for example).

	\param size: size of the hostlist buffer.

	\param errbuf: a pointer to a user-allocated buffer (of size PCAP_ERRBUF_SIZE)
	that will contain the error message (in case there is one).

	\return '0' if everything is fine, '-1' if some errors occurred. The error message is 
	returned into the errbuf variable.
*/
int pcap_remoteact_list(char *hostlist, char sep, int size, char *errbuf)
{
struct activehosts *temp;	// temp var needed to scan the host list chain
int len;
char hoststr[RPCAP_HOSTLIST_SIZE + 1];

	temp= activeHosts;

	len= 0;
	*hostlist= 0;

	while (temp)
	{
		// Get the numeric for of the name of the connecting host
		if (getnameinfo( (struct sockaddr *) &temp->host, sizeof (struct sockaddr_storage), hoststr, 
				RPCAP_HOSTLIST_SIZE, NULL, 0, NI_NUMERICHOST) )
		{
			sock_geterror("getnameinfo(): ", errbuf, PCAP_ERRBUF_SIZE);
			return -1;
		}

		len= len + strlen(hoststr) + 1 /* the separator */;

		if (len >= size)
		{
			snprintf(errbuf, PCAP_ERRBUF_SIZE, "The string you provided is not able to keep "
				"the hostnames for all the active connections");
			return -1;
		}

		strcat(hostlist, hoststr);
		hostlist[len - 1]= sep;
		hostlist[len]= 0;

		temp= temp->next;
	}

	return 0;
}

