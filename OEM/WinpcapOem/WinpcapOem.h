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
BOOL WoemSaveResourceToDisk(HINSTANCE hInst, int ResID, char* FileName, BOOL bDisableFsRedirector);
BOOL WoemGetResource(HINSTANCE hInst, int ResID, LPVOID * lpResMem, LPDWORD dwResSize);

////////////////////////////////////////////////////////////////////
// Definitions of the Win32 functions to disable/enable FS 
// redirectors on WOW64. We need these prototypes because we load
// the functions dynamically.
////////////////////////////////////////////////////////////////////
typedef WINBASEAPI BOOL (WINAPI *Wow64DisableWow64FsRedirectionHandler)( PVOID *OldValue);
typedef WINBASEAPI BOOL (WINAPI *Wow64RevertWow64FsRedirectionHandler)( PVOID OlValue);

////////////////////////////////////////////////////////////////////
// Function to delete the driver binary, taking care of running
// on win32 or WOW64
////////////////////////////////////////////////////////////////////
BOOL WoemDeleteDriverBinary(char* FileName, BOOL bDisableFsRedirector);

////////////////////////////////////////////////////////////////////
// Registry and names-related functions
////////////////////////////////////////////////////////////////////
//BOOL WoemCreateNameRegistryEntries();
//BOOL WoemDeleteNameRegistryEntries();
BOOL WoemCreateBinaryNames();

////////////////////////////////////////////////////////////////////
// Functions to register the open handles to NPF in a global list.
// This hack is needed to close all the handles before unloading the 
// driver on DllUnload
////////////////////////////////////////////////////////////////////
typedef struct _ADAPTER *LPADAPTER;

BOOL RegisterPacketHandleForDllUnloadHandlesSweep(LPADAPTER pAdapter);
VOID DeregisterPacketHandleForDllUnloadHandlesSweep(LPADAPTER pAdapter);



#endif //__WINPCAP_OEM_H_E4C69242_4757_4139_A7E4_CA06F37A5B73
