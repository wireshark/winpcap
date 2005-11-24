@echo off

echo Copying compiled DLLs
copy ..\..\..\winpcap\packetNtx\Dll\Project\NT4_Debug_OEM\packet.dll .\Binaries\packetnt.dll
copy ..\..\..\winpcap\packetNtx\Dll\Project\Debug_PacketWan\packet.dll .\Binaries\packet2k.dll

echo Copying drivers
copy \i386\npf.sys .\Binaries\npf2k.sys
copy \i386\Checked\npf.sys .\Binaries\npfnt.sys

