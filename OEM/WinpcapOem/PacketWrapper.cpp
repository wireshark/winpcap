#include <stdio.h>
#include "Packet32.h"
#include "WinpcapOem.h"
#include "WoemDebug.h"

volatile BOOL g_StillToInit = TRUE;
volatile BOOL g_OemActive = FALSE;

HMODULE	g_PacketLib;

__inline BOOL WoemInitialize(HINSTANCE hDllHandle);


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
	PCHAR returnValue;

	TRACE_ENTER("PacketGetVersion");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		returnValue = "<unknown>";
	}
	else
	{
		returnValue = PacketGetVersionH();
	}

	TRACE_EXIT("PacketGetVersion");
	return returnValue;
}

PCHAR PacketGetDriverVersion()
{
	PCHAR returnValue;

	TRACE_ENTER("PacketGetDriverVersion");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		returnValue = "<unknown>";
	}
	else
	{
		//
		// We don't have the driver binary, but we are sure that driver and dll 
		// versions are identical
		//	
		returnValue = PacketGetVersionH();
	}

	TRACE_EXIT("PacketGetDriverVersion");
	return returnValue;
}

BOOL PacketStopDriver()
{
	BOOL returnValue;
	
	TRACE_ENTER("PacketStopDriver");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		returnValue = FALSE;
		SetLastError(ERROR_INVALID_FUNCTION);
	}
	else
	{
		returnValue = PacketStopDriverH();
	}

	TRACE_EXIT("PacketStopDriver");

	return returnValue;
}

LPADAPTER PacketOpenAdapter(PCHAR AdapterName)
{
	LPADAPTER returnValue;
	
	TRACE_ENTER("PacketOpenAdapter");

	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = NULL;
	}
	else
	{
		returnValue = PacketOpenAdapterH(AdapterName);
	}

	TRACE_EXIT("PacketOpenAdapter");
	return returnValue;
}

VOID PacketCloseAdapter(LPADAPTER lpAdapter)
{
	TRACE_ENTER("PacketCloseAdapter");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
	}
	else
	{
		PacketCloseAdapterH(lpAdapter);
	}
	
	TRACE_EXIT("PacketCloseAdapter");
}

LPPACKET PacketAllocatePacket(void)
{
	LPPACKET returnValue;
	
	TRACE_ENTER("PacketAllocatePacket");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		returnValue = NULL;
		SetLastError(ERROR_INVALID_FUNCTION);
	}
	else
	{
		returnValue = PacketAllocatePacketH();
	}
	
	TRACE_EXIT("PacketAllocatePacket");
	return returnValue;
}

VOID PacketFreePacket(LPPACKET lpPacket)
{
	TRACE_ENTER("PacketFreePacket");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
	}
	else
	{	
		PacketFreePacketH(lpPacket);
	}
	
	TRACE_EXIT("PacketFreePacket");
}

VOID PacketInitPacket(LPPACKET lpPacket,PVOID Buffer,UINT Length)
{
	TRACE_ENTER("PacketInitPacket");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
	}
	else
	{
		PacketInitPacketH(lpPacket, Buffer, Length);
	}
	
	TRACE_EXIT("PacketInitPacket");
}

BOOLEAN PacketReceivePacket(LPADAPTER AdapterObject,LPPACKET lpPacket,BOOLEAN Sync)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketReceivePacket");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketReceivePacketH(AdapterObject, lpPacket, Sync);
	}
	
	TRACE_EXIT("PacketReceivePacket");
	return returnValue;
}

BOOLEAN PacketSendPacket(LPADAPTER AdapterObject,LPPACKET lpPacket,BOOLEAN Sync)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketSendPacket");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketSendPacketH(AdapterObject, lpPacket, Sync);
	}
	
	TRACE_EXIT("PacketSendPacket");
	return returnValue;
}

INT PacketSendPackets(LPADAPTER AdapterObject, PVOID PacketBuff, ULONG Size, BOOLEAN Sync)
{
	INT returnValue;

	TRACE_ENTER("PacketSendPackets");

	//
	// Check if we are the first instance and Init everything accordingly
	//
		if(!WoemInitialize(g_DllHandle))
		{
			SetLastError(ERROR_INVALID_FUNCTION);
			returnValue = 0;
		}
else
{
	returnValue = PacketSendPacketsH(AdapterObject, PacketBuff, Size, Sync);
}

	TRACE_EXIT("PacketSendPackets");
	return returnValue;
}

