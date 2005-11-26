@echo off

echo Copying compiled DLLs
copy ..\..\packetNtx\Dll\Project\Packet_Debug_NT4\packet.dll .\Binaries\packetnt.dll
copy ..\..\packetNtx\Dll\Project\Packet_Debug_2k\packet.dll .\Binaries\packet2k.dll

echo Copying drivers
copy \i386\npf.sys .\Binaries\npf2k.sys
copy \i386\Checked\npf.sys .\Binaries\npfnt.sys

