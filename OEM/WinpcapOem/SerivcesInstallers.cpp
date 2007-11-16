/*
 * Copyright (c) 2003 - 2005 NetGroup, Politecnico di Torino (Italy).
 * Copyright (c) 2005 CACE Technologies, Davis (California).
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
 * 3. Neither the name of the Politecnico di Torino, CACE Technologies
 * nor the names of its contributors may be used to endorse or promote 
 * products derived from this software without specific prior written 
 * permission.
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

//+---------------------------------------------------------------------------
//
// This file includes code shipped with (c) Microsoft Windows DDK (build 2600)
// Copyright (C) Microsoft Corporation, 1997.
//
//----------------------------------------------------------------------------

#define _WIN32_DCOM 

#include <tchar.h>
#include <winsock2.h>

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <lmerr.h>
#include <aclapi.h>

#include <crtdbg.h>
#include <eh.h>

#include <netmon.h>


#include "pch.h"
#pragma hdrstop
#include "NetMonInstaller.h"
//#define DEBUGTRACE
#include "WinpcapOem.h"
#include "WoemDebug.h"

//----------------------------------------------------------------------------
// Globals
//
static const GUID* c_aguidClass[] =
{
    &GUID_DEVCLASS_NET,
    &GUID_DEVCLASS_NETTRANS,
    &GUID_DEVCLASS_NETSERVICE,
    &GUID_DEVCLASS_NETCLIENT
};

//----------------------------------------------------------------------------
// Prototypes of helper functions
//
HRESULT HrInstallNetComponent(IN INetCfg* pnc,
                              IN PCWSTR szComponentId,
                              IN const GUID* pguidClass);

HRESULT HrUninstallNetComponent(IN INetCfg* pnc, IN PCWSTR szComponentId);
HRESULT HrGetINetCfg(IN BOOL fGetWriteLock, INetCfg** ppnc);
HRESULT HrReleaseINetCfg(BOOL fHasWriteLock, INetCfg* pnc);
inline ULONG ReleaseObj(IUnknown* punk)
{
    return (punk) ? punk->Release () : 0;
}

void DisplayErrorText(DWORD dwLastError);


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//                    Netmon-related functions                       //
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

//+---------------------------------------------------------------------------
//
// Function:  HrIsComponentInstalled
//
// Purpose:   Find out if a component is installed
//
// Arguments:
//    szComponentId [in]  id of component to search
//
// Returns:   S_OK    if installed,
//            S_FALSE if not installed,
//            otherwise an error code
//
// Author:    kumarp 11-February-99
//
// Notes:
//
HRESULT HrIsComponentInstalled(IN PCWSTR szComponentId)
{
    HRESULT hr=S_OK;
    INetCfg* pnc;
    INetCfgComponent* pncc;

    hr = HrGetINetCfg(FALSE, &pnc);
    if (S_OK == hr)
    {
        hr = pnc->FindComponent(szComponentId, &pncc);
        if (S_OK == hr)
        {
            ReleaseObj(pncc);
        }
        (void) HrReleaseINetCfg(FALSE, pnc);
    }

    return hr;
}

//+---------------------------------------------------------------------------
//
// Function:  HrInstallNetComponent
//
// Purpose:   Install the specified net component
//
// Arguments:
//    szComponentId [in]  component to install
//    nc            [in]  class of the component
//    szInfFullPath [in]  full path to primary INF file
//                        required if the primary INF and other
//                        associated files are not pre-copied to
//                        the right destination dirs.
//                        Not required when installing MS components
//                        since the files are pre-copied by
//                        Windows NT Setup.
//
// Returns:   S_OK or NETCFG_S_REBOOT on success, otherwise an error code
//
// Notes:
//

HRESULT	HrInstallNetMonProtocol()
{
	HRESULT	hr=S_OK;
	INetCfg* pnc;

	hr	= HrGetINetCfg(TRUE, &pnc);

	if (SUCCEEDED(hr))
	{
		// install szComponentId
		hr = HrInstallNetComponent(pnc,	L"MS_NetMon", &GUID_DEVCLASS_NETTRANS);
		if (SUCCEEDED(hr))
		{
			// Apply the changes
			hr = pnc->Apply();
		}

		// release INetCfg
		(void) HrReleaseINetCfg(TRUE, pnc);
	}
	return hr;
}

//+---------------------------------------------------------------------------
//
// Function:  HrInstallNetComponent
//
// Purpose:   Install the specified net component
//
// Arguments:
//    pnc           [in]  pointer to INetCfg object
//    szComponentId [in]  component to install
//    pguidClass    [in]  class guid of the component
//
// Returns:   S_OK or NETCFG_S_REBOOT on success, otherwise an error code
//
// Notes:
//
HRESULT HrInstallNetComponent(IN INetCfg* pnc,
                              IN PCWSTR szComponentId,
                              IN const GUID* pguidClass)
{
    HRESULT hr=S_OK;
    OBO_TOKEN OboToken;
    INetCfgClassSetup* pncClassSetup;
    INetCfgComponent* pncc;

    // OBO_TOKEN specifies the entity on whose behalf this
    // component is being installed

    // set it to OBO_USER so that szComponentId will be installed
    // On-Behalf-Of "user"
    ZeroMemory (&OboToken, sizeof(OboToken));
    OboToken.Type = OBO_USER;

    hr = pnc->QueryNetCfgClass (pguidClass, IID_INetCfgClassSetup,
                                (void**)&pncClassSetup);
    if (SUCCEEDED(hr))
    {
        hr = pncClassSetup->Install(szComponentId,
                                    &OboToken,
                                    NSF_POSTSYSINSTALL,
                                    0,       // <upgrade-from-build-num>
                                    NULL,    // answerfile name
                                    NULL,    // answerfile section name
                                    &pncc);
        if (S_OK == hr)
        {
            // we dont want to use pncc (INetCfgComponent), release it
            ReleaseObj(pncc);
        }

        ReleaseObj(pncClassSetup);
    }

    return hr;
}

//+---------------------------------------------------------------------------
//
// Function:  HrUninstallNetComponent
//
// Purpose:   Initialize INetCfg and uninstall a component
//
// Arguments:
//    szComponentId [in]  InfId of component to uninstall (e.g. MS_TCPIP)
//
// Returns:   S_OK or NETCFG_S_REBOOT on success, otherwise an error code
//
// Notes:
//
HRESULT HrUninstallNetMonProtocol()
{
    HRESULT hr=S_OK;
    INetCfg* pnc;

    // get INetCfg interface
    hr = HrGetINetCfg(TRUE, &pnc);

    if (SUCCEEDED(hr))
    {
        // uninstall szComponentId
        hr = HrUninstallNetComponent(pnc, L"MS_NetMon");

        if (S_OK == hr)
        {
            // Apply the changes
            hr = pnc->Apply();
        }
        // release INetCfg
        (void) HrReleaseINetCfg(TRUE, pnc);
    }

    return hr;
}

//+---------------------------------------------------------------------------
//
// Function:  HrUninstallNetComponent
//
// Purpose:   Uninstall the specified component.
//
// Arguments:
//    pnc           [in]  pointer to INetCfg object
//    szComponentId [in]  component to uninstall
//
// Returns:   S_OK or NETCFG_S_REBOOT on success, otherwise an error code
//
// Notes:
//
HRESULT HrUninstallNetComponent(IN INetCfg* pnc, IN PCWSTR szComponentId)
{
    HRESULT hr=S_OK;
    OBO_TOKEN OboToken;
    INetCfgComponent* pncc;
    GUID guidClass;
    INetCfgClass* pncClass;
    INetCfgClassSetup* pncClassSetup;

    // OBO_TOKEN specifies the entity on whose behalf this
    // component is being uninstalld

    // set it to OBO_USER so that szComponentId will be uninstalld
    // On-Behalf-Of "user"
    ZeroMemory (&OboToken, sizeof(OboToken));
    OboToken.Type = OBO_USER;

    // see if the component is really installed
    hr = pnc->FindComponent(szComponentId, &pncc);

    if (S_OK == hr)
    {
        // yes, it is installed. obtain INetCfgClassSetup and DeInstall

        hr = pncc->GetClassGuid(&guidClass);

        if (S_OK == hr)
        {
            hr = pnc->QueryNetCfgClass(&guidClass, IID_INetCfgClass,
                                       (void**)&pncClass);
            if (SUCCEEDED(hr))
            {
                hr = pncClass->QueryInterface(IID_INetCfgClassSetup,
                                              (void**)&pncClassSetup);
                    if (SUCCEEDED(hr))
                    {
                        hr = pncClassSetup->DeInstall (pncc, &OboToken, NULL);

                        ReleaseObj (pncClassSetup);
                    }
                ReleaseObj(pncClass);
            }
        }
        ReleaseObj(pncc);
    }

    return hr;
}


//+---------------------------------------------------------------------------
//
// Function:  HrGetINetCfg
//
// Purpose:   Initialize COM, create and initialize INetCfg.
//            Obtain write lock if indicated.
//
// Arguments:
//    fGetWriteLock [in]  whether to get write lock
//    ppnc          [in]  pointer to pointer to INetCfg object
//
// Returns:   S_OK on success, otherwise an error code
//
// Notes:
//
HRESULT HrGetINetCfg(IN BOOL fGetWriteLock,
                     INetCfg** ppnc)
{
    HRESULT hr=S_OK;

    // Initialize the output parameters.
    *ppnc = NULL;

    // initialize COM
    hr = CoInitializeEx(NULL,
                        COINIT_DISABLE_OLE1DDE | COINIT_APARTMENTTHREADED );

    if (SUCCEEDED(hr))
    {
        // Create the object implementing INetCfg.
        //
        INetCfg* pnc;
        hr = CoCreateInstance(CLSID_CNetCfg, NULL, CLSCTX_INPROC_SERVER,
                              IID_INetCfg, (void**)&pnc);
        if (SUCCEEDED(hr))
        {
            INetCfgLock * pncLock = NULL;
            if (fGetWriteLock)
            {
                // Get the locking interface
                hr = pnc->QueryInterface(IID_INetCfgLock,
                                         (LPVOID *)&pncLock);
                if (SUCCEEDED(hr))
                {
                    // Attempt to lock the INetCfg for read/write
                    static const ULONG c_cmsTimeout = 15000;
                    static const WCHAR c_szSampleNetcfgApp[] =
                        L"Sample Netcfg Application (netcfg.exe)";
                    PWSTR szLockedBy;

                    hr = pncLock->AcquireWriteLock(c_cmsTimeout,
                                                   c_szSampleNetcfgApp,
                                                   &szLockedBy);
                    if (S_FALSE == hr)
                    {
                        hr = NETCFG_E_NO_WRITE_LOCK;
                    }
                }
            }

            if (SUCCEEDED(hr))
            {
                // Initialize the INetCfg object.
                //
                hr = pnc->Initialize(NULL);
                if (SUCCEEDED(hr))
                {
                    *ppnc = pnc;
                    pnc->AddRef();
                }
                else
                {
                    // initialize failed, if obtained lock, release it
                    if (pncLock)
                    {
                        pncLock->ReleaseWriteLock();
                    }
                }
            }
            ReleaseObj(pncLock);
            ReleaseObj(pnc);
        }

        if (FAILED(hr))
        {
            CoUninitialize();
        }
    }

    return hr;
}

//+---------------------------------------------------------------------------
//
// Function:  HrReleaseINetCfg
//
// Purpose:   Uninitialize INetCfg, release write lock (if present)
//            and uninitialize COM.
//
// Arguments:
//    fHasWriteLock [in]  whether write lock needs to be released.
//    pnc           [in]  pointer to INetCfg object
//
// Returns:   S_OK on success, otherwise an error code
//
// Notes:
//
HRESULT HrReleaseINetCfg(BOOL fHasWriteLock, INetCfg* pnc)
{
    HRESULT hr = S_OK;

    // uninitialize INetCfg
    hr = pnc->Uninitialize();

    // if write lock is present, unlock it
    if (SUCCEEDED(hr) && fHasWriteLock)
    {
        INetCfgLock* pncLock;

        // Get the locking interface
        hr = pnc->QueryInterface(IID_INetCfgLock,
                                 (LPVOID *)&pncLock);
        if (SUCCEEDED(hr))
        {
            hr = pncLock->ReleaseWriteLock();
            ReleaseObj(pncLock);
        }
    }

    ReleaseObj(pnc);

    CoUninitialize();

    return hr;
}

#define MAX_DELAY_STOP_SERVICE		3000 //ms

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//                    NPF-related functions                          //
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
int delete_service(LPCTSTR ServiceName)
{
	SC_HANDLE SCM_Handle;
	SC_HANDLE ServiceHandle;
	SERVICE_STATUS ServiceStatus;
	ULONG delay;

	DWORD ReturnValue;
	
	SCM_Handle=OpenSCManager(NULL,  /*local machine  */
		NULL,						/*active database*/
		SC_MANAGER_ALL_ACCESS);
	
	if (SCM_Handle==NULL)
	{
		TRACE_MESSAGE("Error opening Service Control Manager: ");
		DisplayErrorText(GetLastError());
		return -1;
	}
	
	ServiceHandle=OpenService(SCM_Handle,
		ServiceName,
		SERVICE_ALL_ACCESS);
	
	
	if (ServiceHandle==NULL)
	{
		TRACE_MESSAGE("Error opening Service Control Manager: ");
		DisplayErrorText(GetLastError());
		
		if (!CloseServiceHandle(SCM_Handle))
			TRACE_MESSAGE("Error closing Service control Manager\n");
		
		return -1;
	}
	
	ReturnValue=0;
	
	if(!ControlService(ServiceHandle,SERVICE_CONTROL_STOP,&ServiceStatus))
	{
		DWORD Err=GetLastError();
		
		if (Err != ERROR_SERVICE_NOT_ACTIVE)
		{
			TRACE_MESSAGE("Error stopping service %s: ",ServiceName);
			DisplayErrorText(Err);
			ReturnValue=-1;
		}
	}
	else
		TRACE_MESSAGE("Service %s successfully stopped\n",ServiceName);
	
	//
	// We can delete the service only if it's stopped. If the service is not stopped, it will be marked for deletion,
	// which is a pain in the butt. Please see the long explanation about this problem in DllMain
	// 
	delay = 0;
	while((QueryServiceStatus(ServiceHandle, &ServiceStatus) == TRUE) && (ServiceStatus.dwCurrentState != SERVICE_STOPPED) && delay < MAX_DELAY_STOP_SERVICE)
	{
		TRACE_MESSAGE("The service status is now %u", ServiceStatus.dwCurrentState);
		Sleep(100);
		delay += 100;
	}

	//
	// If by this time the service is not stopped, just fail to delete the service
	//
	if (ServiceStatus.dwCurrentState != SERVICE_STOPPED)
	{
		TRACE_MESSAGE("The service cannot be stopped (pending handles?)");
		return -1;
	}

	if(!DeleteService(ServiceHandle))
	{
		TRACE_MESSAGE("Error deleting service %s: ",ServiceName);
		DisplayErrorText(GetLastError());
		ReturnValue=-1;
		
	}
	else
		TRACE_MESSAGE("Service %s successfully deleted\n",ServiceName);

	if(!CloseServiceHandle(ServiceHandle))
	{
		TRACE_MESSAGE("Error closing service %s: ",ServiceName);
		DisplayErrorText(GetLastError());
		ReturnValue=-1;
	}
	
	if(!CloseServiceHandle(SCM_Handle))
	{
		TRACE_MESSAGE("Error closing Service control Manager\n");
		ReturnValue=-1;
	}
	
	return ReturnValue;
	
}

