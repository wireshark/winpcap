/* -*- Mode: c; tab-width: 8; indent-tabs-mode: 1; c-basic-offset: 8; -*- */
/*
 * Copyright (c) 1993, 1994, 1995, 1996, 1997
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the Computer Systems
 *	Engineering Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#) $Header: /usr/cvsroot/winpcap/dox/libpcap/funcs/pcap.h,v 1.4 2002/06/18 14:33:34 degioanni Exp $ (LBL)
 */

/** @defgroup wpcap_fn Exported functions
 *  @ingroup wpcap
 *  Functions exported by wpcap.dll
 *  @{
 */

/*!
\brief Opens a physical interface for capture.

  pcap_open_live()  is  used  to  obtain  a  packet  capture
  descriptor to look at packets on the network.  device is a
  string that specifies the network device to open; on Linux
  systems  with  2.2  or later kernels, a device argument of
  "any" or NULL can be used  to  capture  packets  from  all
  interfaces.  snaplen specifies the maximum number of bytes
  to capture.  promisc specifies if the interface is  to  be
  put into promiscuous mode.  (Note that even if this parameter 
  is false, the interface could well be in  promiscuous
  mode  for  some other reason.)  For now, this doesn't work
  on the "any" device; if an argument of "any"  or  NULL  is
  supplied,  the  promisc  flag is ignored.  to_ms specifies
  the read timeout in milliseconds.   The  read  timeout  is
  used to arrange that the read not necessarily return immediately 
  when a packet is seen, but that it wait  for  some
  amount of time to allow more packets to arrive and to read
  multiple packets from the OS kernel in one operation.  Not
  all  platforms  support  a read timeout; on platforms that
  don't, the read timeout is ignored.   errbuf  is  used  to
  return  error  or  warning  text.  It will be set to error
  text when pcap_open_live() fails and returns NULL.  errbuf
  may also be set to warning text when pcap_open_live() succeds; 
  to detect this case the caller should store a  zero
  length  string  in  errbuf before calling pcap_open_live()
  and display the warning to the user if errbuf is no longer
  a zero-length string.

\sa pcap_open_offline(), pcap_open_dead(), pcap_findalldevs(), pcap_close()
*/
pcap_t *pcap_open_live(char *device, int snaplen, int promisc, int to_ms, char *ebuf);

/*! \brief Creates a pcap_t structure without starting a capture.

  pcap_open_dead()  is  used for creating a pcap_t structure
  to use when calling the other functions in libpcap.  It is
  typically  used  when just using libpcap for compiling BPF
  code.

\sa pcap_open_offline(), pcap_open_live(), pcap_findalldevs(), pcap_compile(), pcap_setfilter(), pcap_close()
*/
pcap_t *pcap_open_dead(int linktype, int snaplen);

/*! \brief Opens a savefile in the tcpdump/libpcap format to read the packets.

  pcap_open_offline() is called to open a  "savefile"  for
  reading.   fname  specifies  the name of the file to open.
  The file has the same format as those used  by  tcpdump(1)
  and  tcpslice(1).   The  name  "-" in a synonym for stdin.
  errbuf is used to return error text and is only  set  when
  pcap_open_offline() fails and returns NULL.

\sa pcap_open_live(), pcap_dump_open(), pcap_findalldevs(), pcap_close()
*/
pcap_t *pcap_open_offline(const char *fname, char *errbuf);

/*! \brief Opens a file to write the network traffic.

  pcap_dump_open()  is  called  to  open  a "savefile" for
  writing. fname  is the  name  of  the  file  to  open. 
  The name "-" in a synonym for  stdout. If NULL is returned,
  pcap_geterr() can be used to get the error text.

\sa pcap_dump_close(), pcap_file(), pcap_dump ()
*/
pcap_dumper_t *pcap_dump_open(pcap_t *p, const char *fname);

/*! \brief Switches between blocking and nonblocking mode.

       pcap_setnonblock() puts a capture descriptor, opened  with
       pcap_open_live(),  into "non-blocking" mode, or takes it
       out of "non-blocking" mode,  depending  on  whether  the
       nonblock  argument  is non-zero or zero.  It has no effect
       on "savefiles".  If there is an error,  -1  is  returned
       and errbuf is filled in with an appropriate error message;
       otherwise, 0 is returned.  In  "non-blocking"  mode,  an
       attempt to read from the capture descriptor with pcap_dispatch() 
	   will, if no packets are currently available to  be
       read,  return  0  immediately rather than blocking waiting
       for packets to arrive.  pcap_loop() and  pcap_next()  will
       not work in "non-blocking" mode.

\sa pcap_getnonblock(), pcap_dispatch()
*/
int pcap_setnonblock(pcap_t *p, int nonblock, char *errbuf);

