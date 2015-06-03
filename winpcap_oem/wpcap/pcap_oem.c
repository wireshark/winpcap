/*
 * Copyright 2015 Riverbed Technology, San Francisco (California).
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
 * 3. Neither the name of Riverbed Technology nor the names of its
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

#include <pcap-int.h>

/*!
	\name Functions specific to WinPcap Professional
*/
//\{ 

/*!
	\ingroup wpcapfunc
	\brief Flag used in conjunction with \ref pcap_start_oem.
	When this flag is used, WinPcap Professional will be started without support
	for dialup and VPN adapters (i.e. without loading the Microsoft NetMon component).
*/
#define PCAP_START_OEM_NO_NETMON	0x00000001

/*!
	\ingroup wpcapfunc 
	\brief This function starts WinPcap Professional.
	
	\param err_str pointer to a user-allocated buffer (of minimum size PCAP_ERRBUF_SIZE) 
		that will contain the error message in case of failure.
	\param flags A bitwise combination of the following flags:
		- \ref PCAP_START_OEM_NO_NETMON
	\return 0 on success,  -1 in case of failure.
*/
int pcap_start_oem(char* err_str, int flags)
{
	if(PacketStartOemEx(err_str, PCAP_ERRBUF_SIZE, (ULONG)flags) == FALSE)
	{
		return -1;
	}
	return 0;
}


//\}
