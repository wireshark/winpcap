# Microsoft Developer Studio Project File - Name="wpcap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=wpcap - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wpcap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wpcap.mak" CFG="wpcap - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wpcap - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wpcap - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wpcap - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBPCAP_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../libpcap/" /I "../libpcap/bpf" /I "../libpcap/lbl" /I "../libpcap/Win32/Include" /I "../libpcap/Win32/Include/ipv6kit" /I "../../common" /I "../Win32-Extensions" /D "NDEBUG" /D HAVE_ADDRINFO=1 /D "YY_NEVER_INTERACTIVE" /D yylval=pcap_lval /D "_USRDLL" /D "LIBPCAP_EXPORTS" /D "HAVE_STRERROR" /D "__STDC__" /D "INET6" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D SIZEOF_CHAR=1 /D SIZEOF_SHORT=2 /D SIZEOF_INT=4 /D "WPCAP" /D "REMOTE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ../libpcap/win32/prj/release/libpcap.lib ../../Common/pthreadVC.lib /nologo /dll /machine:I386 /nodefaultlib:"libcmtd.lib" /def:".\wpcap.def" /implib:"../lib/wpcap.lib" /FORCE:MULTIPLE /WARN:0
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "wpcap - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBPCAP_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../libpcap/" /I "../libpcap/bpf" /I "../libpcap/lbl" /I "../libpcap/Win32/Include" /I "../libpcap/Win32/Include/ipv6kit" /I "../../common" /I "../Win32-Extensions" /D "_DEBUG" /D "HAVE_ADDRINFO" /D "YY_NEVER_INTERACTIVE" /D yylval=pcap_lval /D "_USRDLL" /D "LIBPCAP_EXPORTS" /D "HAVE_STRERROR" /D "__STDC__" /D "INET6" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D SIZEOF_CHAR=1 /D SIZEOF_SHORT=2 /D SIZEOF_INT=4 /D "WPCAP" /D "REMOTE" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ../libpcap/win32/prj/debug/libpcap.lib ../../Common/pthreadVC.lib /nologo /dll /debug /machine:I386 /def:".\wpcap.def" /implib:"../lib/wpcap.lib" /pdbtype:sept /FORCE:MULTIPLE /WARN:0
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "wpcap - Win32 Release"
# Name "wpcap - Win32 Debug"
# Begin Group "Extensions"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\Win32-Extensions\Win32-Extensions.c"
# End Source File
# End Group
# Begin Group "Libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\Packet.lib
# End Source File
# End Group
# Begin Source File

SOURCE="..\libpcap\pcap-new.c"
# End Source File
# Begin Source File

SOURCE="..\libpcap\pcap-remote.c"
# End Source File
# Begin Source File

SOURCE=..\libpcap\sockutils.c
# End Source File
# Begin Source File

SOURCE="..\Win32-Extensions\version.rc"
# End Source File
# Begin Source File

SOURCE=.\wpcap.def
# PROP Exclude_From_Build 1
# End Source File
# End Target
# End Project
