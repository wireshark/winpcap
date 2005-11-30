@echo off

mkdir driver\bin 2> nul
mkdir driver\bin\2k 2> nul

set USER_C_DEFINES=-DNDIS50 -DWPCAP_OEM
set TARGETPATH=..\..\..\winpcap_oem\PacketNtx\driver\bin\2k

copy driver\NPF_NT5.RC ..\..\winpcap\packetntx\driver\NPF.RC

pushd ..\..\winpcap\packetntx\driver

prefast build -cefw

popd

set TARGETPATH=
set USER_C_DEFINES=

