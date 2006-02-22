#ifndef __WINPCAP_OEM_H_E4C69242_4757_4139_A7E4_CA06F37A5B73
#define __WINPCAP_OEM_H_E4C69242_4757_4139_A7E4_CA06F37A5B73



////////////////////////////////////////////////////////////////////
// Constants and names
////////////////////////////////////////////////////////////////////

#define MUTEX_NAME "WinPcapOEM-MutexProtectMultipleInstances"
#define SEMAPHORE_NAME "WinPcapOEM-SemaphoreProtectMultipleInstances-" NPF_DRIVER_NAME
#define MAX_VALUE_SEMAPHORE 65535
#define MAX_OBJNAME_LEN 1024
#define PACKET_ERRSTR_SIZE 1024

////////////////////////////////////////////////////////////////////
// Services-related prototypes
////////////////////////////////////////////////////////////////////
int delete_service(LPCTSTR ServiceName);
int stop_service(LPCTSTR ServiceName);
int start_service(LPCTSTR ServiceName);
int create_driver_service(LPCTSTR ServiceName, LPCTSTR ServiceDescription, LPCTSTR ServicePath);
int check_if_service_is_running(LPCTSTR ServiceName);
int check_if_service_is_present(LPCTSTR ServiceName);
int change_start_type_service(LPCTSTR ServiceName, DWORD StartType);
HRESULT	HrInstallNetMonProtocol();
HRESULT HrUninstallNetMonProtocol();

////////////////////////////////////////////////////////////////////
// Packet.dll wrapper prototypes
////////////////////////////////////////////////////////////////////
BOOL LoadPacketDll(char *PacketDllFileName, char *errorString);
void DeleteDll(char *DllFileName);
BOOL WoemEnterDll(HINSTANCE DllHandle, char *WoemErrorString);
BOOL WoemLeaveDll();
#ifdef STATIC_LIB
void RegisterPacketUnloadHandler(void* Handler);
#endif // STATIC_LIB

////////////////////////////////////////////////////////////////////
// Global variables
///////////////////////////////////////////////////////////////////
extern HINSTANCE g_DllHandle;
extern char g_LastWoemError[];
volatile extern BOOL g_InitError;
volatile extern BOOL g_OemActive;
volatile extern BOOL g_StillToInit;


////////////////////////////////////////////////////////////////////
// Resources handling prototypes
////////////////////////////////////////////////////////////////////
BOOL WoemSaveResourceToDisk(HINSTANCE hInst, int ResID, char* FileName);
BOOL WoemGetResource(HINSTANCE hInst, int ResID, LPVOID * lpResMem, LPDWORD dwResSize);

////////////////////////////////////////////////////////////////////
// Registry and names-related functions
////////////////////////////////////////////////////////////////////
//BOOL WoemCreateNameRegistryEntries();
//BOOL WoemDeleteNameRegistryEntries();
BOOL WoemCreateBinaryNames();

////////////////////////////////////////////////////////////////////
// Debug definitions
////////////////////////////////////////////////////////////////////

#if 0

#define DEBUGTRACE
#define TRACE_OUTPUTDEBUGSTRING

#ifdef DEBUGTRACE
#define TracePrint printf
#define WoemReportError() do{MessageBox(NULL, g_LastWoemError, "OEM WinPcap error", MB_OK); g_InitError = TRUE;} while(0)
#else
#define TracePrint
#define WoemReportError() do{g_InitError = TRUE;}while(0)
#endif // DEBUGTRACE

#ifdef TRACE_MBOXES
#define TraceEnter(X) MessageBox(NULL, X, "OEM WinPcap trace", MB_OK)
#elif defined TRACE_OUTPUTDEBUGSTRING
#define TraceEnter(X) OutputDebugString(X)
#else
#define TraceEnter(X)
#endif

#endif

////////////////////////////////////////////////////////////////////
// Error codes
////////////////////////////////////////////////////////////////////


#endif //__WINPCAP_OEM_H_E4C69242_4757_4139_A7E4_CA06F37A5B73
