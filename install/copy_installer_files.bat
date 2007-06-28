@echo off
mkdir installer\distribution		 2>nul >nul
mkdir installer\distribution\2000	 2>nul >nul
mkdir installer\distribution\NT		 2>nul >nul

rem We do no longer support Win9x
rem mkdir installer\distribution\9x		 2>nul >nul

mkdir installer\distribution\x86-64	 2>nul >nul
mkdir installer\distribution\Vista_x86	 2>nul >nul

xcopy /v /Y ..\packetNtx\driver\bin\2k\i386\npf.sys installer\distribution\2000\ >nul
xcopy /v /Y  ..\packetNtx\driver\bin\2k\i386\npf.pdb installer\distribution\2000\ >nul

xcopy /v /Y  ..\packetNtx\driver\bin\NT4\i386\free\npf.sys installer\distribution\NT\ >nul

xcopy /v /Y  ..\packetNtx\driver\bin\xp\amd64\npf.sys installer\distribution\x86-64\ >nul
xcopy /v /Y  ..\packetNtx\driver\bin\xp\amd64\npf.pdb installer\distribution\x86-64\ >nul

xcopy /v /Y  ..\packetNtx\Dll\Project\Release\Packet.dll installer\distribution\2000\ >nul
xcopy /v /Y  ..\packetNtx\Dll\Project\Release_Vista\Packet.dll installer\distribution\Vista_x86\ >nul
xcopy /v /Y  ..\packetNtx\Dll\Project\NT4_Release\Packet.dll installer\distribution\NT\ >nul
xcopy /v /Y  ..\packetNtx\Dll\WanPacket\Release\WanPacket.dll installer\distribution\2000\ >nul

xcopy /v /Y  ..\packetNtx\Dll\Project\Release\Packet.pdb installer\distribution\2000\ >nul
xcopy /v /Y  ..\packetNtx\Dll\Project\Release_Vista\Packet.pdb installer\distribution\Vista_x86\ >nul
xcopy /v /Y  ..\packetNtx\Dll\Project\NT4_Release\Packet.pdb installer\distribution\NT\ >nul
xcopy /v /Y  ..\packetNtx\Dll\WanPacket\Release\WanPacket.pdb installer\distribution\2000\ >nul

rem We do no longer support Win9x
rem echo REMEMBER TO COPY npf.vxd
rem pause
rem copy ..\packet9x\DLL\Project\Release\Packet.dll installer\distribution\9x\

xcopy /v /Y  ..\wpcap\PRJ\Release_REMOTE\wpcap.dll installer\distribution\ >nul
xcopy /v /Y  ..\wpcap\PRJ\Release_REMOTE\rpcapd.exe installer\distribution\2000\ >nul
xcopy /v /Y  ..\wpcap\PRJ\Release_REMOTE\wpcap.pdb installer\distribution\ >nul
xcopy /v /Y  ..\wpcap\PRJ\Release_REMOTE\rpcapd.pdb installer\distribution\2000\ >nul


rem copy daemon_mgm\Release\daemon_mgm.exe installer\distribution\2000\
rem copy NetMonInstaller\objfre_w2k_x86\i386\NetMonInstaller.exe installer\distribution\2000\
rem copy npf_mgm\Release\npf_mgm.exe installer\distribution\2000\

xcopy /v /Y  "WinPcap Installer Helper\Release\WinPcapInstall.dll" installer\distribution\2000\ >nul

xcopy /v /Y  ..\wpcap\libpcap\rpcapd\win32-pthreads\pthreadVC.dll installer\distribution\ >nul
rem this nasty hack is needed to fix some time zone issues with the CVS...
touch installer\distribution\pthreadVC.dll >nul