BOOLEAN PacketSetMinToCopy(LPADAPTER AdapterObject,int nbytes)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketSetMinToCopy");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketSetMinToCopyH(AdapterObject, nbytes);
	}
	
	TRACE_EXIT("PacketSetMinToCopy");
	return returnValue;
}

BOOLEAN PacketSetMode(LPADAPTER AdapterObject,int mode)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketSetMode");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketSetModeH(AdapterObject, mode);
	}
	
	TRACE_EXIT("PacketSetMode");
	return returnValue;
}

BOOLEAN PacketSetDumpName(LPADAPTER AdapterObject, void *name, int len)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketSetDumpName");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketSetDumpNameH(AdapterObject, name, len);
	}
	
	TRACE_EXIT("PacketSetDumpName");
	return returnValue;
}

BOOLEAN PacketSetDumpLimits(LPADAPTER AdapterObject, UINT maxfilesize, UINT maxnpacks)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketSetDumpLimits");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketSetDumpLimitsH(AdapterObject, maxfilesize, maxnpacks);
	}
	
	TRACE_EXIT("PacketSetDumpLimits");
	return returnValue;
}

BOOLEAN PacketIsDumpEnded(LPADAPTER AdapterObject, BOOLEAN sync)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketIsDumpEnded");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketIsDumpEndedH(AdapterObject, sync);
	}
	
	TRACE_EXIT("PacketIsDumpEnded");
	return returnValue;
}

HANDLE PacketGetReadEvent(LPADAPTER AdapterObject)
{
	HANDLE returnValue;
	
	TRACE_ENTER("PacketGetReadEvent");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = NULL;
	}
	else
	{
		returnValue = PacketGetReadEventH(AdapterObject);
	}
	
	TRACE_EXIT("PacketGetReadEvent");
	return returnValue;
}

BOOLEAN PacketSetNumWrites(LPADAPTER AdapterObject,int nwrites)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketSetNumWrites");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketSetNumWritesH(AdapterObject, nwrites);
	}
	
	TRACE_EXIT("PacketSetNumWrites");
	return returnValue;
}

BOOLEAN PacketSetReadTimeout(LPADAPTER AdapterObject,int timeout)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketSetReadTimeout");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketSetReadTimeoutH(AdapterObject, timeout);
	}
	
	TRACE_EXIT("PacketSetReadTimeout");
	return returnValue;
}

BOOLEAN PacketSetBuff(LPADAPTER AdapterObject,int dim)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketSetBuff");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketSetBuffH(AdapterObject, dim);
	}
	
	TRACE_EXIT("PacketSetBuff");
	return returnValue;
}

BOOLEAN PacketSetBpf(LPADAPTER AdapterObject, struct bpf_program *fp)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketSetBpf");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketSetBpfH(AdapterObject, fp);
	}
	
	TRACE_EXIT("PacketSetBpf");
	return returnValue;
}

BOOLEAN PacketSetLoopbackBehavior(LPADAPTER  AdapterObject, UINT LoopbackBehavior)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketSetLoopbackBehavior");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketSetLoopbackBehaviorH(AdapterObject, LoopbackBehavior);
	}
	
	TRACE_EXIT("PacketSetLoopbackBehavior");
	return returnValue;
}

INT PacketSetSnapLen(LPADAPTER AdapterObject, int snaplen)
{
	INT returnValue;
	
	TRACE_ENTER("PacketSetSnapLen");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = 0;
	}
	else
	{
		returnValue = PacketSetSnapLenH(AdapterObject, snaplen);
	}
	
	TRACE_EXIT("PacketSetSnapLen");
	return returnValue;
}

BOOLEAN PacketGetStats(LPADAPTER AdapterObject,struct bpf_stat *s)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketGetStats");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		returnValue = FALSE;
		SetLastError(ERROR_INVALID_FUNCTION);
	}
	else
	{
		returnValue = PacketGetStatsH(AdapterObject, s);
	}
	
	TRACE_EXIT("PacketGetStats");
	return returnValue;
}

