/*
 * Copyright (c) 1999 - 2003
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

/*
 This file contains the support functions used by packet.dll to retrieve information about installed 
 adapters, like

	- the adapter list
	- the device associated to any adapter and the description of the adapter
	- physical parameters like the linkspeed or the link layer type
	- the IP and link layer addresses  */

#define UNICODE 1

#include <stdio.h>
#include <packet32.h>
#include "WanPacket/WanPacket.h"

#include <windows.h>
#include <windowsx.h>
#include <Iphlpapi.h>
#include <IPIfCons.h>
#include <stdio.h>

#include <ntddndis.h>

#include <WpcapNames.h>

LPADAPTER PacketOpenAdapterNPF(PCHAR AdapterName);
BOOLEAN PacketAddFakeNdisWanAdapter();

PADAPTER_INFO AdaptersInfoList = NULL;	///< Head of the adapter information list. This list is populated when packet.dll is linked by the application.
HANDLE AdaptersInfoMutex;		///< Mutex that protects the adapter information list. NOTE: every API that takes an ADAPTER_INFO as parameter assumes that it has been called with the mutex acquired.

extern FARPROC GetAdaptersAddressesPointer;

#ifdef HAVE_DAG_API
extern dagc_open_handler p_dagc_open;
extern dagc_close_handler p_dagc_close;
extern dagc_getlinktype_handler p_dagc_getlinktype;
extern dagc_getlinkspeed_handler p_dagc_getlinkspeed;
extern dagc_finddevs_handler p_dagc_finddevs;
extern dagc_freedevs_handler p_dagc_freedevs;
#endif /* HAVE_DAG_API */

/// Title of error windows
TCHAR   szWindowTitle[] = TEXT("PACKET.DLL");

ULONG inet_addrU(const WCHAR *cp);

extern HKEY WinpcapKey;
extern WCHAR *WinPcapKeyBuffer;


/*! 
  \brief Gets the link layer of an adapter, querying the registry.
  \param AdapterObject Handle to an open adapter.
  \param type Pointer to a NetType structure that will be filled by the function.
  \return If the function succeeds, the return value is nonzero, otherwise the return value is zero.

  This function retrieves from the registry the link layer and the speed (in bps) of an opened adapter.
  These values are copied in the NetType structure provided by the user.
  The LinkType field of the type parameter can have one of the following values:

  - NdisMedium802_3: Ethernet (802.3) 
  - NdisMediumWan: WAN 
  - NdisMedium802_5: Token Ring (802.5) 
  - NdisMediumFddi: FDDI 
  - NdisMediumAtm: ATM 
  - NdisMediumArcnet878_2: ARCNET (878.2) 
*/
BOOLEAN PacketGetLinkLayerFromRegistry(LPADAPTER AdapterObject, NetType *type)
{
    BOOLEAN    Status;
    ULONG      IoCtlBufferLength=(sizeof(PACKET_OID_DATA)+sizeof(ULONG)-1);
    PPACKET_OID_DATA  OidData;

    OidData=GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT,IoCtlBufferLength);
    if (OidData == NULL) {
        ODS("PacketGetLinkLayerFromRegistry failed\n");
        return FALSE;
    }
	//get the link-layer type
    OidData->Oid = OID_GEN_MEDIA_IN_USE;
    OidData->Length = sizeof (ULONG);
    Status = PacketRequest(AdapterObject,FALSE,OidData);
    type->LinkType=*((UINT*)OidData->Data);

	//get the link-layer speed
    OidData->Oid = OID_GEN_LINK_SPEED;
    OidData->Length = sizeof (ULONG);
    Status = PacketRequest(AdapterObject,FALSE,OidData);
	type->LinkSpeed=*((UINT*)OidData->Data)*100;
    GlobalFreePtr (OidData);

	ODSEx("Media:%d\n",type->LinkType);
	ODSEx("Speed=%d\n",type->LinkSpeed);

    return Status;
}


