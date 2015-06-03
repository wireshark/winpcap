/*
 * Copyright 2015 Riverbed Technology, San Francisco (California).
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
 * 3. Neither the name of Riverbed Technology nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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
int start_service(LPCTSTR ServiceName, char *ErrorString, size_t Length);
int create_driver_service(LPCTSTR ServiceName, LPCTSTR ServiceDescription, LPCTSTR ServicePath, char* ErrorString, size_t Length);
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
BOOL WoemEnterDll(HINSTANCE DllHandle, char *WoemErrorString, ULONG flags);
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
