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

#if DEBUG
#define IF_PACKETDEBUG(f) if ( PacketTraceImpt & (f) )
extern ULONG PacketTraceImpt;
#define PACKET_TRACE_IMPT				0x00000001  
#define PACKET_DEBUG_VERY_LOUD			0x00000002  
#define PACKET_DEBUG_INIT				0x00000100  
#define PACKET_DEBUG_BREAK				0x00000200

#define IF_BREAK_SET 		IF_PACKETDEBUG( PACKET_DEBUG_BREAK ) DbgBreakPoint();
#define IF_INIT_TRACE(A)	IF_PACKETDEBUG( PACKET_DEBUG_INIT ) { DbgPrint("Packet: %s\r\n", A); DbgBreakPoint(); }
#define IF_TRACE(A) 			IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("Packet: %s\r\n", A); IF_BREAK_SET }
#define IF_VERY_LOUD(A) 	IF_PACKETDEBUG( PACKET_DEBUG_VERY_LOUD ) { DbgPrint("Packet: %s\r\n", A); IF_BREAK_SET }
#define IF_TRACE_MSG(A,B)	IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("Packet: "); DbgPrint(A,B); DbgPrint("\r\n"); IF_BREAK_SET }
#define IF_TRACE_MSG2(A,B,C)	IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("Packet: "); DbgPrint(A,B,C); DbgPrint("\r\n"); IF_BREAK_SET }
#define IF_TRACE_MSG3(A,B,C,D)	IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("Packet: "); DbgPrint(A,B,C,D); DbgPrint("\r\n"); IF_BREAK_SET }
#define IF_TRACE_MSG4(A,B,C,D,E)	IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("Packet: "); DbgPrint(A,B,C,D,E); DbgPrint("\r\n"); IF_BREAK_SET }
#define INIT_ENTER(A)		IF_PACKETDEBUG( PACKET_DEBUG_INIT ) { DbgPrint("==> Packet: %s\r\n", A); DbgBreakPoint();}
#define INIT_LEAVE(A)		IF_PACKETDEBUG( PACKET_DEBUG_INIT ) { DbgPrint("<== Packet: %s\r\n", A); IF_BREAK_SET}
#define TRACE_ENTER(A)		IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("==> Packet: %s\r\n", A); IF_BREAK_SET}
#define TRACE_LEAVE(A)		IF_PACKETDEBUG( PACKET_TRACE_IMPT ) { DbgPrint("<== Packet: %s\r\n", A); IF_BREAK_SET}
#else
#define IF_PACKETDEBUG(f) 
#define IF_BREAK_SET
#define IF_INIT_TRACE(A)
#define IF_TRACE(A)
#define IF_VERY_LOUD(A)
#define IF_TRACE_MSG(A,B)
#define IF_TRACE_MSG2(A,B)
#define IF_TRACE_MSG3(A,B)
#define IF_TRACE_MSG4(A,B)
#define INIT_ENTER(A)
#define INIT_LEAVE(A)
#define TRACE_ENTER(A)
#define TRACE_LEAVE(A)
#endif
