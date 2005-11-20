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

#include "pch.h"
#pragma hdrstop
#include "NetMonInstaller.h"


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


// ----------------------------------------------------------------------
// Global vars
//
//BOOL g_fVerbose=FALSE;
//static WCHAR* optarg;


void Usage()
{
	_tprintf(TEXT("******************************************************************************\n"));
	_tprintf(TEXT("NetMon protocol driver installer.\n"));
	_tprintf(TEXT("Copyright (C) 2003-2005  NetGroup, Politecnico di Torino\n"));
	_tprintf(TEXT("Copyright (C) 2005  CACE Technologies, Davis, California\n"));
	_tprintf(TEXT("******************************************************************************\n\n"));
	_tprintf(TEXT("Usage: NetMonInstaller (u|i)\n\n"));
	_tprintf(TEXT("\ti - Installs the driver\n\tu - Uninstalls the driver\n"));
}


// ----------------------------------------------------------------------
//
// Function:  wmain
//
// Purpose:   The main function
//
// Arguments: standard main args
//
// Returns:   0 on success, non-zero otherwise
//
// Author:    kumarp 25-December-97
//
// Notes:
//
EXTERN_C int __cdecl wmain(int argc, WCHAR* argv[])
{
    HRESULT hr=S_OK;
    WCHAR ch;
    enum NetClass nc=NC_Unknown;

    // use simple cmd line parsing to get parameters for actions
    // we want to perform. the order of parameters supplied is significant.

     WCHAR szFileFullPath[MAX_PATH+1];
    PWSTR szFileComponent;


	if (argc < 2)
	{
		Usage();
		return -1;
	}

	switch (argv[1][0])
        {
        case L'i':
        case L'I':
             hr = HrInstallNetMonProtocol();
			 if (hr != S_OK)
				_tprintf(TEXT("Failed to install the NetMon protocol driver. HRESULT= %d\n"), hr);
			else
				_tprintf(TEXT("Installation successful!\n"));
            break;

        case L'u':
        case L'U':
            hr = HrUninstallNetMonProtocol();
			if (hr != S_OK)
				_tprintf(TEXT("Failed to uninstall the NetMon protocol driver. HRESULT= %d\n"), hr);
			else
				_tprintf(TEXT("Uninstallation successful!\n"));
            break;
		default:;
			
		}


    return hr;
}


