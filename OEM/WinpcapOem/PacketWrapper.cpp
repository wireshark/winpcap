#include <stdio.h>
#include "Packet32.h"
#include "WinpcapOem.h"

INT setProcAuthorization();

HMODULE PacketLib;
BOOL StillToInit = TRUE;
extern HINSTANCE DllHandle;
extern char LastWoemError[];

//---------------------------------------------------------------------------
// PUBLIC PACKET.DLL API WRAPPERS - HANDLERS
//---------------------------------------------------------------------------
typedef PCHAR (*PacketGetVersionHandler)();
typedef PCHAR (*PacketGetDriverVersionHandler)();
typedef BOOL (*PacketStopDriverHandler)();
typedef LPADAPTER (*PacketOpenAdapterHandler)(PCHAR AdapterName);
typedef VOID (*PacketCloseAdapterHandler)(LPADAPTER lpAdapter);
typedef LPPACKET (*PacketAllocatePacketHandler)(void);
typedef VOID (*PacketFreePacketHandler)(LPPACKET lpPacket);
typedef VOID (*PacketInitPacketHandler)(LPPACKET lpPacket,PVOID Buffer,UINT Length);
typedef BOOLEAN (*PacketReceivePacketHandler)(LPADAPTER AdapterObject,LPPACKET lpPacket,BOOLEAN Sync);
typedef BOOLEAN (*PacketSendPacketHandler)(LPADAPTER AdapterObject,LPPACKET lpPacket,BOOLEAN Sync);
typedef INT (*PacketSendPacketsHandler)(LPADAPTER AdapterObject, PVOID PacketBuff, ULONG Size, BOOLEAN Sync);
typedef BOOLEAN (*PacketSetMinToCopyHandler)(LPADAPTER AdapterObject,int nbytes);
typedef BOOLEAN (*PacketSetModeHandler)(LPADAPTER AdapterObject,int mode);
typedef BOOLEAN (*PacketSetDumpNameHandler)(LPADAPTER AdapterObject, void *name, int len);
typedef BOOLEAN (*PacketSetDumpLimitsHandler)(LPADAPTER AdapterObject, UINT maxfilesize, UINT maxnpacks);
typedef BOOLEAN (*PacketIsDumpEndedHandler)(LPADAPTER AdapterObject, BOOLEAN sync);
typedef HANDLE (*PacketGetReadEventHandler)(LPADAPTER AdapterObject);
typedef BOOLEAN (*PacketSetNumWritesHandler)(LPADAPTER AdapterObject,int nwrites);
typedef BOOLEAN (*PacketSetReadTimeoutHandler)(LPADAPTER AdapterObject,int timeout);
typedef BOOLEAN (*PacketSetBuffHandler)(LPADAPTER AdapterObject,int dim);
typedef BOOLEAN (*PacketSetBpfHandler)(LPADAPTER AdapterObject, struct bpf_program *fp);
typedef INT (*PacketSetSnapLenHandler)(LPADAPTER AdapterObject, int snaplen);
typedef BOOLEAN (*PacketGetStatsHandler)(LPADAPTER AdapterObject,struct bpf_stat *s);
typedef BOOLEAN (*PacketGetStatsExHandler)(LPADAPTER AdapterObject,struct bpf_stat *s);
typedef BOOLEAN (*PacketRequestHandler)(LPADAPTER  AdapterObject,BOOLEAN Set,PPACKET_OID_DATA  OidData);
typedef BOOLEAN (*PacketSetHwFilterHandler)(LPADAPTER  AdapterObject,ULONG Filter);
typedef BOOLEAN (*PacketGetAdapterNamesHandler)(PTSTR pStr,PULONG  BufferSize);
typedef BOOLEAN (*PacketGetNetInfoExHandler)(PCHAR AdapterName, npf_if_addr* buffer, PLONG NEntries);
typedef BOOLEAN (*PacketGetNetTypeHandler)(LPADAPTER AdapterObject, NetType *type);
typedef VOID (*PacketRegWoemLeaveHandlerHandler) (PVOID Handler);
typedef BOOLEAN (*PacketSetLoopbackBehaviorHandler)(LPADAPTER  AdapterObject, UINT LoopbackBehavior);

