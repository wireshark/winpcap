// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

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

#ifdef __cplusplus
}
#endif


#endif // !defined(AFX_WANPACKET_H__C87BC2BD_187D_4A72_A5FF_537A9F4B588F__INCLUDED_)