/*!
  \brief Scan the registry to retrieve the IP addresses of an adapter.
  \param AdapterName String that contains the name of the adapter.
  \param buffer A user allocated array of npf_if_addr that will be filled by the function.
  \param NEntries Size of the array (in npf_if_addr).
  \return If the function succeeds, the return value is nonzero.

  This function grabs from the registry information like the IP addresses, the netmasks 
  and the broadcast addresses of an interface. The buffer passed by the user is filled with 
  npf_if_addr structures, each of which contains the data for a single address. If the buffer
  is full, the reaming addresses are dropeed, therefore set its dimension to sizeof(npf_if_addr)
  if you want only the first address.
*/
BOOLEAN PacketGetAddressesFromRegistry(LPTSTR AdapterName, npf_if_addr* buffer, PLONG NEntries)
{
	char	*AdapterNameA;
	WCHAR	*AdapterNameU;
	WCHAR	*ifname;
	HKEY	SystemKey;
	HKEY	InterfaceKey;
	HKEY	ParametersKey;
	HKEY	TcpIpKey;
	HKEY	UnderTcpKey;
	LONG	status;
	WCHAR	String[1024+1];
	DWORD	RegType;
	ULONG	BufLen;
	DWORD	DHCPEnabled;
	struct	sockaddr_in *TmpAddr, *TmpBroad;
	LONG	naddrs,nmasks,StringPos;
	DWORD	ZeroBroadcast;
	UINT	RegQueryLen;
	WCHAR	npfDeviceNamesPrefix[MAX_WINPCAP_KEY_CHARS];
	
	
	AdapterNameA = (char*)AdapterName;
	if(AdapterNameA[1] != 0) {	//ASCII
		AdapterNameU = SChar2WChar(AdapterNameA);
		AdapterName = AdapterNameU;
	} else {				//Unicode
		AdapterNameU = NULL;
	}
	ifname = wcsrchr(AdapterName, '\\');
	if (ifname == NULL)
		ifname = AdapterName;
	else
		ifname++;

	RegQueryLen = sizeof(npfDeviceNamesPrefix)/sizeof(npfDeviceNamesPrefix[0]);
	
	if (QueryWinPcapRegistryStringW(TEXT(NPF_DEVICES_PREFIX_REG_KEY), npfDeviceNamesPrefix, &RegQueryLen, NPF_DEVICE_NAMES_PREFIX_WIDECHAR) == FALSE && RegQueryLen == 0)
		return FALSE;
	
	if (wcsncmp(ifname, npfDeviceNamesPrefix, RegQueryLen) == 0)
		ifname += RegQueryLen;

	if(	RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces"), 0, KEY_READ, &UnderTcpKey) == ERROR_SUCCESS)
	{
		status = RegOpenKeyEx(UnderTcpKey,ifname,0,KEY_READ,&TcpIpKey);
		if (status != ERROR_SUCCESS) {
			RegCloseKey(UnderTcpKey);
			goto fail;
		}
	}
	else
	{
		
		// Query the registry key with the interface's adresses
		status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("SYSTEM\\CurrentControlSet\\Services"),0,KEY_READ,&SystemKey);
		if (status != ERROR_SUCCESS)
			goto fail;
		status = RegOpenKeyEx(SystemKey,ifname,0,KEY_READ,&InterfaceKey);
		if (status != ERROR_SUCCESS) {
			RegCloseKey(SystemKey);
			RegCloseKey(UnderTcpKey);
			goto fail;
		}
		RegCloseKey(SystemKey);
		status = RegOpenKeyEx(InterfaceKey,TEXT("Parameters"),0,KEY_READ,&ParametersKey);
		if (status != ERROR_SUCCESS) {
			RegCloseKey(InterfaceKey);
			RegCloseKey(UnderTcpKey);
			goto fail;
		}
		RegCloseKey(InterfaceKey);
		status = RegOpenKeyEx(ParametersKey,TEXT("TcpIp"),0,KEY_READ,&TcpIpKey);
		if (status != ERROR_SUCCESS) {
			RegCloseKey(ParametersKey);
			RegCloseKey(UnderTcpKey);
			goto fail;
		}
		RegCloseKey(ParametersKey);
		BufLen = sizeof String;
	}

	BufLen = 4;
	/* Try to detect if the interface has a zero broadcast addr */
	status=RegQueryValueEx(TcpIpKey,TEXT("UseZeroBroadcast"),NULL,&RegType,(LPBYTE)&ZeroBroadcast,&BufLen);
	if (status != ERROR_SUCCESS)
		ZeroBroadcast=0;
	
	BufLen = 4;
	/* See if DHCP is used by this system */
	status=RegQueryValueEx(TcpIpKey,TEXT("EnableDHCP"),NULL,&RegType,(LPBYTE)&DHCPEnabled,&BufLen);
	if (status != ERROR_SUCCESS)
		DHCPEnabled=0;
	
	
	/* Retrieve the adrresses */
	if(DHCPEnabled){
		
		BufLen = sizeof String;
		// Open the key with the addresses
		status = RegQueryValueEx(TcpIpKey,TEXT("DhcpIPAddress"),NULL,&RegType,(LPBYTE)String,&BufLen);
		if (status != ERROR_SUCCESS) {
			RegCloseKey(TcpIpKey);
			RegCloseKey(UnderTcpKey);
			goto fail;
		}

		// scan the key to obtain the addresses
		StringPos = 0;
		for(naddrs = 0;naddrs <* NEntries;naddrs++){
			TmpAddr = (struct sockaddr_in *) &(buffer[naddrs].IPAddress);
			
			if((TmpAddr->sin_addr.S_un.S_addr = inet_addrU(String + StringPos))!= -1){
				TmpAddr->sin_family = AF_INET;
				
				TmpBroad = (struct sockaddr_in *) &(buffer[naddrs].Broadcast);
				TmpBroad->sin_family = AF_INET;
				if(ZeroBroadcast==0)
					TmpBroad->sin_addr.S_un.S_addr = 0xffffffff; // 255.255.255.255
				else
					TmpBroad->sin_addr.S_un.S_addr = 0; // 0.0.0.0

				while(*(String + StringPos) != 0)StringPos++;
				StringPos++;
				
				if(*(String + StringPos) == 0 || (StringPos * sizeof (WCHAR)) >= BufLen)
					break;				
			}
			else break;
		}		
		
		BufLen = sizeof String;
		// Open the key with the netmasks
		status = RegQueryValueEx(TcpIpKey,TEXT("DhcpSubnetMask"),NULL,&RegType,(LPBYTE)String,&BufLen);
		if (status != ERROR_SUCCESS) {
			RegCloseKey(TcpIpKey);
			RegCloseKey(UnderTcpKey);
			goto fail;
		}
		
		// scan the key to obtain the masks
		StringPos = 0;
		for(nmasks = 0;nmasks < *NEntries;nmasks++){
			TmpAddr = (struct sockaddr_in *) &(buffer[nmasks].SubnetMask);
			
			if((TmpAddr->sin_addr.S_un.S_addr = inet_addrU(String + StringPos))!= -1){
				TmpAddr->sin_family = AF_INET;
				
				while(*(String + StringPos) != 0)StringPos++;
				StringPos++;
								
				if(*(String + StringPos) == 0 || (StringPos * sizeof (WCHAR)) >= BufLen)
					break;
			}
			else break;
		}		
		
		// The number of masks MUST be equal to the number of adresses
		if(nmasks != naddrs){
			RegCloseKey(TcpIpKey);
			RegCloseKey(UnderTcpKey);
			goto fail;
		}
				
	}
	else{
		
		BufLen = sizeof String;
		// Open the key with the addresses
		status = RegQueryValueEx(TcpIpKey,TEXT("IPAddress"),NULL,&RegType,(LPBYTE)String,&BufLen);
		if (status != ERROR_SUCCESS) {
			RegCloseKey(TcpIpKey);
			RegCloseKey(UnderTcpKey);
			goto fail;
		}
		
		// scan the key to obtain the addresses
		StringPos = 0;
		for(naddrs = 0;naddrs < *NEntries;naddrs++){
			TmpAddr = (struct sockaddr_in *) &(buffer[naddrs].IPAddress);
			
			if((TmpAddr->sin_addr.S_un.S_addr = inet_addrU(String + StringPos))!= -1){
				TmpAddr->sin_family = AF_INET;

				TmpBroad = (struct sockaddr_in *) &(buffer[naddrs].Broadcast);
				TmpBroad->sin_family = AF_INET;
				if(ZeroBroadcast==0)
					TmpBroad->sin_addr.S_un.S_addr = 0xffffffff; // 255.255.255.255
				else
					TmpBroad->sin_addr.S_un.S_addr = 0; // 0.0.0.0
				
				while(*(String + StringPos) != 0)StringPos++;
				StringPos++;
				
				if(*(String + StringPos) == 0 || (StringPos * sizeof (WCHAR)) >= BufLen)
					break;
			}
			else break;
		}		
		
		BufLen = sizeof String;
		// Open the key with the netmasks
		status = RegQueryValueEx(TcpIpKey,TEXT("SubnetMask"),NULL,&RegType,(LPBYTE)String,&BufLen);
		if (status != ERROR_SUCCESS) {
			RegCloseKey(TcpIpKey);
			RegCloseKey(UnderTcpKey);
			goto fail;
		}
		
		// scan the key to obtain the masks
		StringPos = 0;
		for(nmasks = 0;nmasks <* NEntries;nmasks++){
			TmpAddr = (struct sockaddr_in *) &(buffer[nmasks].SubnetMask);
			
			if((TmpAddr->sin_addr.S_un.S_addr = inet_addrU(String + StringPos))!= -1){
				TmpAddr->sin_family = AF_INET;
				
				while(*(String + StringPos) != 0)StringPos++;
				StringPos++;
				
				if(*(String + StringPos) == 0 || (StringPos * sizeof (WCHAR)) >= BufLen)
					break;
			}
			else break;
		}		
		
		// The number of masks MUST be equal to the number of adresses
		if(nmasks != naddrs){
			RegCloseKey(TcpIpKey);
			RegCloseKey(UnderTcpKey);
			goto fail;
		}
				
	}
	
	*NEntries = naddrs + 1;

	RegCloseKey(TcpIpKey);
	RegCloseKey(UnderTcpKey);
	
	if (status != ERROR_SUCCESS) {
		goto fail;
	}
	
	
	if (AdapterNameU != NULL)
		GlobalFreePtr(AdapterNameU);
	return TRUE;
	
fail:
	if (AdapterNameU != NULL)
		GlobalFreePtr(AdapterNameU);
    return FALSE;
}