PacketGetVersionHandler				PacketGetVersionH = NULL;
PacketGetDriverVersionHandler		PacketGetDriverVersionH = NULL;
PacketStopDriverHandler				PacketStopDriverH = NULL;
PacketOpenAdapterHandler			PacketOpenAdapterH = NULL;
PacketCloseAdapterHandler			PacketCloseAdapterH = NULL;
PacketAllocatePacketHandler			PacketAllocatePacketH = NULL;
PacketFreePacketHandler				PacketFreePacketH = NULL;
PacketInitPacketHandler				PacketInitPacketH = NULL;
PacketReceivePacketHandler			PacketReceivePacketH = NULL;
PacketSendPacketHandler				PacketSendPacketH = NULL;
PacketSendPacketsHandler			PacketSendPacketsH = NULL;
PacketSetMinToCopyHandler			PacketSetMinToCopyH = NULL;
PacketSetModeHandler				PacketSetModeH = NULL;
PacketSetDumpNameHandler			PacketSetDumpNameH = NULL;
PacketSetDumpLimitsHandler			PacketSetDumpLimitsH = NULL;
PacketIsDumpEndedHandler			PacketIsDumpEndedH = NULL;
PacketGetReadEventHandler			PacketGetReadEventH = NULL;
PacketSetNumWritesHandler			PacketSetNumWritesH = NULL;
PacketSetReadTimeoutHandler			PacketSetReadTimeoutH = NULL;
PacketSetBuffHandler				PacketSetBuffH = NULL;
PacketSetBpfHandler					PacketSetBpfH = NULL;
PacketSetSnapLenHandler				PacketSetSnapLenH = NULL;
PacketGetStatsHandler				PacketGetStatsH = NULL;
PacketGetStatsExHandler				PacketGetStatsExH = NULL;
PacketRequestHandler				PacketRequestH = NULL;
PacketSetHwFilterHandler			PacketSetHwFilterH = NULL;
PacketGetAdapterNamesHandler		PacketGetAdapterNamesH = NULL;
PacketGetNetInfoExHandler			PacketGetNetInfoExH = NULL;
PacketGetNetTypeHandler				PacketGetNetTypeH = NULL;
PacketSetLoopbackBehaviorHandler	PacketSetLoopbackBehaviorH = NULL;

PacketRegWoemLeaveHandlerHandler PacketRegWoemLeaveHandlerH = NULL;

//---------------------------------------------------------------------------
// PUBLIC PACKET.DLL API WRAPPERS - IMPLEMENTATIONS
//---------------------------------------------------------------------------
PCHAR PacketGetVersion()
{
	TraceEnter("PacketGetVersion");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return "failed to load packet.dll";
		}

		StillToInit = FALSE;
	}

	return PacketGetVersionH();
}

PCHAR PacketGetDriverVersion()
{
	TraceEnter("PacketGetDriverVersion");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return "failed to load packet.dll";
		}

		StillToInit = FALSE;
	}

	//
	// We don't have the driver binary, but we are sure that driver and dll 
	// versions are identical
	//
	return PacketGetVersionH();
}

BOOL PacketStopDriver()
{
	TraceEnter("PacketStopDriver");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketStopDriverH();
}

LPADAPTER PacketOpenAdapter(PCHAR AdapterName)
{
	TraceEnter("PacketOpenAdapter");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return NULL;
		}

		StillToInit = FALSE;
	}

	return PacketOpenAdapterH(AdapterName);
}

VOID PacketCloseAdapter(LPADAPTER lpAdapter)
{
	TraceEnter("PacketCloseAdapter");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return;
		}

		StillToInit = FALSE;
	}

	PacketCloseAdapterH(lpAdapter);
}

LPPACKET PacketAllocatePacket(void)
{
	TraceEnter("PacketAllocatePacket");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return NULL;
		}

		StillToInit = FALSE;
	}

	return PacketAllocatePacketH();
}

VOID PacketFreePacket(LPPACKET lpPacket)
{
	TraceEnter("PacketFreePacket");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return;
		}

		StillToInit = FALSE;
	}

	PacketFreePacketH(lpPacket);
}

VOID PacketInitPacket(LPPACKET lpPacket,PVOID Buffer,UINT Length)
{
	TraceEnter("PacketInitPacket");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return;
		}

		StillToInit = FALSE;
	}

	PacketInitPacketH(lpPacket, Buffer, Length);
}

BOOLEAN PacketReceivePacket(LPADAPTER AdapterObject,LPPACKET lpPacket,BOOLEAN Sync)
{
	TraceEnter("PacketReceivePacket");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketReceivePacketH(AdapterObject, lpPacket, Sync);
}

BOOLEAN PacketSendPacket(LPADAPTER AdapterObject,LPPACKET lpPacket,BOOLEAN Sync)
{
	TraceEnter("PacketSendPacket");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketSendPacketH(AdapterObject, lpPacket, Sync);
}