/*! \brief Get the "non-blocking" state of an interface.

       pcap_getnonblock()  returns  the  current "non-blocking"
       state of the capture descriptor; it always  returns  0  on
       "savefiles".   If  there is an error, -1 is returned and
       errbuf is filled in with an appropriate error message.

\sa pcap_setnonblock()
*/
int pcap_getnonblock(pcap_t *p, char *errbuf);

/*! \brief Returns the list of the interfaces  available on the system.

       pcap_findalldevs() constructs a list  of  network  devices
       that  can  be  opened  with  pcap_open_live().  (Note that
       there may be network devices that cannot  be  opened  with
       pcap_open_live()   by   the   process  calling  pcap_findalldevs(), 
	   because, for example, that  process  might  not
       have  sufficient privileges to open them for capturing; if
       so, those devices will not appear on the list.)   alldevsp
       is  set  to  point  to the first element of the list; each
       element of the list is of type pcap_if_t.

\sa struct pcap_if, pcap_freealldevs(), pcap_open_live(), pcap_lookupdev(), pcap_lookupnet()
*/
int pcap_findalldevs(pcap_if_t **alldevsp, char *errbuf);

/*! \brief Frees the list of interfaces.

       pcap_freealldevs()  is  used  to  free a list allocated by pcap_findalldevs().

\sa pcap_findalldevs()
*/
void	pcap_freealldevs(pcap_if_t *alldevsp);

/*! \brief Returns the first valid device in the system.

       pcap_lookupdev() returns a pointer  to  a  network  device
       suitable  for  use  with pcap_open_live() and pcap_lookupnet().
	   If there is an error, NULL is returned and  errbuf
       is filled in with an appropriate error message.

\sa pcap_findalldevs(), pcap_open_live()
*/
char *pcap_lookupdev(char *errbuf);

/*! \brief Returns the subnet and the netmask of an interface.

       pcap_lookupnet()  is  used to determine the network number
       and mask associated with the network device device.   Both
       netp  and  maskp are bpf_u_int32 pointers.  A return of -1
       indicates an error in which case errbuf is filled in  with
       an appropriate error message.

\sa pcap_findalldevs()
*/
int pcap_lookupnet(char *device, bpf_u_int32 *netp, bpf_u_int32 *maskp, char *errbuf);

/*! \brief Collects a group of packets. Returns when the timeout set with pcap_open_live() expires.

       pcap_dispatch()  is  used  to collect and process packets.
       cnt specifies the maximum number  of  packets  to  process
       before  returning.   This  is  not  a minimum number; when
       reading a live capture, only one bufferful of  packets  is
       read  at  a  time,  so  fewer than cnt packets may be processed. 
	   A cnt of -1 processes all the packets received  in
       one buffer when reading a live capture, or all the packets
       in the file when reading a "savefile".  callback  specifies 
	   a routine to be called with three arguments: a u_char
       pointer which is passed in from pcap_dispatch(), a pointer
       packet data.

       The  number of packets read is returned.  0 is returned if
       no packets were read from a live capture (if, for example,
       they  were  discarded  because they didn't pass the packet
       filter, or if, on platforms that support  a  read  timeout
       that starts before any packets arrive, the timeout expires
       before any packets arrive, or if the file  descriptor  for
       the  capture device is in non-blocking mode and no packets
       were available to be read)  or  if  no  more  packets  are
       available  in  a "savefile." A return of -1 indicates an
       error in which case pcap_perror() or pcap_geterr() may  be
       used to display the error text.

       \note when  reading  a live capture, pcap_dispatch() will
       not necessarily return when the read times  out;  on  some
       platforms, the read timeout isn't supported, and, on other
       platforms, the timer doesn't  start  until  at  least  one
       packet  arrives.   This means that the read timeout should
       NOT be used in, for example, an  interactive  application,
       to  allow  the  packet  capture  loop to "poll" for user
       input periodically, as there's no guarantee that pcap_dispatch() 
	   will return after the timeout expires.

\sa pcap_loop(), pcap_next(), pcap_open_live(), pcap_open_offline(), pcap_handler
*/
int pcap_dispatch(pcap_t *p, int cnt, pcap_handler callback, u_char *user);

