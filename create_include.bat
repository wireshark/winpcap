@echo off

IF "%2"=="" (set WPDPACKDESTDIR=.\WpdPack\) ELSE (set WPDPACKDESTDIR=%2)

IF ""=="%1" (set WINPCAPSOURCEDIR=.\) ELSE (set WINPCAPSOURCEDIR=%1) 

echo Creating \Include folder
mkdir %WPDPACKDESTDIR%  2>nul >nul
mkdir %WPDPACKDESTDIR%\Include  2>nul >nul

xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\libpcap\Win32\Include\bittypes.h 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\libpcap\Win32\Include\Gnuc.h 			%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\libpcap\Win32\Include\ip6_misc.h		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\libpcap\rpcapd\win32-pthreads\pthread.h	%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\libpcap\rpcapd\win32-pthreads\sched.h 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\libpcap\rpcapd\win32-pthreads\semaphore.h	%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\libpcap\PCAP-INT.H		 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\libpcap\pcap-bpf.h		 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\libpcap\pcap-stdinc.h		 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\libpcap\PCAP.H			 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\libpcap\remote-ext.h		 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\Win32-Extensions\Win32-Extensions.h 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\packetNtx\driver\bucket_lookup.h	 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\packetNtx\driver\count_packets.h	 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\packetNtx\driver\memory_t.h		 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\packetNtx\driver\normal_lookup.h	 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\packetNtx\driver\tcp_session.h		 	%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\packetNtx\driver\time_calls.h		 	%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\packetNtx\driver\tme.h			 	%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\Common\Packet32.h			 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\Common\Devioctl.h			 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\Common\Ntddndis.h			 		%WPDPACKDESTDIR%\Include\	
xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\Common\Ntddpack.h			 		%WPDPACKDESTDIR%\Include\	

echo Creating \Include\NET folder
mkdir %WPDPACKDESTDIR%\Include\NET  2>nul >nul

xcopy /s/e/v /Y %WINPCAPSOURCEDIR%\wpcap\libpcap\bpf\net\bpf.h 				%WPDPACKDESTDIR%\Include\NET\	

echo Folder \Include created successfully
set WPDPACKDESTDIR=
set WINPCAPSOURCEDIR=