int stop_service(LPCTSTR ServiceName)
{
	SC_HANDLE SCM_Handle;
	SC_HANDLE ServiceHandle;
	SERVICE_STATUS ServiceStatus;
	DWORD ReturnValue;
	
	SCM_Handle=OpenSCManager(NULL,  /*local machine  */
		NULL,						/*active database*/
		SC_MANAGER_ALL_ACCESS);
	
	if (SCM_Handle==NULL)
	{
		TRACE_MESSAGE("Error opening Service Control Manager: ");
		DisplayErrorText(GetLastError());
		
		return -1;
	}
	
	ServiceHandle=OpenService(SCM_Handle,
		ServiceName,
		SERVICE_ALL_ACCESS);
	
	
	if (ServiceHandle==NULL)
	{
		TRACE_MESSAGE("Error opening service %s:",ServiceName);
		DisplayErrorText(GetLastError());
		
		if (!CloseServiceHandle(SCM_Handle))
			TRACE_MESSAGE("Error closing Service control Manager\n");
		
		return -1;
	}
	
	ReturnValue=0;
	
	if (!ControlService(ServiceHandle,SERVICE_CONTROL_STOP,&ServiceStatus))
	{
		TRACE_MESSAGE("Error stopping service %s: ",ServiceName);
		DisplayErrorText(GetLastError());
		ReturnValue=-1;
		
	}
	else
		TRACE_MESSAGE("Service %s successfully stopped\n",ServiceName);
	
	if (!CloseServiceHandle(ServiceHandle))
	{
		TRACE_MESSAGE("Error closing service %s: ",ServiceName);
		DisplayErrorText(GetLastError());
		
		ReturnValue=-1;
	}
	
	if (!CloseServiceHandle(SCM_Handle))
	{
		TRACE_MESSAGE("Error closing Service control Manager\n");
		ReturnValue=-1;
	}
	
	return ReturnValue;
	
}

