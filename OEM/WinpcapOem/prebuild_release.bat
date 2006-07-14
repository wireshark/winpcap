@echo off

echo Copying compiled DLLs
copy ..\..\packetNtx\Dll\Project\Packet_Release_NT4\packet.dll .\Binaries\packetnt.dll
copy ..\..\packetNtx\Dll\Project\Packet_Release_2k\packet.dll .\Binaries\packet2k.dll

echo Copying drivers
copy ..\..\packetNtx\Driver\bin\2k\i386\npf.sys .\Binaries\npf2k.sys
copy ..\..\packetNtx\Driver\bin\NT4\i386\free\npf.sys .\Binaries\npfnt.sys
copy ..\..\packetNtx\Driver\bin\xp\AMD64\npf.sys .\Binaries\npfx64.sys