/*!
  \brief Adds the IPv6 addresses of an adapter to the ADAPTER_INFO structure that describes it.
  \param AdInfo Pointer to the ADAPTER_INFO structure that keeps the information about the adapter.
  \return If the function succeeds, the function returns TRUE.

  \note the structure pointed by AdInfo must be initialized the an properly filled. In particular, AdInfo->Name
  must be a valid capture device name.
  \note uses the GetAdaptersAddresses() Ip Helper API function, so it works only on systems where IP Helper API
  provides it (WinXP and successive).
  \note we suppose that we are called after having acquired the AdaptersInfoMutex mutex
*/
#ifndef _WINNT4
BOOLEAN PacketAddIP6Addresses(PADAPTER_INFO AdInfo)
{
	ULONG BufLen;
	PIP_ADAPTER_ADDRESSES AdBuffer, TmpAddr;
	PCHAR OrName;
	PIP_ADAPTER_UNICAST_ADDRESS UnicastAddr;
	struct sockaddr_storage *Addr;
	INT	AddrLen;
	UINT	RegQueryLen;
	CHAR	npfDeviceNamesPrefix[MAX_WINPCAP_KEY_CHARS];

	ODS("PacketAddIP6Addresses\n");

	if(GetAdaptersAddressesPointer == NULL)	return TRUE;	// GetAdaptersAddresses() not present on this system,
															// return immediately.

 	if(GetAdaptersAddressesPointer(AF_UNSPEC, GAA_FLAG_SKIP_ANYCAST| GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_FRIENDLY_NAME, NULL, NULL, &BufLen) != ERROR_BUFFER_OVERFLOW)
	{
		ODS("PacketAddIP6Addresses: GetAdaptersAddresses Failed\n");
		return FALSE;
	}

	ODS("PacketAddIP6Addresses, retrieved needed storage for the call\n");

	AdBuffer = GlobalAllocPtr(GMEM_MOVEABLE, BufLen);
	if (AdBuffer == NULL) {
		ODS("PacketAddIP6Addresses: GlobalAlloc Failed\n");
		return FALSE;
	}

 	if(GetAdaptersAddressesPointer(AF_UNSPEC,  GAA_FLAG_SKIP_ANYCAST| GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_FRIENDLY_NAME, NULL, AdBuffer, &BufLen) != ERROR_SUCCESS)
	{
		ODS("PacketGetIP6AddressesIPH: GetAdaptersAddresses Failed\n");
		GlobalFreePtr(AdBuffer);
		return FALSE;
	}

	ODS("PacketAddIP6Addresses, retrieved addresses\n");

	//
	// Scan the list of adddresses obtained from the IP helper API
	//
	for(TmpAddr = AdBuffer; TmpAddr != NULL; TmpAddr = TmpAddr->Next)
	{
		RegQueryLen = sizeof(npfDeviceNamesPrefix)/sizeof(npfDeviceNamesPrefix[0]);
		
		if (QueryWinPcapRegistryStringA(NPF_DRIVER_COMPLETE_DEVICE_PREFIX_REG_KEY, npfDeviceNamesPrefix, &RegQueryLen, NPF_DRIVER_COMPLETE_DEVICE_PREFIX) == FALSE && RegQueryLen == 0)
			continue;
			
		OrName = AdInfo->Name + RegQueryLen - 1;
/*
		OrName = AdInfo->Name + sizeof("\\device\\npf_") - 1;
*/		
		ODS("PacketAddIP6Addresses, external loop\n");
		if(strcmp(TmpAddr->AdapterName, OrName) == 0)
		{
			// Found a corresponding adapter, scan its address list
			for(UnicastAddr = TmpAddr->FirstUnicastAddress; UnicastAddr != NULL; UnicastAddr = UnicastAddr->Next)
			{
					ODS("PacketAddIP6Addresses, internal loop\n");

					AddrLen = UnicastAddr->Address.iSockaddrLength;
					Addr = (struct sockaddr_storage *)UnicastAddr->Address.lpSockaddr;
					if(Addr->ss_family == AF_INET6)
					{
						// Be sure not to overflow the addresses buffer of this adapter
						if(AdInfo->NNetworkAddresses >= MAX_NETWORK_ADDRESSES)
						{
							GlobalFreePtr(AdBuffer);
							return FALSE;
						}

						memcpy(&(AdInfo->NetworkAddresses[AdInfo->NNetworkAddresses].IPAddress), Addr, AddrLen);
						memset(&(AdInfo->NetworkAddresses[AdInfo->NNetworkAddresses].SubnetMask), 0, sizeof(struct sockaddr_storage));
						memset(&(AdInfo->NetworkAddresses[AdInfo->NNetworkAddresses].Broadcast), 0, sizeof(struct sockaddr_storage));
						AdInfo->NNetworkAddresses ++;
					}
			}
		}
	}

	ODS("PacketAddIP6Addresses, finished parsing the addresses\n");

	GlobalFreePtr(AdBuffer);

	return TRUE;
}
#endif // _WINNT4

/*!
  \brief Check if a string contains the "1394" substring

	We prevent opening of firewire adapters since they have non standard behaviors that can cause
	problems with winpcap

  \param AdapterDesc NULL-terminated ASCII string with the adapter's description 
  \return TRUE if the input string contains "1394"
*/
BOOLEAN IsFireWire(TCHAR *AdapterDesc)
{
	if(wcsstr(AdapterDesc, FIREWIRE_SUBSTR) != NULL)
	{		
		return TRUE;
	}

	return FALSE;
}