int change_start_type_service(LPCTSTR ServiceName, DWORD StartType)
{
	SC_HANDLE SCM_Handle;
	SC_HANDLE ServiceHandle;
	DWORD ReturnValue;
	
	SCM_Handle=OpenSCManager(NULL,  /*local machine  */
		NULL,						/*active database*/
		SC_MANAGER_ALL_ACCESS);
	
	if (SCM_Handle == NULL)
	{
		TRACE_MESSAGE("Error opening Service Control Manager: ");
		DisplayErrorText(GetLastError());
		return -1;
	}
	
	ServiceHandle=OpenService(SCM_Handle,
		ServiceName,
		SERVICE_ALL_ACCESS);
	
	
	if (ServiceHandle == NULL)
	{
		TRACE_MESSAGE("Error opening service %s: ",ServiceName);
		DisplayErrorText(GetLastError());
		
		if (!CloseServiceHandle(SCM_Handle))
			TRACE_MESSAGE("Error closing Service control Manager\n");
		
		return -1;
	}
	
	ReturnValue=0;
	
	if (!ChangeServiceConfig(ServiceHandle,
		SERVICE_NO_CHANGE,
		StartType,
		SERVICE_NO_CHANGE,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL))
	{
		TRACE_MESSAGE("Error changing start type for service %s:",ServiceName);
		DisplayErrorText(GetLastError());
		ReturnValue=-1;
		
	}
	else
		TRACE_MESSAGE("Successfully changed start-type for service %s\n",ServiceName);
	
	if (!CloseServiceHandle(ServiceHandle))
	{
		TRACE_MESSAGE("Error closing service %s\n",ServiceName);
		ReturnValue=-1;
	}
	
	
	if (!CloseServiceHandle(SCM_Handle))
	{
		TRACE_MESSAGE("Error closing Service control Manager\n");
		ReturnValue=-1;
	}
	
	return ReturnValue;
	
}