BOOLEAN PacketGetStatsEx(LPADAPTER AdapterObject,struct bpf_stat *s)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketGetStatsEx");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		returnValue = FALSE;
		SetLastError(ERROR_INVALID_FUNCTION);
	}
	else
	{
		returnValue = PacketGetStatsExH(AdapterObject, s);
	}
	
	TRACE_EXIT("PacketGetStatsEx");
	return returnValue;
}

BOOLEAN PacketRequest(LPADAPTER  AdapterObject,BOOLEAN Set,PPACKET_OID_DATA  OidData)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketRequest");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		returnValue = FALSE;
		SetLastError(ERROR_INVALID_FUNCTION);
	}
	else
	{
		returnValue = PacketRequestH(AdapterObject, Set, OidData);
	}
	
	TRACE_EXIT("PacketRequest");
	return returnValue;
}

BOOLEAN PacketSetHwFilter(LPADAPTER  AdapterObject,ULONG Filter)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketSetHwFilter");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		returnValue = FALSE;
		SetLastError(ERROR_INVALID_FUNCTION);
	}
	else
	{
		returnValue = PacketSetHwFilterH(AdapterObject, Filter);
	}
	
	TRACE_EXIT("PacketSetHwFilter");
	return returnValue;
}

BOOLEAN PacketGetAdapterNames(PTSTR pStr,PULONG  BufferSize)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketGetAdapterNames");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		*BufferSize = 0;
		SetLastError(ERROR_INVALID_FUNCTION);
			
		TRACE_EXIT("PacketGetAdapterNames");
		return FALSE;
	}
	else
	{
		returnValue = PacketGetAdapterNamesH(pStr,  BufferSize);
	}
	
	TRACE_EXIT("PacketGetAdapterNames");
	return returnValue;
}

BOOLEAN PacketGetNetInfoEx(PCHAR AdapterName, npf_if_addr* buffer, PLONG NEntries)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketGetNetInfoEx");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketGetNetInfoExH(AdapterName, buffer, NEntries);
	}
	
	TRACE_EXIT("PacketGetNetInfoEx");
	return returnValue;
}

BOOLEAN PacketGetNetType(LPADAPTER AdapterObject, NetType *type)
{
	BOOLEAN returnValue;
	
	TRACE_ENTER("PacketGetNetType");
	
	//
	// Check if we are the first instance and Init everything accordingly
	//
	if(!WoemInitialize(g_DllHandle))
	{
		SetLastError(ERROR_INVALID_FUNCTION);
		returnValue = FALSE;
	}
	else
	{
		returnValue = PacketGetNetTypeH(AdapterObject, type);
	}
	
	TRACE_EXIT("PacketGetNetType");
	return returnValue;
}

//---------------------------------------------------------------------------
// ADDITIONAL EXPORTS NOT PRESENT IN STANDARD PACKET.DLL
//---------------------------------------------------------------------------

// This public function enables winpcap oem
BOOLEAN PacketStartOem(PCHAR errorString, UINT errorStringLength)
{
	CHAR internalErrorString[PACKET_ERRSTR_SIZE];
	
	TRACE_ENTER("PacketStartOem");

	
	if (WoemEnterDll(::g_DllHandle, internalErrorString) == TRUE)
	{
		g_OemActive = TRUE;
		TRACE_EXIT("PacketStartOem");
		return TRUE;
	}
	else
	{
		errorString[errorStringLength - 1] = '\0';

		strncpy(errorString, internalErrorString, errorStringLength - 1);

		TRACE_EXIT("PacketStartOem");
		return FALSE;
	}
}

//---------------------------------------------------------------------------
// FUNCTIONS
//---------------------------------------------------------------------------


__inline BOOL WoemInitialize(HINSTANCE hDllHandle)
{
	BOOL returnValue;
	char errorString[PACKET_ERRSTR_SIZE];

	TRACE_ENTER("WoemInitialize");
	
	if (g_StillToInit == FALSE)
	{
		TRACE_EXIT("WoemInitialize");
		return TRUE;
	}

	if (g_OemActive == FALSE)
	{
		TRACE_MESSAGE("OEM WinPcap not active");

		TRACE_EXIT("WoemInitialize");
		return FALSE;
	}

	// NOTE: this  function changes the value of g_StillInit!!
	// we cannot do it here because we need to change this value
	// while holding the global lock!
	returnValue = WoemEnterDll(hDllHandle, errorString);

	TRACE_EXIT("WoemInitialize");

	return returnValue;
}	



