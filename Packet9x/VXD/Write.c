/*
 * Copyright (c) 1999, 2000
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

#include <basedef.h>
#include <vmm.h>
#include <ndis.h>
#include <vwin32.h>
#include "debug.h"
#include "packet.h"
#include <ntddpack.h>
#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG
DWORD _stdcall MyPageLock(DWORD, DWORD);
void  _stdcall MyPageUnlock(DWORD, DWORD);

/************************************************************
Start the sending of a packet
************************************************************/
DWORD
PacketWrite(POPEN_INSTANCE	Open,
			DWORD  			dwDDB,
			DWORD  			hDevice,
			PDIOCPARAMETERS	pDiocParms
	)
{
	PNDIS_PACKET	pPacket;
	PNDIS_BUFFER 	pNdisBuffer;
	NDIS_STATUS		Status;
	TRACE_ENTER( "SendPacket" );

	PacketAllocatePacketBuffer( &Status, Open, &pPacket, pDiocParms, IOCTL_PROTOCOL_WRITE );
	if ( Status != NDIS_STATUS_SUCCESS )
	{
		return 0;
	}
	NdisSend( &Status, Open->AdapterHandle, pPacket );
	if ( Status != NDIS_STATUS_PENDING ) 
	{
		PacketSendComplete( Open, pPacket, Status );
	}
	TRACE_LEAVE( "SendPacket" );
	return(-1);	
}

/************************************************************
Function called by NDIS to indicate that the data transfer
is finished
************************************************************/
VOID NDIS_API
PacketSendComplete(	IN NDIS_HANDLE	ProtocolBindingContext,
					IN PNDIS_PACKET	pPacket,
					IN NDIS_STATUS	Status
   )
{
	PNDIS_BUFFER 		pNdisBuffer;
	PPACKET_RESERVED	Reserved = (PPACKET_RESERVED) pPacket->ProtocolReserved;

	TRACE_ENTER( "SendComplete" );
	
	NdisUnchainBufferAtFront( pPacket, &pNdisBuffer );
	if ( pNdisBuffer )
		NdisFreeBuffer( pNdisBuffer );
		
	VWIN32_DIOCCompletionRoutine( Reserved->lpoOverlapped->O_Internal );
		
	PacketPageUnlock( Reserved->lpBuffer, Reserved->cbBuffer );
	PacketPageUnlock( Reserved->lpcbBytesReturned, sizeof(DWORD) );
	PacketPageUnlock( Reserved->lpoOverlapped, sizeof(OVERLAPPED) );
	
	NdisReinitializePacket(pPacket);
		
	NdisFreePacket(pPacket);

	TRACE_LEAVE( "SendComplete" );
	return;
}