int start_service(LPCTSTR ServiceName)
{
	SC_HANDLE SCM_Handle;
	SC_HANDLE ServiceHandle;
	DWORD ReturnValue;
	
	SCM_Handle=OpenSCManager(NULL,  /*local machine  */
		NULL,						/*active database*/
		SC_MANAGER_ALL_ACCESS);
	
	if (SCM_Handle == NULL)
	{
		TRACE_MESSAGE("Error opening Service Control Manager: ");
		DisplayErrorText(GetLastError());
		return -1;
	}
	
	ServiceHandle=OpenService(SCM_Handle,
		ServiceName,
		SERVICE_ALL_ACCESS);
	
	
	if (ServiceHandle == NULL)
	{
		TRACE_MESSAGE("Error opening service %s: ",ServiceName);
		DisplayErrorText(GetLastError());
		
		if (!CloseServiceHandle(SCM_Handle))
			TRACE_MESSAGE("Error closing Service control Manager\n");
		
		return -1;
	}
	
	ReturnValue=0;
	
	if (!StartService(ServiceHandle,0,NULL))
	{
		TRACE_MESSAGE("Error starting service %s:",ServiceName);
		DisplayErrorText(GetLastError());
		ReturnValue=-1;
		
	}
	else
		TRACE_MESSAGE("Service %s successfully started\n",ServiceName);
	
	if (!CloseServiceHandle(ServiceHandle))
	{
		TRACE_MESSAGE("Error closing service %s\n",ServiceName);
		ReturnValue=-1;
	}
	
	
	if (!CloseServiceHandle(SCM_Handle))
	{
		TRACE_MESSAGE("Error closing Service control Manager\n");
		ReturnValue=-1;
	}
	
	return ReturnValue;
	
}