#define LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR(_msg) do{strncpy(errorString, _msg, PACKET_ERRSTR_SIZE - 1); TRACE_MESSAGE(_msg);}while(0)


////////////////////////////////////////////////////////////////////
// Load the packet.dll binary with the specifed name
////////////////////////////////////////////////////////////////////
BOOL LoadPacketDll(char *PacketDllFileName, char *errorString)
{	
	TRACE_ENTER("LoadPacketDll");
	
	errorString[PACKET_ERRSTR_SIZE - 1] = '\0';
	// 
	// Dinamically load the packet.dll library with loadlibrary
	//
	if((g_PacketLib =  LoadLibrary(PacketDllFileName)) == NULL)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to initialize the Packet.dll dynamic library");

		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	// 
	// Get the entry points of the just loaded library
	//
	PacketGetVersionH = (PacketGetVersionHandler) GetProcAddress(g_PacketLib, "PacketGetVersion");	
	if(!PacketGetVersionH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 1)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketGetDriverVersionH = (PacketGetDriverVersionHandler) GetProcAddress(g_PacketLib, "PacketGetDriverVersion");
	if(!PacketGetDriverVersionH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 2)");

		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketStopDriverH = (PacketStopDriverHandler) GetProcAddress(g_PacketLib, "PacketStopDriver");
	if(!PacketStopDriverH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 3)");

		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketOpenAdapterH = (PacketOpenAdapterHandler) GetProcAddress(g_PacketLib, "PacketOpenAdapter");
	if(!PacketOpenAdapterH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 4)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketCloseAdapterH = (PacketCloseAdapterHandler) GetProcAddress(g_PacketLib, "PacketCloseAdapter");
	if(!PacketCloseAdapterH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 5)");

		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketAllocatePacketH = (PacketAllocatePacketHandler) GetProcAddress(g_PacketLib, "PacketAllocatePacket");
	if(!PacketAllocatePacketH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 6)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketFreePacketH = (PacketFreePacketHandler) GetProcAddress(g_PacketLib, "PacketFreePacket");
	if(!PacketFreePacketH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 7)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketInitPacketH = (PacketInitPacketHandler) GetProcAddress(g_PacketLib, "PacketInitPacket");
	if(!PacketInitPacketH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 8)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketReceivePacketH = (PacketReceivePacketHandler) GetProcAddress(g_PacketLib, "PacketReceivePacket");
	if(!PacketReceivePacketH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 9)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSendPacketH = (PacketSendPacketHandler) GetProcAddress(g_PacketLib, "PacketSendPacket");
	if(!PacketSendPacketH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 10)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSendPacketsH = (PacketSendPacketsHandler) GetProcAddress(g_PacketLib, "PacketSendPackets");
	if(!PacketSendPacketsH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 11)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSetMinToCopyH = (PacketSetMinToCopyHandler) GetProcAddress(g_PacketLib, "PacketSetMinToCopy");
	if(!PacketSetMinToCopyH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 12)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSetModeH = (PacketSetModeHandler) GetProcAddress(g_PacketLib, "PacketSetMode");
	if(!PacketSetModeH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 12a)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSetDumpNameH = (PacketSetDumpNameHandler) GetProcAddress(g_PacketLib, "PacketSetDumpName");
	if(!PacketSetDumpNameH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 13)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSetDumpLimitsH = (PacketSetDumpLimitsHandler) GetProcAddress(g_PacketLib, "PacketSetDumpLimits");
	if(!PacketSetDumpLimitsH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 14)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketIsDumpEndedH = (PacketIsDumpEndedHandler) GetProcAddress(g_PacketLib, "PacketIsDumpEnded");
	if(!PacketIsDumpEndedH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 15)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketGetReadEventH = (PacketGetReadEventHandler) GetProcAddress(g_PacketLib, "PacketGetReadEvent");
	if(!PacketGetReadEventH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 16)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSetNumWritesH = (PacketSetNumWritesHandler) GetProcAddress(g_PacketLib, "PacketSetNumWrites");
	if(!PacketSetNumWritesH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 17)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSetReadTimeoutH = (PacketSetReadTimeoutHandler) GetProcAddress(g_PacketLib, "PacketSetReadTimeout");
	if(!PacketSetReadTimeoutH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 18)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSetBuffH = (PacketSetBuffHandler) GetProcAddress(g_PacketLib, "PacketSetBuff");
	if(!PacketSetBuffH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 19)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSetBpfH = (PacketSetBpfHandler) GetProcAddress(g_PacketLib, "PacketSetBpf");
	if(!PacketSetBpfH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 20)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSetSnapLenH = (PacketSetSnapLenHandler) GetProcAddress(g_PacketLib, "PacketSetSnapLen");
	if(!PacketSetSnapLenH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 21)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketGetStatsH = (PacketGetStatsHandler) GetProcAddress(g_PacketLib, "PacketGetStats");
	if(!PacketGetStatsH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 22)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketGetStatsExH = (PacketGetStatsExHandler) GetProcAddress(g_PacketLib, "PacketGetStatsEx");
	if(!PacketGetStatsExH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 23)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketRequestH = (PacketRequestHandler) GetProcAddress(g_PacketLib, "PacketRequest");
	if(!PacketRequestH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 24)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSetHwFilterH = (PacketSetHwFilterHandler) GetProcAddress(g_PacketLib, "PacketSetHwFilter");
	if(!PacketSetHwFilterH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 25)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketGetAdapterNamesH = (PacketGetAdapterNamesHandler) GetProcAddress(g_PacketLib, "PacketGetAdapterNames");
	if(!PacketGetAdapterNamesH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 26)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketGetNetInfoExH = (PacketGetNetInfoExHandler) GetProcAddress(g_PacketLib, "PacketGetNetInfoEx");
	if(!PacketGetNetInfoExH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 27)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketGetNetTypeH = (PacketGetNetTypeHandler) GetProcAddress(g_PacketLib, "PacketGetNetType");		
	if(!PacketGetNetTypeH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 28)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

	PacketSetLoopbackBehaviorH = (PacketSetLoopbackBehaviorHandler) GetProcAddress(g_PacketLib, "PacketSetLoopbackBehavior");		
	if(!PacketSetLoopbackBehaviorH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 29)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}