INT PacketSendPackets(LPADAPTER AdapterObject, PVOID PacketBuff, ULONG Size, BOOLEAN Sync)
{
	TraceEnter("PacketSendPackets");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return 0;
		}

		StillToInit = FALSE;
	}

	return PacketSendPacketsH(AdapterObject, PacketBuff, Size, Sync);
}

BOOLEAN PacketSetMinToCopy(LPADAPTER AdapterObject,int nbytes)
{
	TraceEnter("PacketSetMinToCopy");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketSetMinToCopyH(AdapterObject, nbytes);
}

BOOLEAN PacketSetMode(LPADAPTER AdapterObject,int mode)
{
	TraceEnter("PacketSetMode");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketSetModeH(AdapterObject, mode);
}

BOOLEAN PacketSetDumpName(LPADAPTER AdapterObject, void *name, int len)
{
	TraceEnter("PacketSetDumpName");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketSetDumpNameH(AdapterObject, name, len);
}

BOOLEAN PacketSetDumpLimits(LPADAPTER AdapterObject, UINT maxfilesize, UINT maxnpacks)
{
	TraceEnter("PacketSetDumpLimits");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketSetDumpLimitsH(AdapterObject, maxfilesize, maxnpacks);
}

BOOLEAN PacketIsDumpEnded(LPADAPTER AdapterObject, BOOLEAN sync)
{
	TraceEnter("PacketIsDumpEnded");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketIsDumpEndedH(AdapterObject, sync);
}

HANDLE PacketGetReadEvent(LPADAPTER AdapterObject)
{
	TraceEnter("PacketGetReadEvent");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return NULL;
		}

		StillToInit = FALSE;
	}

	return PacketGetReadEventH(AdapterObject);
}

BOOLEAN PacketSetNumWrites(LPADAPTER AdapterObject,int nwrites)
{
	TraceEnter("PacketSetNumWrites");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketSetNumWritesH(AdapterObject, nwrites);
}

BOOLEAN PacketSetReadTimeout(LPADAPTER AdapterObject,int timeout)
{
	TraceEnter("PacketSetReadTimeout");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketSetReadTimeoutH(AdapterObject, timeout);
}

BOOLEAN PacketSetBuff(LPADAPTER AdapterObject,int dim)
{
	TraceEnter("PacketSetBuff");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketSetBuffH(AdapterObject, dim);
}

BOOLEAN PacketSetBpf(LPADAPTER AdapterObject, struct bpf_program *fp)
{
	TraceEnter("PacketSetBpf");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketSetBpfH(AdapterObject, fp);
}

BOOLEAN PacketSetLoopbackBehavior(LPADAPTER  AdapterObject, UINT LoopbackBehavior)
{
	TraceEnter("PacketSetLoopbackBehavior");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketSetLoopbackBehaviorH(AdapterObject, LoopbackBehavior);
}

INT PacketSetSnapLen(LPADAPTER AdapterObject, int snaplen)
{
	TraceEnter("PacketSetSnapLen");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return 0;
		}

		StillToInit = FALSE;
	}

	return PacketSetSnapLenH(AdapterObject, snaplen);
}

BOOLEAN PacketGetStats(LPADAPTER AdapterObject,struct bpf_stat *s)
{
	TraceEnter("PacketGetStats");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketGetStatsH(AdapterObject, s);
}

BOOLEAN PacketGetStatsEx(LPADAPTER AdapterObject,struct bpf_stat *s)
{
	TraceEnter("PacketGetStatsEx");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketGetStatsExH(AdapterObject, s);
}

BOOLEAN PacketRequest(LPADAPTER  AdapterObject,BOOLEAN Set,PPACKET_OID_DATA  OidData)
{
	TraceEnter("PacketRequest");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketRequestH(AdapterObject, Set, OidData);
}

BOOLEAN PacketSetHwFilter(LPADAPTER  AdapterObject,ULONG Filter)
{
	TraceEnter("PacketSetHwFilter");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketSetHwFilterH(AdapterObject, Filter);
}

BOOLEAN PacketGetAdapterNames(PTSTR pStr,PULONG  BufferSize)
{
	TraceEnter("PacketGetAdapterNames");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			*BufferSize = 0;
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketGetAdapterNamesH(pStr,  BufferSize);
}

BOOLEAN PacketGetNetInfoEx(PCHAR AdapterName, npf_if_addr* buffer, PLONG NEntries)
{
	TraceEnter("PacketGetNetInfoEx");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketGetNetInfoExH(AdapterName, buffer, NEntries);
}

BOOLEAN PacketGetNetType(LPADAPTER AdapterObject, NetType *type)
{
	TraceEnter("PacketGetNetType");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(StillToInit)
	{
		if(!WoemEnterDll(DllHandle))
		{
			return FALSE;
		}

		StillToInit = FALSE;
	}

	return PacketGetNetTypeH(AdapterObject, type);
}

