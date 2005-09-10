/*
 * Copyright (c) 2005 CACE Technologies
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
 * 3. Neither the name of the Politecnico di Torino nor the names of its
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

//
// Original names
//
#define NPF_DRIVER_NAME							"WTB"
#define NPF_DRIVER_NAME_WIDECHAR				L"WTB"

//
// Derived strings
//

#define NPF_SERVICE_DESC_WIDECHAR				L"WinPcap Packet Driver"
#define NPF_SERVICE_REGISTRY_LOCATION_WIDECHAR	L"SYSTEM\\CurrentControlSet\\Services\\" NPF_DRIVER_NAME_WIDECHAR
#define NPF_COMPLETE_SERVICE_REGISTRY_LOCATION	"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\" NPF_DRIVER_NAME
#define NPF_DRIVER_PATH_WIDECHAR				L"system32\\drivers\\"
#define NPF_DRIVER_BINARY_WIDECHAR				NPF_DRIVER_NAME_WIDECHAR L".sys"
#define NPF_DRIVER_COMPLETE_WIDECHAR			NPF_DRIVER_PATH_WIDECHAR NPF_DRIVER_BINARY_WIDECHAR

#define NPF_SERVICE_DESC_ASCII					"WinPcap Packet Driver (" NPF_DRIVER_NAME ")"
#define NPF_DRIVER_PATH_ASCII					"system32\\drivers\\"
#define NPF_DRIVER_BINARY_ASCII					NPF_DRIVER_NAME ".sys"

#define NPF_DEVICE_NAMES_PREFIX NPF_DRIVER_NAME "_"
#define NPF_DEVICE_NAMES_PREFIX_WIDECHAR NPF_DRIVER_NAME_WIDECHAR L"_"
#define NPF_DRIVER_COMPLETE_DRIVER_PREFIX "\\Device\\" NPF_DRIVER_NAME "_"
#define NPF_USER_EVENTS_NAMES NPF_DRIVER_NAME_WIDECHAR L"0000000000"
#define NPF_KERNEL_EVENTS_NAMES L"\\BaseNamedObjects\\" NPF_USER_EVENTS_NAMES L"\0"

#define FAKE_NDISWAN_ADAPTER_NAME "\\Device\\" NPF_DRIVER_NAME "_GenericDialupAdapter"  ///< Name of a fake ndiswan adapter that is always available on 2000/XP/2003, used to capture NCP/LCP packets
#define FAKE_NDISWAN_ADAPTER_DESCRIPTION "Adapter for generic dialup and VPN capture"	///< Description of a fake ndiswan adapter that is always available on 2000/XP/2003, used to capture NCP/LCP packets


//
// WinPcap Global Registry Key
//
#define WINPCAP_GLOBAL_KEY "SOFTWARE\\CaceWinPcap"
#define WINPCAP_GLOBAL_KEY_WIDECHAR L"SOFTWARE\\CaceWinPcap"
#define MAX_WINPCAP_KEY_CHARS 512
