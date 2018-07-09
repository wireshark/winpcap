@echo off


if "%_BUILDARCH%"=="x86" goto x86_build
if "%_BUILDARCH%"=="AMD64" goto amd64_build
if "%_BUILDARCH%"=="IA64" goto ia64_build
if "%_BUILDARCH%"=="" goto nt4_build

echo ******************************************************
echo   ERROR: unknown build architecture (%_BUILDARCH%)
echo ******************************************************

goto end

:x86_build

echo ******************************************************
echo *  Compiling the driver for Windows NT5.x 32 bit     *
echo ******************************************************

mkdir .\driver\bin 2> nul
mkdir .\driver\bin\2k 2> nul

copy driver\NPF.RC ..\..\winpcap\packetntx\driver\NPF.RC

//set TARGETPATH=..\..\..\winpcap_oem\PacketNtx\driver\bin\2k
set NPF_C_DEFINES=-DNDIS50 -DWPCAP_OEM
rem ** enable the following line to enable the TME extensions **
rem set NPF_TME_FILES=tme.c count_packets.c tcp_session.c functions.c bucket_lookup.c normal_lookup.c win_bpf_filter_init.c
rem set NPF_C_DEFINES=%NPF_C_DEFINES% -DHAVE_BUGGY_TME_SUPPORT

set NPF_JIT_FILES=jitter.c

pushd ..\..\winpcap\packetntx\driver

prefast build -cefw

popd

set TARGETPATH=
set NPF_C_DEFINES=
set NPF_TME_FILES=
set NPF_JIT_FILES=

goto end

:amd64_build

echo *******************************************************
echo *  Compiling the driver for Windows NT5.x x64 (AMD64) *
echo *******************************************************

mkdir driver\bin 2> nul
mkdir driver\bin\xp 2> nul

copy driver\NPF.RC ..\..\winpcap\packetntx\driver\NPF.RC

set TARGETPATH=..\..\..\winpcap_oem\PacketNtx\driver\bin\xp
set NPF_C_DEFINES=-DNDIS50 -DWPCAP_OEM

rem
rem The TME extensions and the JIT is not supported on x64, at the moment
rem
set NPF_TME_FILES=
set NPF_JIT_FILES=

pushd ..\..\winpcap\packetntx\driver

build -cefw

popd

set TARGETPATH=
set NPF_C_DEFINES=
set NPF_TME_FILES=
set NPF_JIT_FILES=

goto end

:ia64_build

echo *******************************************************
echo *  Compiling the driver for Windows NT5.x IA64        *
echo *******************************************************

echo ..
echo ERROR: IA64 is not supported by WinPcap.
echo ..

goto end

:nt4_build

echo *******************************************************
echo *  Compiling the driver for Windows NT4               *
echo *                                                     *
echo * NOTE: the script didn't detect the actual BUILD env *
echo *       so it assumed NT4 DDK.                        *
echo *******************************************************

mkdir .\driver\bin 2> nul
mkdir .\driver\bin\NT4 2> nul
mkdir .\driver\bin\NT4\i386 2> nul
mkdir .\driver\bin\NT4\i386\free 2> nul
mkdir .\driver\bin\NT4\i386\checked 2> nul

copy driver\NPF.RC ..\..\winpcap\packetntx\driver\NPF.RC

set NPF_C_DEFINES=-DNDIS30 -D__NPF_NT4__ -DWPCAP_OEM

rem ** enable the following line to enable the TME extensions **
rem set NPF_TME_FILES=tme.c count_packets.c tcp_session.c functions.c bucket_lookup.c normal_lookup.c win_bpf_filter_init.c
rem set NPF_C_DEFINES=%NPF_C_DEFINES% -DHAVE_BUGGY_TME_SUPPORT

set NPF_JIT_FILES=jitter.c

if NOT "%NPF_COMPILED%" == "1" (
		set Include=%BASEDIR%\src\network\inc;%BASEDIR%\inc;%Include%
		set NPF_COMPILED=1
			)

pushd ..\..\winpcap\packetntx\driver

mkdir .\bin 2> nul
mkdir .\bin\i386 2> nul
mkdir .\bin\i386\free 2> nul
mkdir .\bin\i386\checked 2> nul

build -cefw

popd

xcopy /s /e /v ..\..\winpcap\packetntx\driver\bin\i386\* .\driver\bin\NT4\i386\

set TARGETPATH=
set NPF_C_DEFINES=
set NPF_TME_FILES=
set NPF_JIT_FILES=

goto end

:end