/*!
  \brief Adds an entry to the adapter description list, gathering its values from the IP Helper API.
  \param IphAd PIP_ADAPTER_INFO IP Helper API structure containing the parameters of the adapter that must be added to the list.
  \return If the function succeeds, the return value is TRUE.
  \note we suppose that we are called after having acquired the AdaptersInfoMutex mutex
*/
#ifndef _WINNT4
BOOLEAN AddAdapterIPH(PIP_ADAPTER_INFO IphAd)
{
	PIP_ADAPTER_INFO AdList = NULL;
	ULONG OutBufLen=0;
	PADAPTER_INFO TmpAdInfo, SAdInfo;
	PIP_ADDR_STRING TmpAddrStr;
	UINT i;
	struct sockaddr_in *TmpAddr;
	CHAR TName[256];
	LPADAPTER adapter;
	PWCHAR UAdName;
	UINT	RegQueryLen;
	CHAR	npfCompleteDriverPrefix[MAX_WINPCAP_KEY_CHARS];

	// Create the NPF device name from the original device name

	RegQueryLen = sizeof(npfCompleteDriverPrefix)/sizeof(npfCompleteDriverPrefix[0]);
	
	if (QueryWinPcapRegistryStringA(NPF_DRIVER_COMPLETE_DEVICE_PREFIX_REG_KEY, npfCompleteDriverPrefix, &RegQueryLen, NPF_DRIVER_COMPLETE_DEVICE_PREFIX) == FALSE && RegQueryLen == 0)
		return FALSE;

	// Create the NPF device name from the original device name
	_snprintf(TName,
		sizeof(TName) - 1 - RegQueryLen - 1, 
		"%s%s",
		npfCompleteDriverPrefix, 
		IphAd->AdapterName);
	TName[sizeof(TName) - 1] = '\0';

	
	// Scan the adapters list to see if this one is already present
	for(SAdInfo = AdaptersInfoList; SAdInfo != NULL; SAdInfo = SAdInfo->Next)
	{
		if(strcmp(TName, SAdInfo->Name) == 0)
		{
			ODS("PacketGetAdaptersIPH: Adapter already present in the list\n");
			goto SkipAd;
		}
	}
	
	if(IphAd->Type == IF_TYPE_PPP || IphAd->Type == IF_TYPE_SLIP)
	{
		if (!WanPacketTestAdapter())
			goto SkipAd;
	}
	else
	{
		//convert the string to unicode, as OpenAdapterNPF accepts unicode strings, only. 
		UAdName = SChar2WChar(TName);
		if (UAdName == NULL)
		{
			ODS("AddAdapterIPH: unable to convert an ASCII string to UNICODE\n");
			goto SkipAd;
		}

		adapter = PacketOpenAdapterNPF((PCHAR)UAdName);
		GlobalFreePtr(UAdName);

		if(adapter == NULL)
		{
			// We are not able to open this adapter. Skip to the next one.
			ODS("PacketGetAdaptersIPH: unable to open the adapter\n");
			goto SkipAd;
		}
		else
		{
			PacketCloseAdapter(adapter);
		}
	}	
	
	// 
	// Adapter valid and not yet present in the list. Allocate the ADAPTER_INFO structure
	//
	TmpAdInfo = GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(ADAPTER_INFO));
	if (TmpAdInfo == NULL) {
		ODS("PacketGetAdaptersIPH: GlobalAlloc Failed\n");
		return FALSE;
	}
	
	// Copy the device name
	strcpy(TmpAdInfo->Name, TName);
	
	// Copy the description
	_snprintf(TmpAdInfo->Description, ADAPTER_DESC_LENGTH, "%s", IphAd->Description);
	
	// Copy the MAC address
	TmpAdInfo->MacAddressLen = IphAd->AddressLength;
	
	memcpy(TmpAdInfo->MacAddress, 
		IphAd->Address, 
		(MAX_MAC_ADDR_LENGTH<MAX_ADAPTER_ADDRESS_LENGTH)? MAX_MAC_ADDR_LENGTH:MAX_ADAPTER_ADDRESS_LENGTH);
	
	// Calculate the number of IP addresses of this interface
	for(TmpAddrStr = &IphAd->IpAddressList, i = 0; TmpAddrStr != NULL; TmpAddrStr = TmpAddrStr->Next, i++)
	{
		
	}

	TmpAdInfo->NetworkAddresses = GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT, MAX_NETWORK_ADDRESSES * sizeof(npf_if_addr));
	if (TmpAdInfo->NetworkAddresses == NULL) {
		ODS("PacketGetAdaptersIPH: GlobalAlloc Failed\n");
		GlobalFreePtr(TmpAdInfo);
		return FALSE;
	}
	
	// Scan the addresses, convert them to addrinfo structures and put each of them in the list
	for(TmpAddrStr = &IphAd->IpAddressList, i = 0; TmpAddrStr != NULL; TmpAddrStr = TmpAddrStr->Next)
	{
		TmpAddr = (struct sockaddr_in *)&(TmpAdInfo->NetworkAddresses[i].IPAddress);
		if((TmpAddr->sin_addr.S_un.S_addr = inet_addr(TmpAddrStr->IpAddress.String))!= INADDR_NONE)
		{
			TmpAddr->sin_family = AF_INET;
			TmpAddr = (struct sockaddr_in *)&(TmpAdInfo->NetworkAddresses[i].SubnetMask);
			TmpAddr->sin_addr.S_un.S_addr = inet_addr(TmpAddrStr->IpMask.String);
			TmpAddr->sin_family = AF_INET;
			TmpAddr = (struct sockaddr_in *)&(TmpAdInfo->NetworkAddresses[i].Broadcast);
			TmpAddr->sin_addr.S_un.S_addr = 0xffffffff; // Consider 255.255.255.255 as broadcast address since IP Helper API doesn't provide information about it
			TmpAddr->sin_family = AF_INET;
			i++;
		}
	}
	
	TmpAdInfo->NNetworkAddresses = i;
	
	// Now Add IPv6 Addresses
	PacketAddIP6Addresses(TmpAdInfo);
	
	if(IphAd->Type == IF_TYPE_PPP || IphAd->Type == IF_TYPE_SLIP)
	{
		// NdisWan adapter
		TmpAdInfo->Flags = INFO_FLAG_NDISWAN_ADAPTER;
	}
	
	// Update the AdaptersInfo list
	TmpAdInfo->Next = AdaptersInfoList;
	AdaptersInfoList = TmpAdInfo;
	
SkipAd:

	return TRUE;
}
#endif // _WINNT4


/*!
  \brief Updates the list of the adapters querying the IP Helper API.
  \return If the function succeeds, the return value is nonzero.

  This function populates the list of adapter descriptions, retrieving the information from a query to
  the IP Helper API. The IP Helper API is used as a support of the standard registry query method to obtain
  adapter information, so PacketGetAdaptersIPH() add only information about the adapters that were not 
  found by PacketGetAdapters().
*/
#ifndef _WINNT4
BOOLEAN PacketGetAdaptersIPH()
{
	PIP_ADAPTER_INFO AdList = NULL;
	PIP_ADAPTER_INFO TmpAd;
	ULONG OutBufLen=0;

	ODS("PacketGetAdaptersIPH\n");

	// Find the size of the buffer filled by GetAdaptersInfo
	if(GetAdaptersInfo(AdList, &OutBufLen) == ERROR_NOT_SUPPORTED)
	{
		ODS("IP Helper API not supported on this system!\n");
		return FALSE;
	}

	ODS("PacketGetAdaptersIPH: retrieved needed bytes for IPH\n");

	// Allocate the buffer
	AdList = GlobalAllocPtr(GMEM_MOVEABLE, OutBufLen);
	if (AdList == NULL) {
		ODS("PacketGetAdaptersIPH: GlobalAlloc Failed\n");
		return FALSE;
	}
	
	// Retrieve the adapters information using the IP helper API
	GetAdaptersInfo(AdList, &OutBufLen);
	
	ODS("PacketGetAdaptersIPH: retrieved list from IPH\n");

	// Scan the list of adapters obtained from the IP helper API, create a new ADAPTER_INFO
	// structure for every new adapter and put it in our global list
	for(TmpAd = AdList; TmpAd != NULL; TmpAd = TmpAd->Next)
	{
		AddAdapterIPH(TmpAd);
	}
	
	GlobalFreePtr(AdList);

	return TRUE;
}
#endif // _WINNT4


