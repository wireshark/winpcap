////////////////////////////////////////////////////////////////////
// Constants and names
////////////////////////////////////////////////////////////////////

#define MUTEX_NAME "WinPcapOEM-MutexProtectMultipleInstances"
#define SEMAPHORE_NAME "WinPcapOEM-SemaphoreProtectMultipleInstances"
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
BOOL LoadPacketDll(char *PacketDllFileName);
void DeleteDll(char *DllFileName);
BOOL WoemEnterDll(HINSTANCE DllHandle);
BOOL WoemLeaveDll();
#ifdef STATIC_LIB
void RegisterPacketUnloadHandler(void* Handler);
#endif // STATIC_LIB

////////////////////////////////////////////////////////////////////
// Resources handling prototypes
////////////////////////////////////////////////////////////////////
BOOL WoemSaveResourceToDisk(HINSTANCE hInst, int ResID, char* FileName);

////////////////////////////////////////////////////////////////////
// Registry and names-related functions
////////////////////////////////////////////////////////////////////
BOOL WoemCreateNameRegistryEntries();
BOOL WoemCreateBinaryNames();

////////////////////////////////////////////////////////////////////
// Debug definitions
////////////////////////////////////////////////////////////////////

//#define DEBUGTRACE
//#define TRACE_OUTPUTDEBUGSTRING

#ifdef DEBUGTRACE
#define TracePrint printf
#define WoemReportError() MessageBox(NULL, LastWoemError, "WinPcap OEM error", MB_OK)
#else
#define TracePrint
#define WoemReportError()
#endif // DEBUGTRACE

#ifdef TRACE_MBOXES
#define TraceEnter(X) MessageBox(NULL, X, "WinPcap OEM trace", MB_OK)
#else
#define TraceEnter(X)
#endif

#ifdef TRACE_OUTPUTDEBUGSTRING
#define TraceEnter(X) OutputDebugString(X)
#else
#define TraceEnter(X)
#endif
