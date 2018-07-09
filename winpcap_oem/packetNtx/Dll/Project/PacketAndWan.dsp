# Microsoft Developer Studio Project File - Name="PacketAndWan" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=PacketAndWan - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PacketAndWan.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PacketAndWan.mak" CFG="PacketAndWan - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PacketAndWan - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PacketAndWan - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PacketAndWan - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../dag/include" /I "../../../dag/drv/windows" /I "../../../common" /I "..\..\driver" /D "NDEBUG" /D "HAVE_DAG_API" /D "WIN32" /D "_WINDOWS" /D "WPCAP_OEM" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Iphlpapi.lib version.lib npptools.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release/Packet.dll"

!ELSEIF  "$(CFG)" == "PacketAndWan - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../dag/include" /I "../../../dag/drv/windows" /I "../../../common" /I "..\..\driver" /D "HAVE_DAG_API" /D "WIN32" /D "_WINDOWS" /D "WPCAP_OEM" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Iphlpapi.lib version.lib npptools.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/Packet.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "PacketAndWan - Win32 Release"
# Name "PacketAndWan - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\AdInfo.c
# End Source File
# Begin Source File

SOURCE=..\..\driver\bucket_lookup.c
# End Source File
# Begin Source File

SOURCE=..\..\driver\count_packets.c
# End Source File
# Begin Source File

SOURCE=..\..\driver\functions.c
# End Source File
# Begin Source File

SOURCE=..\..\driver\normal_lookup.c
# End Source File
# Begin Source File

SOURCE=..\PACKET32.C
# End Source File
# Begin Source File

SOURCE=..\..\driver\tcp_session.c
# End Source File
# Begin Source File

SOURCE=..\..\driver\tme.c
# End Source File
# Begin Source File

SOURCE=..\WanPacket\WanPacket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\driver\win_bpf_filter.c
# End Source File
# Begin Source File

SOURCE=..\..\driver\win_bpf_filter_init.c
# End Source File
# End Group
# Begin Group "Include Files"

# PROP Default_Filter "*.h*"
# Begin Source File

SOURCE=..\..\..\Common\Packet32.h
# End Source File
# Begin Source File

SOURCE=..\WanPacket\WanPacket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Common\WpcapNames.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Packet.def
# End Source File
# Begin Source File

SOURCE=..\version.rc
# End Source File
# End Target
# End Project