/*!
  \brief Adds an entry to the adapter description list.
  \param AdName Name of the adapter to add
  \return If the function succeeds, the return value is nonzero.

  Used by PacketGetAdapters(). Queries the registry to fill the PADAPTER_INFO describing the new adapter.
*/
BOOLEAN AddAdapter(PCHAR AdName, UINT flags)
{
	//this function should acquire the AdaptersInfoMutex, since it's NOT called with an ADAPTER_INFO as parameter
	DWORD		RegKeySize=0;
	LONG		Status;
	LPADAPTER	adapter;
	PPACKET_OID_DATA  OidData;
	int			i=0;
	PADAPTER_INFO	TmpAdInfo;
	PADAPTER_INFO TAdInfo;	
	PWCHAR		UAdName;
	
	ODS("AddAdapter\n");
	
	//
	// let's check that the adapter name will fit in the space available within ADAPTER_INFO::Name
	// If not, simply fail, since we cannot properly save the adapter name
	//
	if (strlen(AdName) + 1 > sizeof(TmpAdInfo->Name))
	{
		ODS("AddAdapter: adapter name is too long to be stored into ADAPTER_INFO::Name, simply skip it");
		return FALSE;
	}

	WaitForSingleObject(AdaptersInfoMutex, INFINITE);
	
	for(TAdInfo = AdaptersInfoList; TAdInfo != NULL; TAdInfo = TAdInfo->Next)
	{
		if(strcmp(AdName, TAdInfo->Name) == 0)
		{
			ODS("AddAdapter: Adapter already present in the list\n");
			ReleaseMutex(AdaptersInfoMutex);
			return TRUE;
		}
	}
	
	//here we could have released the mutex, but what happens if two threads try to add the same adapter? 
	//The adapter would be duplicated on the linked list
	
	if(flags != INFO_FLAG_DONT_EXPORT)
	{	
		UAdName = SChar2WChar(AdName);
	
		// Try to Open the adapter
		adapter = PacketOpenAdapterNPF((PCHAR)UAdName);
		
		GlobalFreePtr(UAdName);
		
		if(adapter == NULL)
		{
			// We are not able to open this adapter. Skip to the next one.
			ReleaseMutex(AdaptersInfoMutex);
			return FALSE;
		}
		
		// Allocate a buffer to get the vendor description from the driver
		OidData = GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT,512);
		if (OidData == NULL) 
		{
			ODS("AddAdapter: GlobalAlloc Failed\n");
			PacketCloseAdapter(adapter);
			ReleaseMutex(AdaptersInfoMutex);
			return FALSE;
		}
	}
	
	//
	// PacketOpenAdapter was succesful. Consider this a valid adapter and allocate an entry for it
	// In the adapter list
	//
	
	TmpAdInfo = GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(ADAPTER_INFO));
	if (TmpAdInfo == NULL) 
	{
		ODS("AddAdapter: GlobalAlloc Failed\n");
	
		if(flags != INFO_FLAG_DONT_EXPORT)
		{ 
			GlobalFreePtr(OidData);
			PacketCloseAdapter(adapter);
		}
		
		ReleaseMutex(AdaptersInfoMutex);
		return FALSE;
	}
	
	// Copy the device name
	strncpy(TmpAdInfo->Name, AdName, sizeof(TmpAdInfo->Name)/ sizeof(TmpAdInfo->Name[0]) - 1);

	//we do not need to terminate the string TmpAdInfo->Name, since we have left a char at the end, and
	//the memory for TmpAdInfo was zeroed upon allocation

	if(flags != INFO_FLAG_DONT_EXPORT)
	{
		// Retrieve the adapter description querying the NIC driver
		OidData->Oid = OID_GEN_VENDOR_DESCRIPTION;
		OidData->Length = 256;
		ZeroMemory(OidData->Data, 256);
		
		Status = PacketRequest(adapter, FALSE, OidData);
		
		if(Status==0 || ((char*)OidData->Data)[0]==0)
		{
			ODS("AddAdapter: unable to get a valid adapter description from the NIC driver\n");
		}
		
		ODSEx("Adapter Description=%s\n\n",OidData->Data);
		
		// Copy the description
		strncpy(TmpAdInfo->Description, OidData->Data, sizeof(TmpAdInfo->Description)/ sizeof(TmpAdInfo->Description[0]) - 1);
		//we do not need to terminate the string TmpAdInfo->Description, since we have left a char at the end, and
		//the memory for TmpAdInfo was zeroed upon allocation
		
		PacketGetLinkLayerFromRegistry(adapter, &(TmpAdInfo->LinkLayer));
		
		// Retrieve the adapter MAC address querying the NIC driver
		OidData->Oid = OID_802_3_CURRENT_ADDRESS;	// XXX At the moment only Ethernet is supported.
													// Waiting a patch to support other Link Layers
		OidData->Length = 256;
		ZeroMemory(OidData->Data, 256);
		
		Status = PacketRequest(adapter, FALSE, OidData);
		if(Status)
		{
			memcpy(TmpAdInfo->MacAddress, OidData->Data, 6);
			TmpAdInfo->MacAddressLen = 6;
		}
		else
		{
			memset(TmpAdInfo->MacAddress, 0, 6);
			TmpAdInfo->MacAddressLen = 0;
		}
		
		// Retrieve IP addresses
		TmpAdInfo->NetworkAddresses = GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT, MAX_NETWORK_ADDRESSES * sizeof(npf_if_addr));
		if (TmpAdInfo->NetworkAddresses == NULL) {
			ODS("AddAdapter: GlobalAlloc Failed\n");
			PacketCloseAdapter(adapter);
			GlobalFreePtr(OidData);
			GlobalFreePtr(TmpAdInfo);
			ReleaseMutex(AdaptersInfoMutex);
			return FALSE;
		}
		
		TmpAdInfo->NNetworkAddresses = MAX_NETWORK_ADDRESSES;
		if(!PacketGetAddressesFromRegistry((LPTSTR)TmpAdInfo->Name, TmpAdInfo->NetworkAddresses, &TmpAdInfo->NNetworkAddresses))
		{
#ifndef _WINNT4
			// Try to see if the interface has some IPv6 addresses
			TmpAdInfo->NNetworkAddresses = 0; // We have no addresses because PacketGetAddressesFromRegistry() failed
			
			if(!PacketAddIP6Addresses(TmpAdInfo))
			{
#endif // _WINNT4
				GlobalFreePtr(TmpAdInfo->NetworkAddresses);
				TmpAdInfo->NetworkAddresses = NULL;
				TmpAdInfo->NNetworkAddresses = 0;
#ifndef _WINNT4
			}
#endif // _WINNT4
		}
		
#ifndef _WINNT4
		// Now Add IPv6 Addresses
		PacketAddIP6Addresses(TmpAdInfo);
