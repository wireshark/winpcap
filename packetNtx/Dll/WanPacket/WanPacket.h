/*
 * Copyright (c) 1999 - 2003
 *	Politecnico di Torino.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the Politecnico
 * di Torino, and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#if !defined(AFX_WANPACKET_H__C87BC2BD_187D_4A72_A5FF_537A9F4B588F__INCLUDED_)
#define AFX_WANPACKET_H__C87BC2BD_187D_4A72_A5FF_537A9F4B588F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif


BOOLEAN WanPacketSetBpfFilter(PWAN_ADAPTER pWanAdapter, PUCHAR FilterCode, DWORD Length);
PWAN_ADAPTER WanPacketOpenAdapter();
BOOLEAN WanPacketCloseAdapter(PWAN_ADAPTER pWanAdapter);
BOOLEAN WanPacketSetBufferSize(PWAN_ADAPTER pWanAdapter, DWORD BufferSize);
DWORD WanPacketReceivePacket(PWAN_ADAPTER pWanAdapter, PUCHAR Buffer, DWORD BufferSize);
BOOLEAN WanPacketSetMinToCopy(PWAN_ADAPTER pWanAdapter, DWORD MinToCopy);
BOOLEAN WanPacketGetStats(PWAN_ADAPTER pWanAdapter, struct bpf_stat *s);
BOOLEAN WanPacketSetReadTimeout(PWAN_ADAPTER pWanAdapter, DWORD ReadTimeout);
BOOLEAN WanPacketSetMode(PWAN_ADAPTER pWanAdapter, DWORD Mode);
HANDLE WanPacketGetReadEvent(PWAN_ADAPTER pWanAdapter);
BOOLEAN WanPacketTestAdapter();

#ifdef __cplusplus
}
#endif


#endif // !defined(AFX_WANPACKET_H__C87BC2BD_187D_4A72_A5FF_537A9F4B588F__INCLUDED_)