//---------------------------------------------------------------------------
// ADDITIONAL EXPORTS NOT PRESENT IN STANDARD PACKET.DLL
//---------------------------------------------------------------------------
PCHAR PacketGetLastOemError()
{
	return LastWoemError;
}

//---------------------------------------------------------------------------
// FUNCTIONS
//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////
// Load the packet.dll binary with the specifed name
////////////////////////////////////////////////////////////////////
BOOL LoadPacketDll(char *PacketDllFileName)
{	
	// 
	// Dinamically load the packet.dll library with loadlibrary
	//
	if((PacketLib =  LoadLibrary(PacketDllFileName)) == NULL)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to initialize the Packet.dll dynamic library");
		WoemReportError();
		return FALSE;
	}

	// 
	// Get the entry points of the just loaded library
	//
	PacketGetVersionH = (PacketGetVersionHandler) GetProcAddress(PacketLib, "PacketGetVersion");	
	if(!PacketGetVersionH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 1)");
		WoemReportError();
		return FALSE;
	}

	PacketGetDriverVersionH = (PacketGetDriverVersionHandler) GetProcAddress(PacketLib, "PacketGetDriverVersion");
	if(!PacketGetDriverVersionH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 2)");
		WoemReportError();
		return FALSE;
	}

	PacketStopDriverH = (PacketStopDriverHandler) GetProcAddress(PacketLib, "PacketStopDriver");
	if(!PacketStopDriverH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 3)");
		WoemReportError();
		return FALSE;
	}

	PacketOpenAdapterH = (PacketOpenAdapterHandler) GetProcAddress(PacketLib, "PacketOpenAdapter");
	if(!PacketOpenAdapterH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 4)");
		WoemReportError();
		return FALSE;
	}

	PacketCloseAdapterH = (PacketCloseAdapterHandler) GetProcAddress(PacketLib, "PacketCloseAdapter");
	if(!PacketCloseAdapterH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 5)");
		WoemReportError();
		return FALSE;
	}

	PacketAllocatePacketH = (PacketAllocatePacketHandler) GetProcAddress(PacketLib, "PacketAllocatePacket");
	if(!PacketAllocatePacketH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 6)");
		WoemReportError();
		return FALSE;
	}

	PacketFreePacketH = (PacketFreePacketHandler) GetProcAddress(PacketLib, "PacketFreePacket");
	if(!PacketFreePacketH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 7)");
		WoemReportError();
		return FALSE;
	}

	PacketInitPacketH = (PacketInitPacketHandler) GetProcAddress(PacketLib, "PacketInitPacket");
	if(!PacketInitPacketH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 8)");
		WoemReportError();
		return FALSE;
	}

	PacketReceivePacketH = (PacketReceivePacketHandler) GetProcAddress(PacketLib, "PacketReceivePacket");
	if(!PacketReceivePacketH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 9)");
		WoemReportError();
		return FALSE;
	}

	PacketSendPacketH = (PacketSendPacketHandler) GetProcAddress(PacketLib, "PacketSendPacket");
	if(!PacketSendPacketH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 10)");
		WoemReportError();
		return FALSE;
	}

	PacketSendPacketsH = (PacketSendPacketsHandler) GetProcAddress(PacketLib, "PacketSendPackets");
	if(!PacketSendPacketsH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 11)");
		WoemReportError();
		return FALSE;
	}

	PacketSetMinToCopyH = (PacketSetMinToCopyHandler) GetProcAddress(PacketLib, "PacketSetMinToCopy");
	if(!PacketSetMinToCopyH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 12)");
		WoemReportError();
		return FALSE;
	}

	PacketSetModeH = (PacketSetModeHandler) GetProcAddress(PacketLib, "PacketSetMode");
	if(!PacketSetModeH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 12a)");
		WoemReportError();
		return FALSE;
	}

	PacketSetDumpNameH = (PacketSetDumpNameHandler) GetProcAddress(PacketLib, "PacketSetDumpName");
	if(!PacketSetDumpNameH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 13)");
		WoemReportError();
		return FALSE;
	}

	PacketSetDumpLimitsH = (PacketSetDumpLimitsHandler) GetProcAddress(PacketLib, "PacketSetDumpLimits");
	if(!PacketSetDumpLimitsH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 14)");
		WoemReportError();
		return FALSE;
	}

	PacketIsDumpEndedH = (PacketIsDumpEndedHandler) GetProcAddress(PacketLib, "PacketIsDumpEnded");
	if(!PacketIsDumpEndedH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 15)");
		WoemReportError();
		return FALSE;
	}

	PacketGetReadEventH = (PacketGetReadEventHandler) GetProcAddress(PacketLib, "PacketGetReadEvent");
	if(!PacketGetReadEventH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 16)");
		WoemReportError();
		return FALSE;
	}

	PacketSetNumWritesH = (PacketSetNumWritesHandler) GetProcAddress(PacketLib, "PacketSetNumWrites");
	if(!PacketSetNumWritesH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 17)");
		WoemReportError();
		return FALSE;
	}

	PacketSetReadTimeoutH = (PacketSetReadTimeoutHandler) GetProcAddress(PacketLib, "PacketSetReadTimeout");
	if(!PacketSetReadTimeoutH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 18)");
		WoemReportError();
		return FALSE;
	}

	PacketSetBuffH = (PacketSetBuffHandler) GetProcAddress(PacketLib, "PacketSetBuff");
	if(!PacketSetBuffH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 19)");
		WoemReportError();
		return FALSE;
	}

	PacketSetBpfH = (PacketSetBpfHandler) GetProcAddress(PacketLib, "PacketSetBpf");
	if(!PacketSetBpfH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 20)");
		WoemReportError();
		return FALSE;
	}

	PacketSetSnapLenH = (PacketSetSnapLenHandler) GetProcAddress(PacketLib, "PacketSetSnapLen");
	if(!PacketSetSnapLenH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 21)");
		WoemReportError();
		return FALSE;
	}

	PacketGetStatsH = (PacketGetStatsHandler) GetProcAddress(PacketLib, "PacketGetStats");
	if(!PacketGetStatsH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 22)");
		WoemReportError();
		return FALSE;
	}

	PacketGetStatsExH = (PacketGetStatsExHandler) GetProcAddress(PacketLib, "PacketGetStatsEx");
	if(!PacketGetStatsExH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 23)");
		WoemReportError();
		return FALSE;
	}

	PacketRequestH = (PacketRequestHandler) GetProcAddress(PacketLib, "PacketRequest");
	if(!PacketRequestH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 24)");
		WoemReportError();
		return FALSE;
	}

	PacketSetHwFilterH = (PacketSetHwFilterHandler) GetProcAddress(PacketLib, "PacketSetHwFilter");
	if(!PacketSetHwFilterH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 25)");
		WoemReportError();
		return FALSE;
	}

	PacketGetAdapterNamesH = (PacketGetAdapterNamesHandler) GetProcAddress(PacketLib, "PacketGetAdapterNames");
	if(!PacketGetAdapterNamesH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 26)");
		WoemReportError();
		return FALSE;
	}

	PacketGetNetInfoExH = (PacketGetNetInfoExHandler) GetProcAddress(PacketLib, "PacketGetNetInfoEx");
	if(!PacketGetNetInfoExH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 27)");
		WoemReportError();
		return FALSE;
	}

	PacketGetNetTypeH = (PacketGetNetTypeHandler) GetProcAddress(PacketLib, "PacketGetNetType");		
	if(!PacketGetNetTypeH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 28)");
		WoemReportError();
		return FALSE;
	}