#endif // _WINNT4
		
		TmpAdInfo->Flags = INFO_FLAG_NDIS_ADAPTER;	// NdisWan adapters are not exported by the NPF driver,
													// therefore it's impossible to see them here
		
		// Free storage
		PacketCloseAdapter(adapter);
		GlobalFreePtr(OidData);
	}
	else
	{
		// Write in the flags that this adapter is firewire
		// This will block it in all successive calls
		TmpAdInfo->Flags = INFO_FLAG_DONT_EXPORT;
	}
	
	// Update the AdaptersInfo list
	TmpAdInfo->Next = AdaptersInfoList;
	AdaptersInfoList = TmpAdInfo;
	
	ReleaseMutex(AdaptersInfoMutex);
	return TRUE;
}


/*!
  \brief Updates the list of the adapters querying the registry.
  \return If the function succeeds, the return value is nonzero.

  This function populates the list of adapter descriptions, retrieving the information from the registry. 
*/
BOOLEAN PacketGetAdapters()
{
	HKEY		LinkageKey,AdapKey, OneAdapKey;
	DWORD		RegKeySize=0;
	LONG		Status;
	ULONG		Result;
	INT			i;
	DWORD		dim;
	DWORD		RegType;
	WCHAR		TName[256];
	CHAR		TAName[256];
	TCHAR		AdapName[256];
	CHAR		*TcpBindingsMultiString;
	UINT		FireWireFlag;
	UINT		RegQueryLen;
	CHAR		npfCompleteDriverPrefix[MAX_WINPCAP_KEY_CHARS];
	CHAR		DeviceGuidName[256];

	ODS("PacketGetAdapters\n");
	
	// Get device prefixes from the registry
	RegQueryLen = sizeof(npfCompleteDriverPrefix)/sizeof(npfCompleteDriverPrefix[0]);
	
	if (QueryWinPcapRegistryStringA(NPF_DRIVER_COMPLETE_DEVICE_PREFIX_REG_KEY, npfCompleteDriverPrefix, &RegQueryLen, NPF_DRIVER_COMPLETE_DEVICE_PREFIX) == FALSE && RegQueryLen == 0)
		return FALSE;

	Status=RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		TEXT("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"),
		0,
		KEY_READ,
		&AdapKey);
	
	if ( Status != ERROR_SUCCESS ){
		ODS("PacketGetAdapters: RegOpenKeyEx ( Class\\{networkclassguid} ) Failed\n");
		goto tcpip_linkage;
	}

	i = 0;

	ODS("PacketGetAdapters: Cycling through the adapters:\n");

	// 
	// Cycle through the entries inside the {4D36E972-E325-11CE-BFC1-08002BE10318} key
	// To get the names of the adapters
	//
	while((Result = RegEnumKey(AdapKey, i, AdapName, sizeof(AdapName)/2)) == ERROR_SUCCESS)
	{
		i++;
		ODSEx(" %d) ", i);
		FireWireFlag = 0;

		// 
		// Get the adapter name from the registry key
		//
		Status=RegOpenKeyEx(AdapKey, AdapName, 0, KEY_READ, &OneAdapKey);
		if ( Status != ERROR_SUCCESS )
		{
			ODS("PacketGetAdapters: RegOpenKeyEx ( OneAdapKey ) Failed\n");
			continue;			
		}

		//
		//
		// Check if this is a FireWire adapter, looking for "1394" in its ComponentId string. 
		// We prevent listing FireWire adapters because winpcap can open them, but their interface
		// with the OS is broken and they can cause blue screens.
		//
		dim = sizeof(TName);
        Status = RegQueryValueEx(OneAdapKey, 
			TEXT("ComponentId"),
			NULL,
			NULL,
			(PBYTE)TName, 
			&dim);

		if(Status == ERROR_SUCCESS)
		{
			if(IsFireWire(TName))
			{
				FireWireFlag = INFO_FLAG_DONT_EXPORT;
			}
		}

		Status=RegOpenKeyEx(OneAdapKey, TEXT("Linkage"), 0, KEY_READ, &LinkageKey);
		if (Status != ERROR_SUCCESS)
		{
			RegCloseKey(OneAdapKey);
			ODS("PacketGetAdapters: RegOpenKeyEx ( LinkageKey ) Failed\n");
			continue;
		}
		
		dim = sizeof(DeviceGuidName);
        Status=RegQueryValueExA(LinkageKey, 
			"Export", 
			NULL, 
			NULL, 
			(PBYTE)DeviceGuidName, 
			&dim);

		if(Status != ERROR_SUCCESS)
		{
			RegCloseKey(OneAdapKey);
			RegCloseKey(LinkageKey);
			ODS("Name = SKIPPED (error reading the key)\n");
			continue;
		}

		if (strlen(DeviceGuidName) >= strlen("\\Device\\"))
		{
			// Put the \Device\NPF_ string at the beginning of the name
			_snprintf(TAName, sizeof(TAName) - 1, "%s%s",
			npfCompleteDriverPrefix,
			DeviceGuidName + strlen("\\Device\\"));
		}
		else
			continue;

		//terminate the string, just in case
		TAName[sizeof(TAName) - 1] = '\0';

		// If the adapter is valid, add it to the list.
		AddAdapter(TAName, FireWireFlag);

		RegCloseKey(OneAdapKey);
		RegCloseKey(LinkageKey);
		
	} // while enum reg keys

	RegCloseKey(AdapKey);

tcpip_linkage:
	//
	// no adapters were found under {4D36E972-E325-11CE-BFC1-08002BE10318}. This means with great probability
	// that we are under Windows NT 4, so we try to look under the tcpip bindings.
	//
	
	ODS("Adapters not found under SYSTEM\\CurrentControlSet\\Control\\Class. Using the TCP/IP bindings.\n");
		
	Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		TEXT("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Linkage"),
		0,
		KEY_READ,
		&LinkageKey);

	if (Status == ERROR_SUCCESS)
	{
		// Retrieve the length of th binde key
		// This key contains the name of the devices as \device\foo
		//in ASCII, separated by a single '\0'. The list is terminated
		//by another '\0'
		Status=RegQueryValueExA(LinkageKey,
			"bind",
			NULL,
			&RegType,
			NULL,
			&RegKeySize);

		// Allocate the buffer
		TcpBindingsMultiString = GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT, RegKeySize + 2);

		if (TcpBindingsMultiString == NULL)
		{
			return FALSE;
		}
		
		// Query the key again to get its content		
		Status = RegQueryValueExA(LinkageKey,
			"bind",
			NULL,
			&RegType,
			(LPBYTE)TcpBindingsMultiString,
			&RegKeySize);
		
		RegCloseKey(LinkageKey);

		// Scan the buffer with the device names
		for(i = 0;;)
		{
			if (TcpBindingsMultiString[i] == '\0')
				break;
			
			_snprintf(TAName, sizeof(TAName) - 1, "%s%s", 
				npfCompleteDriverPrefix,
				TcpBindingsMultiString + i + strlen("\\Device\\"));

			//terminate the string, just in case
			TAName[sizeof(TAName) - 1] = '\0';
		
			i += strlen(&TcpBindingsMultiString[i]) + 1;

			// If the adapter is valid, add it to the list.
			AddAdapter(TAName, 0);
		}
	
 		GlobalFreePtr(TcpBindingsMultiString);
	}
	
	else{
#ifdef _WINNT4
		MessageBox(NULL,TEXT("Can not find TCP/IP bindings.\nIn order to run the packet capture driver you must install TCP/IP."),szWindowTitle,MB_OK);
		ODS("Cannot find the TCP/IP bindings");
		return FALSE;
#endif		
	}
	
	return TRUE;
}

