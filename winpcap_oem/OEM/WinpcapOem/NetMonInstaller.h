//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997.
//
//  File:       S N E T C F G . H
//
//  Contents:   Sample code that demonstrates how to:
//              - find out if a component is installed
//              - install a net component
//              - install an OEM net component
//              - uninstall a net component
//              - enumerate net components
//              - enumerate net adapters using Setup API
//              - enumerate binding paths of a component
//
//  Notes:
//
//  Author:     kumarp 26-March-98
//
//----------------------------------------------------------------------------

#pragma once

enum NetClass
{
    NC_NetAdapter=0,
    NC_NetProtocol,
    NC_NetService,
    NC_NetClient,
    NC_Unknown
};

void FindIfComponentInstalled(IN PCWSTR szComponentId);

HRESULT	HrInstallNetMonProtocol();
HRESULT	HrUninstallNetMonProtocol();

