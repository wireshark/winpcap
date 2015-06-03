# Microsoft Developer Studio Project File - Name="StaticWinpcapOem" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=StaticWinpcapOem - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "StaticWinpcapOem.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "StaticWinpcapOem.mak" CFG="StaticWinpcapOem - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "StaticWinpcapOem - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "StaticWinpcapOem - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "StaticWinpcapOem - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "./" /I "../../../winpcap/common" /I "../../../common" /I "../../../winpcap/wpcap/libpcap" /I "../../../winpcap/wpcap/libpcap/lbl/" /I "../../../winpcap/wpcap/libpcap/bpf/" /I "../../../winpcap/wpcap/libpcap/win32/include/" /I "../../../winpcap/wpcap/Win32-Extensions" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "STATIC_LIB" /D "YY_NEVER_INTERACTIVE" /D yylval=pcap_lval /D "_USRDLL" /D "LIBPCAP_EXPORTS" /D "HAVE_STRERROR" /D "__STDC__" /D "INET6" /D "_WINDOWS" /D SIZEOF_CHAR=1 /D SIZEOF_SHORT=2 /D SIZEOF_INT=4 /D "HAVE_ADDRINFO" /D _U_= /D "HAVE_SNPRINTF" /D "HAVE_VSNPRINTF" /D "HAVE_REMOTE" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\wpcap.lib"

!ELSEIF  "$(CFG)" == "StaticWinpcapOem - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "./" /I "../../../winpcap/common" /I "../../../common" /I "../../../winpcap/wpcap/libpcap" /I "../../../winpcap/wpcap/libpcap/lbl/" /I "../../../winpcap/wpcap/libpcap/bpf/" /I "../../../winpcap/wpcap/libpcap/win32/include/" /I "../../../winpcap/wpcap/Win32-Extensions" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "STATIC_LIB" /D "YY_NEVER_INTERACTIVE" /D yylval=pcap_lval /D "_USRDLL" /D "LIBPCAP_EXPORTS" /D "HAVE_STRERROR" /D "__STDC__" /D "INET6" /D "_WINDOWS" /D SIZEOF_CHAR=1 /D SIZEOF_SHORT=2 /D SIZEOF_INT=4 /D "HAVE_ADDRINFO" /D _U_= /D "HAVE_SNPRINTF" /D "HAVE_VSNPRINTF" /D "HAVE_REMOTE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\wpcap.lib"

!ENDIF 

# Begin Target

# Name "StaticWinpcapOem - Win32 Release"
# Name "StaticWinpcapOem - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Crypto.cpp
# End Source File
# Begin Source File

SOURCE=.\PacketWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\Resources.cpp
# End Source File
# Begin Source File

SOURCE=.\Security.cpp
# End Source File
# Begin Source File

SOURCE=.\SerivcesInstallers.cpp
# End Source File
# Begin Source File

SOURCE=.\WinPcapOem.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Crypto.h
# End Source File
# Begin Source File

SOURCE=.\idls.h
# End Source File
# Begin Source File

SOURCE=.\netcfgn.h
# End Source File
# Begin Source File

SOURCE=.\netcfgx.h
# End Source File
# Begin Source File

SOURCE=.\NetMonInstaller.h
# End Source File
# Begin Source File

SOURCE=.\Npf2k.h

!IF  "$(CFG)" == "StaticWinpcapOem - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__NPF2K="$(WkspDir)\binaries\npf2k.sys"	
# Begin Custom Build - Building the include file from npf2k.sys
WkspDir=.
InputPath=.\Npf2k.h

"Npf2k.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(WkspDir)\..\bin2c\BIN2C.exe binaries\npf2k.sys Npf2k Npf2k

# End Custom Build

!ELSEIF  "$(CFG)" == "StaticWinpcapOem - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__NPF2K="$(WkspDir)\binaries\npf2k.sys"	
# Begin Custom Build - Building the include file from npf2k.sys
WkspDir=.
InputPath=.\Npf2k.h

