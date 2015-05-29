@echo off
mkdir installer\distribution     2>nul >nul
mkdir installer\distribution\x86   2>nul >nul

mkdir installer\distribution\x64   2>nul >nul
mkdir installer\distribution\Vista_x86   2>nul >nul
mkdir installer\distribution\Vista_x64   2>nul >nul

rem
rem  Driver
rem 
xcopy /v /Y ..\packetNtx\driver\bin\i386\npf.sys installer\distribution\x86\ >nul
xcopy /v /Y  ..\packetNtx\driver\bin\i386\npf.pdb installer\distribution\x86\ >nul
xcopy /v /Y  ..\packetNtx\driver\bin\amd64\npf.sys installer\distribution\x64\ >nul
xcopy /v /Y  ..\packetNtx\driver\bin\amd64\npf.pdb installer\distribution\x64\ >nul

rem 
rem Packet DLL
rem

rem 
rem Windows 2000, XP, 2003 (x86 and x64)
rem
xcopy /v /Y  ..\packetntx\dll\project\Release\x86\Packet.dll installer\distribution\x86\ >nul
xcopy /v /Y  ..\packetntx\dll\project\Release\x86\Packet.pdb installer\distribution\x86\ >nul
xcopy /v /Y  ..\packetntx\dll\project\Release\x64\Packet.dll installer\distribution\x64\ >nul
xcopy /v /Y  ..\packetntx\dll\project\Release\x64\Packet.pdb installer\distribution\x64\ >nul

rem
rem Windows Vista (x86 and x64)
rem
xcopy /v /Y  "..\packetntx\dll\project\Release No NetMon\x86\Packet.dll" installer\distribution\Vista_x86\ >nul
xcopy /v /Y  "..\packetntx\dll\project\Release No NetMon\x64\Packet.dll" installer\distribution\Vista_x64\ >nul
xcopy /v /Y  "..\packetntx\dll\project\Release No NetMon\x86\Packet.pdb" installer\distribution\Vista_x86\ >nul
xcopy /v /Y  "..\packetntx\dll\project\Release No NetMon\x64\Packet.pdb" installer\distribution\Vista_x64\ >nul

rem
rem Wpcap.dll (x86 and x64)
rem
xcopy /v /Y  ..\wpcap\prj\Release\x86\wpcap.dll installer\distribution\x86\ >nul
xcopy /v /Y  ..\wpcap\prj\Release\x86\wpcap.pdb installer\distribution\x86\ >nul
xcopy /v /Y  ..\wpcap\prj\Release\x64\wpcap.dll installer\distribution\x64\ >nul
xcopy /v /Y  ..\wpcap\prj\Release\x64\wpcap.pdb installer\distribution\x64\ >nul

rem 
rem rpcapd (x86 only)
rem
xcopy /v /Y  ..\wpcap\libpcap\rpcapd\Release\x86\rpcapd.exe installer\distribution\x86\ >nul
xcopy /v /Y  ..\wpcap\libpcap\rpcapd\Release\x86\rpcapd.pdb installer\distribution\x86\ >nul

rem 
rem installer helper (x86 as the installer is 32 bit)
rem
xcopy /v /Y  ".\WinPcap Installer Helper\Release\x86\WinPcapInstall.dll" installer\distribution\x86\ >nul
xcopy /v /Y  ".\WinPcap Installer Helper\Release\x86\WinPcapInstall.pdb" installer\distribution\x86\ >nul

xcopy /v /Y  ..\wpcap\libpcap\rpcapd\win32-pthreads\pthreadVC.dll installer\distribution\x86\ >nul
rem this nasty hack is needed to fix some time zone issues with the CVS...
touch installer\distribution\x86\pthreadVC.dll >nul