/*! \brief Collects a group of packets. Doesn't return until the specified number
 of packets has been received, also if the timeout set with pcap_open_live() expires.

       pcap_loop()  is similar to pcap_dispatch() except it keeps
       reading packets until cnt  packets  are  processed  or  an
       error  occurs.  It does not return when live read timeouts
       occur.  Rather, specifying  a  non-zero  read  timeout  to
       pcap_open_live()  and  then calling pcap_dispatch() allows
       the reception and processing of any  packets  that  arrive
       when   the   timeout   occurs.    A  negative  cnt  causes
       pcap_loop() to loop forever (or at least  until  an  error
       occur s).

\sa pcap_dispatch(), pcap_next(), pcap_open_live(), pcap_open_offline(), pcap_handler
*/
int pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user);

/*!\brief Saves a packet to disk.

       pcap_dump() outputs a packet to  the  "savefile"  opened
       with  pcap_dump_open().   Note  that its calling arguments
       are suitable for use with pcap_dispatch() or  pcap_loop().
       If   called  directly,  the  user  parameter  is  of  type
       pcap_dumper_t as returned by pcap_dump_open().

\sa pcap_dump_open(), pcap_dump_close(), pcap_dispatch(), pcap_loop()
*/
void pcap_dump(u_char *user, const struct pcap_pkthdr *h, const u_char *sp);

/*! \brief Compiles a packet filter. Converts an high level filtering expression 
(see \ref language) in a program that can be interpreted by the kernel-level
filtering engine.

       pcap_compile() is used to compile the string  str  into  a
       filter  program.   program  is  a pointer to a bpf_program
       struct and is filled in by pcap_compile().  optimize  controls  
	   whether  optimization on the resulting code is performed.  
	   netmask specifies the netmask of the  local  net.
       A   return   of  -1  indicates  an  error  in  which  case
       pcap_geterr() may be used to display the error text.

\sa pcap_open_live(), pcap_setfilter(), pcap_freecode(), pcap_snapshot()
*/
int pcap_compile(pcap_t *p, struct bpf_program *fp, char *str, int optimize, bpf_u_int32 netmask);

/*!\brief Compiles a packet filter without the need of opening an adapter. Converts an high level filtering expression 
(see \ref language) in a program that can be interpreted by the kernel-level
filtering engine.

       pcap_compile_nopcap() is similar to pcap_compile() except 
       that  instead  of passing a pcap structure, one passes the
       snaplen and linktype explicitly.  It  is  intended  to  be
       used  for  compiling filters for direct BPF usage, without
       necessarily having called pcap_open().   A  return  of  -1
       indicates   an  error;  the  error  text  is  unavailable.
       (pcap_compile_nopcap()     is     a     wrapper     around
       pcap_open_dead(),  pcap_compile(),  and  pcap_close(); the
       latter three routines can be used directly in order to get
       the error text for a compilation error.)

       Look at the \ref language section for details on the 
       str parameter.

\sa pcap_open_live(), pcap_setfilter(), pcap_freecode(), pcap_snapshot()
*/
int pcap_compile_nopcap(int snaplen_arg, int linktype_arg, struct bpf_program *program, char *buf, int optimize, bpf_u_int32 mask);

/*! \brief Associates a filter to a capture.

       pcap_setfilter()  is used to specify a filter program.  fp
       is a pointer to a bpf_program struct, usually  the  result
       of  a  call to pcap_compile().  -1 is returned on failure,
       in which case pcap_geterr() may be  used  to  display  the
       error text; 0 is returned on success.

\sa pcap_compile(), pcap_compile_nopcap()
*/
int pcap_setfilter(pcap_t *p, struct bpf_program *fp);

/*! \brief Frees a filter.

       pcap_freecode()  is  used  to  free  up  allocated  memory
       pointed to by a bpf_program struct generated by  pcap_compile()  
	   when  that  BPF  program  is no longer needed, for
       example after it has been made the filter  program  for  a
       pcap structure by a call to pcap_setfilter().

\sa pcap_compile(), pcap_compile_nopcap()
*/
void pcap_freecode(struct bpf_program *fp);

/*! \brief Returns the next available packet.

       pcap_next()  reads  the  next packet (by calling pcap_dispatch() 
	   with a cnt of 1) and returns a u_char  pointer  to
       the data in that packet.  (The pcap_pkthdr struct for that
       packet is not supplied.)

\sa pcap_dispatch(), pcap_loop()
*/

