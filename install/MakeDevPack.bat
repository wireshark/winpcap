@echo off
echo MakeDevPack: Builds the WinPcap developer's pack from the compiled source code
echo              Copyright 2000-2002 Politecnico di Torino


if %1=="" goto usage
if %2=="" goto usage

if not exist %2\nul md %2
if not exist %2\Drivers\nul md %2\Drivers
if not exist %2\Examples\nul md %2\Examples
if not exist %2\Include\nul md %2\Include
if not exist %2\Include\net\nul md %2\Include\net
if not exist %2\Lib\nul md %2\Lib

echo on
copy %1\wpcap\lib\wpcap.lib %2\Lib
copy %1\packetntx\dll\project\release\packet.lib %2\Lib
copy %1\common\Devioctl.h %2\Include
copy %1\common\NTDDNDIS.H %2\Include
rem copy %1\common\Ntddpack.h %2\Include
copy %1\common\PACKET32.H %2\Include
copy %1\wpcap\libpcap\win32\include\gnuc.h %2\Include

copy %1\wpcap\libpcap\win32\include\pthread.h %2\Include
copy %1\wpcap\libpcap\win32\include\sched.h %2\Include
copy %1\wpcap\libpcap\win32\include\semaphore.h %2\Include

copy %1\wpcap\libpcap\win32\include\bittypes.h %2\Include
copy %1\wpcap\libpcap\pcap.h %2\Include
copy %1\wpcap\libpcap\Pcap-int.h %2\Include
copy %1\wpcap\libpcap\bpf\net\bpf.h %2\Include\net
copy %1\packetntx\dll\project\packet.a %2\Lib
copy %1\wpcap\lib\libwpcap.a %2\Lib
copy %1\wpcap\libpcap\pcap-stdinc.h %2\Include
copy %1\wpcap\win32-extensions\win32-extensions.h %2\Include

copy %1\packetntx\driver\bucket_lookup.h %2\Include
copy %1\packetntx\driver\count_packets.h %2\Include
copy %1\packetntx\driver\memory_t.h %2\Include
copy %1\packetntx\driver\normal_lookup.h %2\Include
copy %1\packetntx\driver\tcp_session.h %2\Include
copy %1\packetntx\driver\time_calls.h %2\Include
copy %1\packetntx\driver\tme.h %2\Include

copy %1\wpcap\win32-extensions\keyparser\key_parser.h %2\Include
copy %1\wpcap\win32-extensions\keyparser\key_parser.lib %2\Lib

@echo off
goto end

:usage
echo usage: MakeDevPack winpcapdir devpackdir 
goto end

:end