int create_driver_service(LPCTSTR ServiceName,LPCTSTR ServiceDescription,LPCTSTR ServicePath)
{
	SC_HANDLE SCM_Handle;
	SC_HANDLE ServiceHandle;
	DWORD ReturnValue;
	
	SCM_Handle=OpenSCManager(NULL,  /*local machine  */
		NULL,						/*active database*/
		SC_MANAGER_ALL_ACCESS);
	
	if (SCM_Handle==NULL)
	{
		
		TRACE_MESSAGE("Error opening Service Control Manager: ");
		DisplayErrorText(GetLastError());
		
		return -1;
	}
	
	ServiceHandle=CreateService(SCM_Handle,
		ServiceName,
		ServiceDescription,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		ServicePath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);
	
	if (ServiceHandle==NULL)
	{
		if(GetLastError() == ERROR_SERVICE_EXISTS)
		{
			if (!CloseServiceHandle(SCM_Handle))
				if (stdout != NULL)
					TRACE_MESSAGE("Error closing Service control Manager\n");
				
				return 0;
		}			
		
		TRACE_MESSAGE("Error creating service %s: (%d)",
			ServiceName,
			GetLastError());
		
		DisplayErrorText(GetLastError());
		
		if (!CloseServiceHandle(SCM_Handle))
			if (stdout != NULL)
				TRACE_MESSAGE("Error closing Service control Manager\n");
			
			return -1;
	}
	
	TRACE_MESSAGE("Service %s successfully created.\n",ServiceName);
	
	ReturnValue=0;
	
	if (!CloseServiceHandle(ServiceHandle))
	{
		TRACE_MESSAGE("Error closing service %s.\n",ServiceName);
		ReturnValue=-1;
	}
	
	
	if (!CloseServiceHandle(SCM_Handle))
	{
		TRACE_MESSAGE("Error closing Service control Manager\n");
		ReturnValue=-1;
	}
	
	return ReturnValue;
	
}