#ifdef HAVE_DAG_API
/*!
  \brief Add a dag adapter to the adapters info list, gathering information from the dagc API
  \param name Name of the adapter.
  \param description description of the adapter.
  \return If the function succeeds, the return value is nonzero.
*/
BOOLEAN PacketAddAdapterDag(PCHAR name, PCHAR description, BOOLEAN IsAFile)
{
	//this function should acquire the AdaptersInfoMutex, since it's NOT called with an ADAPTER_INFO as parameter
	CHAR ebuf[DAGC_ERRBUF_SIZE];
	PADAPTER_INFO TmpAdInfo;
	dagc_t *dagfd;

	//XXX what about checking if the adapter already exists???
	
	//
	// Allocate a descriptor for this adapter
	//			
	//here we do not acquire the mutex, since we are not touching the list, yet.
    TmpAdInfo = GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(ADAPTER_INFO));
	if (TmpAdInfo == NULL) 
	{
		ODS("PacketAddAdapterDag: GlobalAlloc Failed\n");
		return FALSE;
	}

	// Copy the device name and description
	_snprintf(TmpAdInfo->Name, 
		sizeof(TmpAdInfo->Name), 
		"%s", 
		name);

	_snprintf(TmpAdInfo->Description, 
		sizeof(TmpAdInfo->Description), 
		"%s", 
		description);

	if(IsAFile)
		TmpAdInfo->Flags = INFO_FLAG_DAG_FILE;
	else
		TmpAdInfo->Flags = INFO_FLAG_DAG_CARD;

	if(p_dagc_open)
		dagfd = p_dagc_open(name, 0, ebuf);
	else
		dagfd = NULL;

	if(!dagfd)
	{
		GlobalFreePtr(TmpAdInfo);
		return FALSE;
	}

	TmpAdInfo->LinkLayer.LinkType = p_dagc_getlinktype(dagfd);

	switch(p_dagc_getlinktype(dagfd)) 
	{
	case TYPE_HDLC_POS:
		TmpAdInfo->LinkLayer.LinkType = NdisMediumCHDLC; // Note: custom linktype, NDIS doesn't provide an equivalent
		break;
	case -TYPE_HDLC_POS:
		TmpAdInfo->LinkLayer.LinkType = NdisMediumPPPSerial; // Note: custom linktype, NDIS doesn't provide an equivalent
		break;
	case TYPE_ETH:
		TmpAdInfo->LinkLayer.LinkType = NdisMedium802_3;
		break;
	case TYPE_ATM: 
		TmpAdInfo->LinkLayer.LinkType = NdisMediumAtm;
		break;
	default:
		TmpAdInfo->LinkLayer.LinkType = NdisMediumNull; // Note: custom linktype, NDIS doesn't provide an equivalent
		break;
	}			

	TmpAdInfo->LinkLayer.LinkSpeed = (p_dagc_getlinkspeed(dagfd) == -1)?
		100000000:  // Unknown speed, default to 100Mbit
	p_dagc_getlinkspeed(dagfd) * 1000000; 

	p_dagc_close(dagfd);

	WaitForSingleObject(AdaptersInfoMutex, INFINITE);

	// Update the AdaptersInfo list
	TmpAdInfo->Next = AdaptersInfoList;
	AdaptersInfoList = TmpAdInfo;

	ReleaseMutex(AdaptersInfoMutex);

	return TRUE;
}

/*!
  \brief Updates the list of the adapters using the DAGC API.
  \return If the function succeeds, the return value is nonzero.

  This function populates the list of adapter descriptions, looking for DAG cards on the system. 
*/
BOOLEAN PacketGetAdaptersDag()
{
	CHAR ebuf[DAGC_ERRBUF_SIZE];
	dagc_if_t *devs = NULL, *tmpdevs;
	UINT i;
	
	if(p_dagc_finddevs(&devs, ebuf))
		// No dag cards found on this system
		return FALSE;
	else
	{
		for(tmpdevs = devs, i=0; tmpdevs != NULL; tmpdevs = tmpdevs->next)
		{
			PacketAddAdapterDag(tmpdevs->name, tmpdevs->description, FALSE);
		}
	}
	
	p_dagc_freedevs(devs);
	
	return TRUE;
}
#endif // HAVE_DAG_API

/*!
\brief Find the information about an adapter scanning the global ADAPTER_INFO list.
  \param AdapterName Name of the adapter whose information has to be retrieved.
  \return If the function succeeds, the return value is non-null.
*/
PADAPTER_INFO PacketFindAdInfo(PCHAR AdapterName)
{
	//this function should NOT acquire the AdaptersInfoMutex, since it does return an ADAPTER_INFO structure
	PADAPTER_INFO TAdInfo;
	
	if (AdaptersInfoList == NULL)
		PacketPopulateAdaptersInfoList();

	TAdInfo = AdaptersInfoList;
	
	while(TAdInfo != NULL)
	{
		if(strcmp(TAdInfo->Name, AdapterName) == 0) break;

		TAdInfo = TAdInfo->Next;
	}

	return TAdInfo;
}