u_char *pcap_next(pcap_t *p, struct pcap_pkthdr *h);

/*! \brief Returns the data link of an adapter.

       pcap_datalink()  returns  the  link layer type; link layer
       types it can return include:
            - DLT_NULL
                 BSD  loopback  encapsulation;  the  link   layer
                 header  is  a  4-byte field, in host byte order,
                 containing a PF_ value  from  socket.h  for  the
                 network-layer protocol of the packet
                 Note  that "host byte order" is the byte order
                 of the machine on which  the  packets  are  captured,  
				 and the PF_ values are for the OS of the
                 machine on which the packets are captured; if  a
                 live  capture is being done, "host byte order"
                 is the byte order of the machine  capturing  the
                 packets,  and the PF_ values are those of the OS
                 of the machine capturing the packets, but  if  a
                 "savefile"  is  being read, the byte order and
                 PF_ values are  not  necessarily  those  of  the
                 machine reading the capture file.
            - DLT_EN10MB
                 Ethernet (10Mb, 100Mb, 1000Mb, and up)
            - DLT_IEEE802
                 IEEE 802.5 Token Ring
            - DLT_ARCNET
                 ARCNET
                 SLIP; the link layer header contains, in order:
                      a  1-byte  flag,  which  is  0  for packets
                      received by the machine and 1  for  packets
                      sent by the machine;
                      a  1-byte  field, the upper 4 bits of which
                      indicate the type of  packet,  as  per  RFC
                      1144:
                           - 0x40 an  unmodified    IP   datagram
                                (TYPE_IP);
                           - 0x70 an uncompressed-TCP IP datagram
                                (UNCOMPRESSED_TCP),   with   that
                                byte being the first byte of  the
                                raw  IP  header on the wire, containing 
								the connection number  in
                                the protocol field;
                           - 0x80 a compressed-TCP   IP  datagram
                                (COMPRESSED_TCP), with that  byte
                                being  the first byte of the compressed 
								TCP/IP datagram header;
                      for UNCOMPRESSED_TCP, the rest of the modified 
					  IP header, and for COMPRESSED_TCP, the
                      compressed TCP/IP datagram header;
                 for a total of 16  bytes;  the  uncompressed  IP
                 datagram follows the header
            - DLT_PPP
                 PPP;  if  the  first  2 bytes are 0xff and 0x03,
                 it's PPP in  HDLC-like  framing,  with  the  PPP
                 header following those two bytes, otherwise it's
                 PPP without framing, and the packet begins  with
                 the PPP header
            - DLT_FDDI
                 FDDI
            - DLT_ATM_RFC1483
                 RFC  1483  LLC/SNAP-encapsulated ATM; the packet
                 begins with an IEEE 802.2 LLC header
            - DLT_RAW
                 raw IP; the packet begins with an IP header
            - DLT_PPP_SERIAL
                 PPP in HDLC-like framing, as per  RFC  1662,  or
                 Cisco  PPP  with  HDLC  framing,  as per section
                 or 0x8F for Cisco PPP with HDLC framing
            - DLT_PPP_ETHER
                 PPPoE; the packet begins with a PPPoE header, as
                 per RFC 2516
            - DLT_C_HDLC
                 Cisco  PPP  with  HDLC  framing,  as per section
                 4.3.1 of RFC 1547
            - DLT_IEEE802_11
                 IEEE 802.11 wireless LAN
            - DLT_LOOP
                 OpenBSD loopback encapsulation; the  link  layer
                 header is a 4-byte field, in network byte order,
                 containing a PF_ value from  OpenBSD's  socket.h
                 for the network-layer protocol of the packet
                 Note  that,  if  a  "savefile"  is being read,
                 those PF_ values are not  necessarily  those  of
                 the machine reading the capture file.
            - DLT_LINUX_SLL
                 Linux  "cooked"  capture encapsulation; the link
                 layer header contains, in order:
                      a 2-byte "packet  type",  in  network  byte
                      order, which is one of:
                           - 0  packet was sent to us by somebody
                                else
                           - 1  packet was broadcast by  somebody
                                else
                           - 2  packet  was  multicast,  but  not
                                broadcast, by somebody else
                           - 3  packet was sent by somebody  else
                                to somebody else
                           - 4  packet was sent by us
                      a 2-byte field, in network byte order, containing 
					  a Linux ARPHRD_ value for the  link
                      layer device type;
                      a 2-byte field, in network byte order, containing  
					  the  length  of  the  link   layer
                      address  of the sender of the packet (which
                      could be 0);
                      bytes  of  the  link layer header (if there
                      are more than 8 bytes, only the first 8 are
                      present);
                      a  2-byte field containing an Ethernet protocol 
					  type, in network byte order, or  containing  
					  1  for Novell 802.3 frames without
                      an 802.2 LLC header or 4 for frames  beginning 
					  with an 802.2 LLC header.
            - DLT_LTALK
                 Apple  LocalTalk;  the  packet  begins  with  an
                 AppleTalk LLAP header
*/
int pcap_datalink(pcap_t *p);

