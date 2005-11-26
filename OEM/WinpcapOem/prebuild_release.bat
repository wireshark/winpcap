@echo off

echo Copying compiled DLLs
copy ..\..\..\winpcap\packetNtx\Dll\Project\Packet_Release_NT4\packet.dll .\Binaries\packetnt.dll
copy ..\..\..\winpcap\packetNtx\Dll\Project\Packet_Release_2k\packet.dll .\Binaries\packet2k.dll

echo Copying drivers
copy \i386\npf.sys .\Binaries\npf2k.sys
copy \i386\free\npf.sys .\Binaries\npfnt.sys