int check_if_service_is_running(LPCTSTR ServiceName)
{
	SC_HANDLE SCM_Handle;
	SC_HANDLE ServiceHandle;
	SERVICE_STATUS ServiceStatus;
	
	SCM_Handle=OpenSCManager(NULL,  /*local machine  */
		NULL,						/*active database*/
		SC_MANAGER_ALL_ACCESS);
	
	if (SCM_Handle == NULL)
	{
		TRACE_MESSAGE("Error opening Service Control Manager: ");
		DisplayErrorText(GetLastError());
		return -1;
	}
	
	ServiceHandle = OpenService(SCM_Handle,
		ServiceName,
		SERVICE_ALL_ACCESS);
		
	if (ServiceHandle == NULL)
	{		
		// Requested service not existing
		CloseServiceHandle(SCM_Handle);		
		return -1;
	}
	
	if(!ControlService(ServiceHandle, SERVICE_CONTROL_INTERROGATE, &ServiceStatus))
	{
		// We get here if service's not been started
		
		DisplayErrorText(GetLastError());
		
		// Close handles
		CloseServiceHandle(ServiceHandle);	
		CloseServiceHandle(SCM_Handle);
		
		return -1;
	}
	
	// Close handles
	CloseServiceHandle(ServiceHandle);	
	CloseServiceHandle(SCM_Handle);
	
	if(ServiceStatus.dwCurrentState == SERVICE_RUNNING)
		return 0;
	else
		return -1;
}


int check_if_service_is_present(LPCTSTR ServiceName)
{
	SC_HANDLE SCM_Handle;
	SC_HANDLE ServiceHandle;
	
	SCM_Handle=OpenSCManager(NULL,  /*local machine  */
		NULL,						/*active database*/
		SC_MANAGER_ALL_ACCESS);
	
	if (SCM_Handle == NULL)
	{
		TRACE_MESSAGE("Error opening Service Control Manager: ");
		DisplayErrorText(GetLastError());
		return -1;
	}
	
	ServiceHandle = OpenService(SCM_Handle,
		ServiceName,
		SERVICE_ALL_ACCESS);
		
	if (ServiceHandle == NULL)
	{		
		// Requested service not existing
		CloseServiceHandle(SCM_Handle);		
		return -1;
	}
	
	
	// Close handles
	CloseServiceHandle(ServiceHandle);	
	CloseServiceHandle(SCM_Handle);
	
	return 0;
}


void DisplayErrorText(DWORD dwLastError)
{
    HMODULE hModule = NULL; // default to system source
    LPSTR MessageBuffer;
    DWORD dwBufferLength;
	
    DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_FROM_SYSTEM ;
	
    //
    // Call FormatMessage() to allow for message 
    //  text to be acquired from the system 
    //  or from the supplied module handle.
    //
	
    if(dwBufferLength = FormatMessageA(
        dwFormatFlags,
        hModule, // module to get message from (NULL == system)
        dwLastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
        (LPSTR) &MessageBuffer,
        0,
        NULL
        ))
    {
		TRACE_MESSAGE("%s",MessageBuffer);
		
        //
        // Free the buffer allocated by the system.
        //
        LocalFree(MessageBuffer);
    }
	
    //
    // If we loaded a message source, unload it.
    //
    if(hModule != NULL)
        FreeLibrary(hModule);
}