/*! \brief Returns the portion of the packets (in bytes) 
 captured by libpcap.

       pcap_snapshot() returns the snapshot length specified when
       pcap_open_live was called.

\sa pcap_open_live(), pcap_compile(), pcap_compile_nopcap()
*/
int pcap_snapshot(pcap_t *p);

/*! \brief
returns true if the current "savefile"
uses a different byte order than the current system.
*/
int pcap_is_swapped(pcap_t *p);

/*! \brief
       returns the major number of the  version of the pcap used to write the savefile.

\sa pcap_minor_version()
*/
int pcap_major_version(pcap_t *p);

/*! \brief
       returns the minor number of the version of the pcap used to write the savefile.

\sa pcap_major_version()
*/
int pcap_minor_version(pcap_t *p);

/*! \brief Returns statistics on current capture.

       pcap_stats()  returns  0  and fills in a pcap_stat struct.
       The values represent packet statistics from the  start  of
       the  run  to the time of the call. If there is an error or
       the  underlying  packet  capture  doesn't  support  packet
       statistics,  -1  is  returned  and  the  error text can be
       obtained    with    pcap_perror()    or     pcap_geterr().
       pcap_stats()  is  supported  only on live captures, not on
       "savefiles"; no statistics are stored in  "savefiles",
       so no statistics are available when reading from a "savefile".

\sa pcap_open_live()
*/
int pcap_stats(pcap_t *p, struct pcap_stat *ps);

/*! \brief Returns the stdio stream of an offile capture.

       pcap_file() returns the standard I/O stream of the "savefile",
	   if    a    "savefile"    was    opened   with
       pcap_open_offline(), or NULL,  if  a  network  device  was
       opened with pcap_open_live().

\sa pcap_open_offline(), pcap_open_live()
*/
FILE *pcap_file(pcap_t *p);

/*! \brief Returns the file descriptor of a capture device.

       pcap_fileno() returns  the  file  descriptor  number  from
       which  captured  packets are read, if a network device was
       opened with pcap_open_live(), or -1, if a "savefile" was
       opened with pcap_open_offline().

\sa pcap_open_offline(), pcap_open_live()
*/
int pcap_fileno(pcap_t *p);

/*! \brief
       prints  the  text  of the last pcap library
       error on stderr, prefixed by prefix.

\sa pcap_geterr()
*/
void pcap_perror(pcap_t *p, char *prefix);

/*! \brief
       returns the error  text  pertaining  to  the
       last  pcap  library  error.   

       \note the pointer it returns will no longer point to a valid 
       error message string after the pcap_t passed to it is closed; 
       you must use or copy the string before closing the pcap_t. 

\sa pcap_perror()
*/
char *pcap_geterr(pcap_t *p);

/*! \brief
       pcap_strerror() is  provided  in  case  strerror()  isn't
       available.

\sa pcap_perror(), pcap_geterr()
*/
char *pcap_strerror(int error);

/*! \brief
       closes the files associated with p and deallocates resources.

\sa pcap_open_live(), pcap_open_offline(), pcap_open_dead()
*/
\fn void pcap_close(pcap_t *p);

/*! \brief
       pcap_dump_close() closes the "savefile".

\sa pcap_dump_open(), pcap_dump()
*/
\fn void pcap_dump_close(pcap_dumper_t *p);

