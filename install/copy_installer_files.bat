@echo off
mkdir installer\distribution
mkdir installer\distribution\2000
mkdir installer\distribution\NT
mkdir installer\distribution\9x
mkdir installer\distribution\x86-64

copy \i386\npf.sys installer\distribution\2000\
copy \i386\npf.pdb installer\distribution\2000\
copy \i386\free\npf.sys installer\distribution\NT\
copy \amd64\npf.sys installer\distribution\x86-64\
copy \amd64\npf.pdb installer\distribution\x86-64\

copy ..\packetNtx\Dll\Project\Release\Packet.dll installer\distribution\2000\
copy ..\packetNtx\Dll\Project\NT4_Release\Packet.dll installer\distribution\NT\
copy ..\packetNtx\Dll\WanPacket\Release\WanPacket.dll installer\distribution\2000\
echo REMEMBER TO COPY npf.vxd
pause
copy ..\packet9x\DLL\Project\Release\Packet.dll installer\distribution\9x\
copy ..\wpcap\PRJ\Release_REMOTE\wpcap.dll installer\distribution\
copy ..\wpcap\PRJ\Release_REMOTE\rpcapd.exe installer\distribution\2000\
copy daemon_mgm\Release\daemon_mgm.exe installer\distribution\2000\
copy NetMonInstaller\objfre_w2k_x86\i386\NetMonInstaller.exe installer\distribution\2000\
copy npf_mgm\Release\npf_mgm.exe installer\distribution\2000\

copy ..\wpcap\libpcap\rpcapd\win32-pthreads\pthreadVC.dll installer\distribution\
rem this nasty hack is needed to fix some time zone issues with the CVS...
touch installer\distribution\pthreadVC.dll