#ifdef STATIC_LIB
	PacketRegWoemLeaveHandlerH = (PacketRegWoemLeaveHandlerHandler) GetProcAddress(g_PacketLib, "PacketRegWoemLeaveHandler");		
	if(!PacketRegWoemLeaveHandlerH)
	{
		LOAD_PACKET_DLL_TRACE_AND_COPY_ERROR("Unable to load Packet.dll (internal error 30)");
		
		TRACE_EXIT("LoadPacketDll");
		return FALSE;
	}
#endif
	
	TRACE_EXIT("LoadPacketDll");
	return TRUE;
}

#ifdef STATIC_LIB
////////////////////////////////////////////////////////////////////
// Register an unload handler that will be called by packet.dll 
// DllMain.
////////////////////////////////////////////////////////////////////
void RegisterPacketUnloadHandler(void* Handler)
{
	TRACE_ENTER("RegisterPacketUnloadHandler");
	PacketRegWoemLeaveHandlerH(Handler);
	TRACE_EXIT("RegisterPacketUnloadHandler");

}
#endif // STATIC_LIB

////////////////////////////////////////////////////////////////////
// Unload the packet.dll binary we loaded on startup, and delete
// the file on disk.
////////////////////////////////////////////////////////////////////
void DeleteDll(char *DllFileName)
{
	TRACE_ENTER("RegisterPacketUnloadHandler");
	//	BOOL FRes;
	
	//
	// We don't check return values because in every case we want to go on with cleanup
	//

	//	FRes = FreeLibrary(g_g_PacketLib);
	
	//
	// We cannot delete the library immediately, because we're using it. Therefore, we delay 
	// its deletion to the next reboot.
	//
	MoveFileEx(
		DllFileName,
		NULL,
		MOVEFILE_DELAY_UNTIL_REBOOT);

	TRACE_EXIT("RegisterPacketUnloadHandler");
}
