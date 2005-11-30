@echo off
if NOT "%NPF_COMPILED%" == "1" (
		set Include=%BASEDIR%\src\network\inc;%BASEDIR%\inc;%Include%
		set NPF_COMPILED=1
			)
set USER_C_DEFINES=-D__NPF_NT4__ -DNDIS30 -DWPCAP_OEM
set TARGETPATH=..\..\..\winpcap_oem\PacketNtx\driver\bin\NT4

REM this enables the generation of pdb files for both free and debug mode
rem set USE_PDB=1
rem set NTDEBUGTYPE=windbg

mkdir driver\bin 2> nul
mkdir driver\bin\NT4 2> nul
mkdir driver\bin\NT4\i386\ 2> nul
mkdir driver\bin\NT4\i386\free 2> nul
mkdir driver\bin\NT4\i386\checked 2> nul

copy driver\NPF_NT4.RC ..\..\winpcap\packetntx\driver\NPF.RC

pushd ..\..\winpcap\packetntx\driver

build -cefw

popd

if "%DDKBUILDENV%" == "free" (
		copy driver\bin\NT4\i386\free\npf.sys driver\bin\NT4\i386\
		copy driver\bin\NT4\i386\free\npf.pdb driver\bin\NT4\i386\
			)

if "%DDKBUILDENV%" == "checked" (
		copy driver\bin\NT4\i386\checked\npf.sys driver\bin\NT4\i386\
		copy driver\bin\NT4\i386\checked\npf.pdb driver\bin\NT4\i386\
			)
set TARGETPATH=
set USER_C_DEFINES=
rem set USE_PDB=
rem set NTDEBUGTYPE=