/*! \brief <b>Win32 Specific.</b> Sets the size of the kernel buffer associated with the adapter p to dim bytes.

Return value is 0 when the call succeeds, -1 otherwise. If an old buffer was already created 
with a previous call to pcap_setbuff(), it is deleted and the packets contained are discarded. 
pcap_open_live() creates a 1MB buffer by default. 

\sa pcap_open_live(), pcap_loop(), pcap_dispatch()
*/
\fn int pcap_setbuff(pcap_t *p, int dim);
/*! \brief <b>Win32 Specific.</b> Sets the working mode of the interface p to mode. 

Valid values for mode are 
MODE_CAPT (default capture mode) and MODE_STAT (statistical mode). If the interface is in statistical 
mode, the callback function set by pcap_dispatch() or pcap_loop() is invoked every to_ms milliseconds 
(where to_ms is the timeout passed as an input parameter to pcap_open_live()). The received data 
contains two 64 bit integers indicating respectively the number of packets and the amount of total 
bytes that satisfied the BPF filter set with pcap_setfilter().
*/
int pcap_setmode(pcap_t *p, int mode);

/*! \brief <b>Win32 Specific.</b> sends a raw packet.

This function allows to send a raw packet to 
the network using libpcap instead of accessing directly the underlying APIs. p is the interface that 
will be used to send the packet, buf contains the data of the packet to send (including the various 
protocol headers), size is the dimension of the buffer pointed by buf. The MAC CRC doesn't need to be 
calculated and added to the packet, because it is transparently put after the end of the data portion 
by the network interface. 

\sa pcap_open_live()
*/
int pcap_sendpacket(pcap_t *p, u_char *buf, int size);

/*! \brief <b>Win32 Specific.</b> Sets the minumum amount of data received by the kernel in a single call.

pcap_setmintocopy() changes the minimum amount of data in the kernel buffer that causes a read from 
the packet driver to return (unless the timeout expires). If size is big, the kernel is forced to wait 
the arrival of several packets before copying the data to the user. This guarantees a low number of 
system calls, i.e. low processor usage, and is a good setting for applications like packet-sniffers 
and protocol analyzers. Vice versa, in presence of a small value for this variable, the kernel will 
copy the packets as soon as the application is ready to receive them. This is useful for real time 
applications that need the best responsiveness from the kernel.

\sa pcap_open_live(), pcap_loop(), pcap_dispatch()
*/
int pcap_setmintocopy(pcap_t *p, int size);

/*! \brief <b>Win32 Specific.</b> Returns the handle of the event associated with the interface p. 

This event can be passed to functions like WaitForSingleObject() or WaitForMultipleObjects() to wait 
until the driver's buffer contains some data without performing a read.

\sa pcap_open_live()
*/
HANDLE pcap_getevent(pcap_t *p);

/*! \brief <b>Win32 Specific.</b> Allocate a send queue. 

This function allocate a send queue, i.e. a buffer containing a set of raw packets that will be sent with
pcap_sendqueue_transmit().

memsize is the size, in bytes, of the queue, therefore it determines the maximum amount of data that the 
queue will contain.

Use pcap_sendqueue_queue() to add a packet to the queue.

\sa pcap_sendqueue_queue(), pcap_sendqueue_transmit(), pcap_sendqueue_destroy()
*/
pcap_send_queue* pcap_sendqueue_alloc(u_int memsize);

/*! \brief <b>Win32 Specific.</b> Destroy a send queue. 

Delete the send queue pointed by the queue parameter and frees all the memory associated with it.

\sa pcap_sendqueue_alloc(), pcap_sendqueue_queue(), pcap_sendqueue_transmit()
*/
void pcap_sendqueue_destroy(pcap_send_queue* queue);

/*! \brief <b>Win32 Specific.</b> Add a packet to a send queue. 

pcap_sendqueue_queue() adds a packet at the end of the send queue pointed by the queue parameter. 
pkt_header points to a pcap_pkthdr structure with the timestamp and the length of the packet, pkt_data
points to a buffer with the raw packet.

The pcap_pkthdr structure is the same used by WinPcap and libpcap to store the packets in a file, 
therefore sending a capture file is straightforward.
'Raw packet' means that the sending application will have to include the protocol headers, since every packet 
is sent to the network 'as is'. The CRC of the packets needs not to be calculated, because it will be 
transparently added by the network interface.

\sa pcap_sendqueue_alloc(), pcap_sendqueue_transmit(), pcap_sendqueue_destroy()
*/
int pcap_sendqueue_queue(pcap_send_queue* queue, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);

