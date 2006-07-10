@echo off

echo Copying compiled DLLs
copy ..\..\packetNtx\Dll\Project\Packet_Debug_NT4\packet.dll .\Binaries\packetnt.dll
copy ..\..\packetNtx\Dll\Project\Packet_Debug_2k\packet.dll .\Binaries\packet2k.dll

echo Copying drivers
copy ..\..\packetNtx\Driver\bin\2k\i386\npf.sys .\Binaries\npf2k.sys
copy ..\..\packetNtx\Driver\bin\NT4\i386\checked\npf.sys .\Binaries\npfnt.sys

