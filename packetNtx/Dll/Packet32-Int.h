/*
 * Copyright (c) 2006
 * CACE Technologies LLC, Davis (CA)
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
 * 3. Neither the name of the company (CACE Technologies LLC) nor the 
 * names of its contributors may be used to endorse or promote products 
 * derived from this software without specific prior written permission.
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

//
// Internal constants
//

// The following is used to check the adapter name in PacketOpenAdapterNPF and prevent 
// opening of firewire adapters 
#define FIREWIRE_SUBSTR L"1394"

#ifdef HAVE_NPFIM_API
#include "NpfImExt.h"
#endif //HAVE_NPFIM_API

#ifdef __MINGW32__
#include <ddk/ntddndis.h>
#else
#include <ntddndis.h>
#endif //__MINGW32__


//
// Internal functions
//
VOID PacketLoadLibrariesDynamically();
void PacketPopulateAdaptersInfoList();
BOOL PacketGetFileVersion(LPTSTR FileName, PCHAR VersionBuff, UINT VersionBuffLen);
PADAPTER_INFO PacketFindAdInfo(PCHAR AdapterName);
BOOLEAN PacketUpdateAdInfo(PCHAR AdapterName);
BOOLEAN IsFireWire(TCHAR *AdapterDesc);
LPADAPTER PacketOpenAdapterNPF(PCHAR AdapterName);

// 
// Definitions and functions specific to the CACETech airpcap API
//
#ifdef HAVE_AIRPCAP_API
typedef PCHAR (*AirpcapGetLastErrorHandler)(PAirpcapHandle Handle);
typedef BOOL (*AirpcapGetDeviceListHandler)(AirpcapDeviceDescription **AllDevsP, PCHAR Ebuf);		///< prototype used to dynamically load the dag dll
typedef VOID (*AirpcapFreeDeviceListHandler)(AirpcapDeviceDescription *AllDevsP);					///< prototype used to dynamically load the dag dll
typedef PAirpcapHandle (*AirpcapOpenHandler)(PCHAR DeviceName, PCHAR Ebuf);							///< prototype used to dynamically load the dag dll	
typedef VOID (*AirpcapCloseHandler)(PAirpcapHandle Handle);											///< prototype used to dynamically load the dag dll
typedef BOOL (*AirpcapGetLinkTypeHandler)(PAirpcapHandle Handle, AirpcapLinkType* LinkLayer);		///< prototype used to dynamically load the dag dll
typedef BOOL (*AirpcapSetKernelBufferHandler)(PAirpcapHandle Handle, ULONG Size);					///< prototype used to dynamically load the dag dll
typedef BOOL (*AirpcapSetFilterHandler)(PAirpcapHandle Handle, void *Instructions, UINT Len);		///< prototype used to dynamically load the dag dll
typedef BOOL (*AirpcapGetMacAddressHandler)(PAirpcapHandle Handle, CHAR **MacAddress);				///< prototype used to dynamically load the dag dll
typedef BOOL (*AirpcapSetMinToCopyHandler)(PAirpcapHandle Handle, ULONG Bytes);						///< prototype used to dynamically load the dag dll
typedef BOOL (*AirpcapGetReadEventHandler)(PAirpcapHandle Handle, HANDLE* PReadEvent);				///< prototype used to dynamically load the dag dll
typedef BOOL (*AirpcapReadHandler)(PAirpcapHandle Handle, PUCHAR BufferToFill, ULONG BufSize, PULONG ReceievedBytes);	///< prototype used to dynamically load the dag dll
typedef BOOL (*AirpcapGetStatsHandler)(PAirpcapHandle Handle, AirpcapStats *Stats);					///< prototype used to dynamically load the dag dll
typedef BOOL (*AirpcapWriteHandler)(PAirpcapHandle Handle, PCHAR TxPacket, ULONG PacketLen);		///< prototype used to dynamically load the dag dll

#endif // HAVE_AIRPCAP_API


// 
// Definitions and functions specific to the Endace Dag API
//
#ifdef HAVE_DAG_API
typedef dagc_t* (*dagc_open_handler)(const char *source, unsigned flags, char *ebuf);	///< prototype used to dynamically load the dag dll
typedef void (*dagc_close_handler)(dagc_t *dagcfd);										///< prototype used to dynamically load the dag dll
typedef int (*dagc_getlinktype_handler)(dagc_t *dagcfd);								///< prototype used to dynamically load the dag dll
typedef int (*dagc_getlinkspeed_handler)(dagc_t *dagcfd);								///< prototype used to dynamically load the dag dll
typedef int (*dagc_setsnaplen_handler)(dagc_t *dagcfd, unsigned snaplen);				///< prototype used to dynamically load the dag dll
typedef unsigned (*dagc_getfcslen_handler)(dagc_t *dagcfd);								///< prototype used to dynamically load the dag dll
typedef int (*dagc_receive_handler)(dagc_t *dagcfd, u_char **buffer, u_int *bufsize);	///< prototype used to dynamically load the dag dll
typedef int (*dagc_stats_handler)(dagc_t *dagcfd, dagc_stats_t *ps);					///< prototype used to dynamically load the dag dll
typedef int (*dagc_wait_handler)(dagc_t *dagcfd, struct timeval *timeout);				///< prototype used to dynamically load the dag dll
typedef int (*dagc_finddevs_handler)(dagc_if_t **alldevsp, char *ebuf);					///< prototype used to dynamically load the dag dll
typedef int (*dagc_freedevs_handler)(dagc_if_t *alldevsp);								///< prototype used to dynamically load the dag dll
#endif // HAVE_DAG_API
