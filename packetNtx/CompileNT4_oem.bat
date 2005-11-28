@echo off
if NOT "%NPF_COMPILED%" == "1" (
		set Include=%BASEDIR%\src\network\inc;%BASEDIR%\inc;%Include%
		set NPF_COMPILED=1
			)
set USER_C_DEFINES=-D__NPF_NT4__ -DNDIS30 -DWPCAP_OEM
set TARGETPATH=..\..\..\winpcap_oem\PacketNtx\driver\bin\NT4

mkdir driver\bin 2> nul
mkdir driver\bin\NT4 2> nul
mkdir driver\bin\NT4\i386\ 2> nul
mkdir driver\bin\NT4\i386\free 2> nul
mkdir driver\bin\NT4\i386\checked 2> nul

pushd ..\..\winpcap\packetntx\driver

build -cefw

popd

copy driver\bin\NT4\i386\%DDKBUILDENV%\*.* driver\bin\NT4\i386\

set TARGETPATH=
set USER_C_DEFINES=