/*! \brief <b>Win32 Specific.</b> Sends a queue of raw packets to the network.

This function can be used to send a queue with several packets to the wire with a single call. p is a 
pointer to the adapter on which the packets will be sent, queue points to a pcap_send_queue structure 
with the packets to send (see pcap_sendqueue_alloc() and pcap_sendqueue_queue()), sync determines if the 
send must be synchronized: if it is non-zero, the packets are sent respecting the timestamps. 
If 0, the packets are sent as fast as possible.

The return value is the amount of bytes actually sent. If it is smaller than the size parameter, an
error occurred during the send. The error can be caused by a driver/adapter problem or by an inconsistent/bogus 
packet buffer.

\note Using this function if more efficient than issuing a series of pcap_sendpacket(), because the packets are
buffered in the kernel driver, so the number of context switches is reduced.

\note When Sync is set to TRUE, the packets are synchronized in the kernel with a high precision timestamp.
This requires a remarkable amount of CPU, but allows normally to send the packets with a precision of some 
microseconds (depending on the precision of the performance counter of the machine). Such a precision cannot 
be reached sending the packets with pcap_sendpacket().

\sa pcap_sendqueue_alloc(), pcap_sendqueue_queue(), pcap_sendqueue_destroy()
*/
u_int pcap_sendqueue_transmit(pcap_t *p, pcap_send_queue* queue, int sync);

/*! \brief <b>Win32 Specific.</b> Read a packet from an interface or from an offline capture.

This function is used to explicitly read a single packet, bypassing the callback method traditionally exported
by libpcap.

pcap_read_ex fills the pkt_header and pkt_data parameters (see pcap_handler()) with the pointers to the 
header and to the data of the last captured packet.

The return value can be:
- 1 if a packet has been read without problems
- 0 if the timeout set with pcap_open_live() has elapsed. In this case pkt_header and pkt_data don't point to a valid packet
- -1 if an error occurred
- -2 if EOF was reached readinf from an offline capture

\sa pcap_open_live(), pcap_loop(), pcap_dispatch(), pcap_handler()
*/
int pcap_read_ex(pcap_t *p, struct pcap_pkthdr **pkt_header, u_char **pkt_data);

/*! \brief <b>Win32 Specific.</b> Saves a capture to file.

pcap_live_dump() dumps the network traffic from an interface to
a file. Using this function the dump is performed at kernel level, therefore it is more efficient than using
pcap_dump().

The parameters of this function are an interface descriptor (obtained with pcap_open_live()), the name of the
dump file, the maximum size of the file (in bytes) and the maximum number of packets that the file
will contain. Setting maxsize or maxpacks to 0 means no limit. When maxsize or maxpacks are reached, 
the dump ends.

pcap_live_dump() is non-blocking, threfore it returns immediately. pcap_live_dump_ended() can be used to 
check the status of the dump process or to wait until it is finished.

Note that when one of the two limits is reached, the dump is stopped, but the file remains opened. In order 
to correctly flush the data and to access the file consistently, the adapter must be closed with 
pcap_close().


\sa pcap_live_dump_ended(), pcap_open_live(), pcap_close(), pcap_dump_open(), pcap_dump()
*/
int pcap_live_dump(pcap_t *p, char *filename, int maxsize, int maxpacks);

/*! \brief <b>Win32 Specific.</b> Returns the status of the kernel dump process, i.e. tells if one of the limits defined with pcap_live_dump() has been reached.

pcap_live_dump_ended() informs the user about the limits that were set with a previous call to 
pcap_live_dump() on the interface pointed by p: the return value is nonzero, one of the limits has been 
reched and the dump process is finished. 
If sync is nonzero, the function blocks until the dump is finished, otherwise it returns immediately.

\warning if the dump process has no limits (i.e. if the maxsize and maxpacks arguments of pcap_live_dump() 
were both 0), setting sync to TRUE will block the application on this call forever.

\sa pcap_live_dump()
*/
int pcap_live_dump_ended(pcap_t *p, int sync);



/*! \brief Prototype of the callback function that receives the packets. 

When pcap_dispatch() or pcap_loop() are called by the user, the packets are passed to the application
by means of this callback. user is a user-defined parameter that contains the state of the
capture session, it corresponds to the \e user parameter of pcap_dispatch() and pcap_loop(). pkt_header is
the header associated by the capture driver to the packet. It is NOT a protocol header. pkt_data
points to the data of the packet, including the protocol headers.
*/
typedef void (*pcap_handler)(u_char *user, const struct pcap_pkthdr *pkt_header,
			     const u_char *pkt_data);

/*@}*/