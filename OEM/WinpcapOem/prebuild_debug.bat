@echo off

echo Copying Debug DLLs

if "%1"=="x64" (
echo Copying compiled DLLs for x64
copy "..\..\packetNtx\Dll\Project\Packet (NetMon) Debug\x64\packet.dll"		.\Binaries\x64\packet2k.dll
copy "..\..\PacketNtx\Dll\Project\Packet (NoNetMon) Debug\x64\packet.dll"	.\Binaries\x64\packetVista.dll
echo Copying drivers for x64
copy ..\..\packetNtx\Driver\bin\xp\AMD64\npf.sys							.\Binaries\x64\npfx64.sys
)

if "%1"=="x86" (
echo Copying compiled DLLs for x86
copy "..\..\packetNtx\Dll\Project\Packet (NT4) Debug\x86\packet.dll"		.\Binaries\x86\packetnt.dll
copy "..\..\packetNtx\Dll\Project\Packet (NetMon) Debug\x86\packet.dll"		.\Binaries\x86\packet2k.dll
copy "..\..\PacketNtx\Dll\Project\Packet (NoNetMon) Debug\x86\packet.dll"	.\Binaries\x86\packetVista.dll
echo Copying drivers for x86 and x64
copy ..\..\packetNtx\Driver\bin\2k\i386\npf.sys								.\Binaries\x86\npf2k.sys
copy ..\..\packetNtx\Driver\bin\NT4\i386\free\npf.sys						.\Binaries\x86\npfnt.sys
copy ..\..\packetNtx\Driver\bin\xp\AMD64\npf.sys							.\Binaries\x64\npfx64.sys
)