"Npf2k.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(WkspDir)\..\bin2c\BIN2C.exe binaries\npf2k.sys Npf2k Npf2k

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Npfnt.h

!IF  "$(CFG)" == "StaticWinpcapOem - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__NPFNT="$(WkspDir)\binaries\npfnt.sys"	
# Begin Custom Build - Building the include file from npfnt.sys
WkspDir=.
InputPath=.\Npfnt.h

"Npfnt.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(WkspDir)\..\bin2c\BIN2C.exe binaries\npfnt.sys NpfNT NpfNT

# End Custom Build

!ELSEIF  "$(CFG)" == "StaticWinpcapOem - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__NPFNT="$(WkspDir)\binaries\npfnt.sys"	
# Begin Custom Build - Building the include file from npfnt.sys
WkspDir=.
InputPath=.\Npfnt.h

"Npfnt.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(WkspDir)\..\bin2c\BIN2C.exe binaries\npfnt.sys NpfNT NpfNT

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Packet2k.h

!IF  "$(CFG)" == "StaticWinpcapOem - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__PACKE="$(WkspDir)\binaries\packet2k.dll"	
# Begin Custom Build - Building the include file from packet2k.dll
WkspDir=.
InputPath=.\Packet2k.h

"Packet2k.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(WkspDir)\..\bin2c\BIN2C.exe binaries\Packet2k.dll Packet2k Packet2k

# End Custom Build

!ELSEIF  "$(CFG)" == "StaticWinpcapOem - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__PACKE="$(WkspDir)\binaries\packet2k.dll"	
# Begin Custom Build - Building the include file from packet2k.dll
WkspDir=.
InputPath=.\Packet2k.h

"Packet2k.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(WkspDir)\..\bin2c\BIN2C.exe binaries\Packet2k.dll Packet2k Packet2k

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PacketNT.h

!IF  "$(CFG)" == "StaticWinpcapOem - Win32 Release"

USERDEP__PACKET="$(WkspDir)\binaries\packetnt.dll"	
# Begin Custom Build - Building the include file from packetnt.dll
WkspDir=.
InputPath=.\PacketNT.h

"PacketNT.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(WkspDir)\..\bin2c\BIN2C.exe binaries\Packetnt.dll PacketNT PacketNT

# End Custom Build

!ELSEIF  "$(CFG)" == "StaticWinpcapOem - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__PACKET="$(WkspDir)\binaries\packetnt.dll"	
# Begin Custom Build - Building the include file from packetnt.dll
WkspDir=.
InputPath=.\PacketNT.h

"PacketNT.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(WkspDir)\..\bin2c\BIN2C.exe binaries\Packetnt.dll PacketNT PacketNT

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pch.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resrc1.h
# End Source File
# Begin Source File

SOURCE=.\Security.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\WinpcapOem.h
# End Source File
# End Group
# Begin Group "Wpcap Files"

# PROP Default_Filter ""
# Begin Group "Wpcap Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\bpf_dump.c
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\bpf_image.c
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\etherent.c
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\fad-win32.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\gencode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\grammar.c
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\inet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\nametoaddr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\optimize.c
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-new.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-remote.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-win32.c"
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\pcap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\savefile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\scanner.c
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\sockutils.c
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\Win32-Extensions\Win32-Extensions.c"
# End Source File
# End Group
# Begin Group "Wpcap Include Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\acconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\arcnet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\atmuni31.h
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\ethertype.h
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\gencode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\llc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\nlpid.h
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-bpf.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-dag.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-dos.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-int.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-namedb.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-nit.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-pf.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-remote.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-septel.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\pcap-stdinc.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\pcap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\pcap1.h
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\pf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\ppp.h
# End Source File
# Begin Source File

SOURCE="..\..\..\winpcap\wpcap\libpcap\remote-ext.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\sll.h
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\sockutils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\sunatmpos.h
# End Source File
# Begin Source File

SOURCE=..\..\..\winpcap\wpcap\libpcap\tokdefs.h
# End Source File
# End Group
# End Group
# End Target
# End Project