/*!
  \brief Updates information about an adapter in the global ADAPTER_INFO list.
  \param AdapterName Name of the adapter whose information has to be retrieved.
  \return If the function succeeds, the return value is TRUE. A false value means that the adapter is no
  more valid or that it is disconnected.
*/
BOOLEAN PacketUpdateAdInfo(PCHAR AdapterName)
{
	//this function should acquire the AdaptersInfoMutex, since it's NOT called with an ADAPTER_INFO as parameter
	PADAPTER_INFO TAdInfo, PrevAdInfo;
	UINT	RegQueryLen;
	CHAR	FakeNdisWanAdapterName[MAX_WINPCAP_KEY_CHARS];

	// retrieve the name for the fake ndis wan adapter
	RegQueryLen = sizeof(FakeNdisWanAdapterName)/sizeof(FakeNdisWanAdapterName[0]);
	if (QueryWinPcapRegistryStringA(NPF_FAKE_NDISWAN_ADAPTER_NAME_REG_KEY, FakeNdisWanAdapterName, &RegQueryLen, FAKE_NDISWAN_ADAPTER_NAME) == FALSE && RegQueryLen == 0)
		return FALSE;
	
	WaitForSingleObject(AdaptersInfoMutex, INFINITE);
	
	PrevAdInfo = TAdInfo = AdaptersInfoList;

	//
	// If an entry for this adapter is present in the list, we destroy it
	//
	while(TAdInfo != NULL)
	{
		if(strcmp(TAdInfo->Name, AdapterName) == 0)
		{
			if (strcmp(AdapterName, FakeNdisWanAdapterName) == 0)
			{
				ReleaseMutex(AdaptersInfoMutex);
				return TRUE;
			}

			if(TAdInfo == AdaptersInfoList)
			{
				AdaptersInfoList = TAdInfo->Next;
			}
			else
			{
				PrevAdInfo->Next = TAdInfo->Next;
			}

			if (TAdInfo->NetworkAddresses != NULL)
				GlobalFreePtr(TAdInfo->NetworkAddresses);
			GlobalFreePtr(TAdInfo);

			break;
		}

		PrevAdInfo = TAdInfo;

		TAdInfo = TAdInfo->Next;
	}

	ReleaseMutex(AdaptersInfoMutex);

	//
	// Now obtain the information about this adapter
	//
	if(AddAdapter(AdapterName, 0) == TRUE)
		return TRUE;
#ifndef _WINNT4
	// 
	// Not a tradiditonal adapter, but possibly a Wan or DAG interface 
	// Gather all the available adapters from IPH API and dagc API
	//
	PacketGetAdaptersIPH();
	PacketAddFakeNdisWanAdapter();
#ifdef HAVE_DAG_API
	if(p_dagc_open == NULL)	
		return TRUE;	// dagc.dll not present on this system.
	else
	PacketGetAdaptersDag();
#endif // HAVE_DAG_API

#endif // _WINNT4

	// Adapter not found
	return TRUE;
}

/*!
  \brief Populates the list of the adapters.

  This function populates the list of adapter descriptions, invoking first PacketGetAdapters() and then
  PacketGetAdaptersIPH(). 
*/
void PacketPopulateAdaptersInfoList()
{
	//this function should acquire the AdaptersInfoMutex, since it's NOT called with an ADAPTER_INFO as parameter
	PADAPTER_INFO TAdInfo;
	PVOID Mem1, Mem2;

	WaitForSingleObject(AdaptersInfoMutex, INFINITE);

	if(AdaptersInfoList)
	{
		// Free the old list
		TAdInfo = AdaptersInfoList;
		while(TAdInfo != NULL)
		{
			Mem1 = TAdInfo->NetworkAddresses;
			Mem2 = TAdInfo;
			
			TAdInfo = TAdInfo->Next;
			
			if (Mem1 != NULL)
				GlobalFreePtr(Mem1);
			GlobalFreePtr(Mem2);
		}
		
		AdaptersInfoList = NULL;
	}

	//
	// Fill the new list
	//
	if(!PacketGetAdapters())
	{
		// No info about adapters in the registry. 
		ODS("PacketPopulateAdaptersInfoList: registry scan for adapters failed!\n");
	}
#ifndef _WINNT4
	if(!PacketGetAdaptersIPH())
	{
		// IP Helper API not present. We are under WinNT 4 or TCP/IP is not installed
		ODS("PacketPopulateAdaptersInfoList: failed to get adapters from the IP Helper API!\n");
	}

	if (!PacketAddFakeNdisWanAdapter())
	{
		ODS("PacketPopulateAdaptersInfoList: adding fake NdisWan adapter failed.\n");
	}

#ifdef HAVE_DAG_API
	if(p_dagc_open == NULL)	
	{}	// dagc.dll not present on this system.
	else
	{
		if(!PacketGetAdaptersDag())
		{
			// No info about adapters in the registry. 
			ODS("PacketPopulateAdaptersInfoList: lookup of dag cards failed!\n");
		}
	}
#endif // HAVE_DAG_API

#endif // _WINNT4

	ReleaseMutex(AdaptersInfoMutex);
}

#ifndef _WINNT4

BOOLEAN PacketAddFakeNdisWanAdapter()
{
	//this function should acquire the AdaptersInfoMutex, since it's NOT called with an ADAPTER_INFO as parameter
	PADAPTER_INFO TmpAdInfo, SAdInfo;
	CHAR DialupName[MAX_WINPCAP_KEY_CHARS];
	CHAR DialupDesc[MAX_WINPCAP_KEY_CHARS];
	UINT RegQueryLen;

	//
	// Get name and description of the wan adapter from the registry
	//
	RegQueryLen = sizeof(DialupName)/sizeof(DialupName[0]);
	if (QueryWinPcapRegistryStringA(NPF_FAKE_NDISWAN_ADAPTER_NAME_REG_KEY, DialupName, &RegQueryLen, FAKE_NDISWAN_ADAPTER_NAME) == FALSE && RegQueryLen == 0)
		return FALSE;
	
	RegQueryLen = sizeof(DialupDesc)/sizeof(DialupDesc[0]);
	if (QueryWinPcapRegistryStringA(NPF_FAKE_NDISWAN_ADAPTER_DESC_REG_KEY, DialupDesc, &RegQueryLen, FAKE_NDISWAN_ADAPTER_DESCRIPTION) == FALSE && RegQueryLen == 0)
		return FALSE;

	// Scan the adapters list to see if this one is already present

	if (!WanPacketTestAdapter())
	{
		ODS("Cannot add the adapter, since it cannot be opened.");
		//the adapter cannot be opened, we do not list it, but we return t
		return FALSE;
	}

	WaitForSingleObject(AdaptersInfoMutex, INFINITE);
	
	for(SAdInfo = AdaptersInfoList; SAdInfo != NULL; SAdInfo = SAdInfo->Next)
	{
		if(strcmp(DialupName, SAdInfo->Name) == 0)
		{
			ODS("PacketAddFakeNdisWanAdapter: Adapter already present in the list\n");
			ReleaseMutex(AdaptersInfoMutex);
			return TRUE;
		}
	}

	TmpAdInfo = GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(ADAPTER_INFO));
	if (TmpAdInfo == NULL) 
	{
		ODS("PacketAddFakeNdisWanAdapter: GlobalAlloc Failed\n");
		ReleaseMutex(AdaptersInfoMutex);
		return FALSE;
	}

	strncpy(TmpAdInfo->Name, DialupName, sizeof(TmpAdInfo->Name) - 1);
	strncpy(TmpAdInfo->Description, DialupDesc, sizeof(TmpAdInfo->Description) - 1);
	TmpAdInfo->LinkLayer.LinkType = NdisMedium802_3;
	TmpAdInfo->LinkLayer.LinkSpeed = 10 * 1000 * 1000; //we emulate a fake 10MBit Ethernet
	TmpAdInfo->Flags = INFO_FLAG_NDISWAN_ADAPTER;
	memset(TmpAdInfo->MacAddress,'0',6);
	TmpAdInfo->MacAddressLen = 6;
	TmpAdInfo->NetworkAddresses = NULL;
	TmpAdInfo->NNetworkAddresses = 0;

	TmpAdInfo->Next = AdaptersInfoList;
	AdaptersInfoList = TmpAdInfo;
	ReleaseMutex(AdaptersInfoMutex);

	return TRUE;
}

#endif