#ifdef STATIC_LIB
	PacketRegWoemLeaveHandlerH = (PacketRegWoemLeaveHandlerHandler) GetProcAddress(PacketLib, "PacketRegWoemLeaveHandler");		
	if(!PacketRegWoemLeaveHandlerH)
	{
		_snprintf(LastWoemError, PACKET_ERRSTR_SIZE - 1, "Unable to load Packet.dll (internal error 29)");
		WoemReportError();
		return FALSE;
	}
#endif
	
	return TRUE;
}

#ifdef STATIC_LIB
////////////////////////////////////////////////////////////////////
// Register an unload handler that will be called by packet.dll 
// DllMain.
////////////////////////////////////////////////////////////////////
void RegisterPacketUnloadHandler(void* Handler)
{
	PacketRegWoemLeaveHandlerH(Handler);
}
#endif // STATIC_LIB

////////////////////////////////////////////////////////////////////
// Unload the packet.dll binary we loaded on startup, and delete
// the file on disk.
////////////////////////////////////////////////////////////////////
void DeleteDll(char *DllFileName)
{
	//	BOOL FRes;
	
	//
	// We don't check return values because in every case we want to go on with cleanup
	//

	//	FRes = FreeLibrary(PacketLib);
	
	//
	// We cannot delete the library immediately, because we're using it. Therefore, we delay 
	// its deletion to the next reboot.
	//
	MoveFileEx(
		DllFileName,
		NULL,
		MOVEFILE_DELAY_UNTIL_REBOOT);
}
