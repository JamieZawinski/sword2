# Microsoft Developer Studio Project File - Name="sword2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=sword2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sword2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sword2.mak" CFG="sword2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sword2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "sword2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sword2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\temp\release\"
# PROP Intermediate_Dir "..\temp\release\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ddraw.lib dsound.lib dxguid.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib shell32.lib vfw32.lib /nologo /subsystem:windows /machine:I386 /out:"..\release\sword2.exe"

!ELSEIF  "$(CFG)" == "sword2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\temp\debug\"
# PROP Intermediate_Dir "..\temp\debug\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ddraw.lib dsound.lib dxguid.lib winmm.lib vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\release\sword2_debug.exe" /pdbtype:sept
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "sword2 - Win32 Release"
# Name "sword2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Cursor.cpp
# End Source File
# Begin Source File

SOURCE=.\events.cpp
# End Source File
# Begin Source File

SOURCE=.\FindPath.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\map.cpp
# End Source File
# Begin Source File

SOURCE=.\message.cpp
# End Source File
# Begin Source File

SOURCE=.\Node.cpp
# End Source File
# Begin Source File

SOURCE=.\npc.cpp
# End Source File
# Begin Source File

SOURCE=.\Queue.cpp
# End Source File
# Begin Source File

SOURCE=.\role.cpp
# End Source File
# Begin Source File

SOURCE=.\script.cpp
# End Source File
# Begin Source File

SOURCE=.\Script1.rc
# End Source File
# Begin Source File

SOURCE=.\Stack.cpp
# End Source File
# Begin Source File

SOURCE=.\talk.cpp
# End Source File
# Begin Source File

SOURCE=.\Tree.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Cursor.h
# End Source File
# Begin Source File

SOURCE=.\events.h
# End Source File
# Begin Source File

SOURCE=.\findpath.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\map.h
# End Source File
# Begin Source File

SOURCE=.\message.h
# End Source File
# Begin Source File

SOURCE=.\Node.h
# End Source File
# Begin Source File

SOURCE=.\npc.h
# End Source File
# Begin Source File

SOURCE=.\Queue.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\role.h
# End Source File
# Begin Source File

SOURCE=.\script.h
# End Source File
# Begin Source File

SOURCE=.\Stack.h
# End Source File
# Begin Source File

SOURCE=.\talk.h
# End Source File
# Begin Source File

SOURCE=.\Tree.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\sword.ico
# End Source File
# End Group
# Begin Group "GameLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gamelib\Ddutil.cpp
# End Source File
# Begin Source File

SOURCE=.\gamelib\ddutil.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Goldpoint.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Goldpoint2.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_alpha.cpp
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_alpha.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_ani.cpp
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_ani.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_avi.cpp
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_avi.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_draw.cpp
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_draw.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_iniset.cpp
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_iniset.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_init.cpp
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_init.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_input.cpp
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_input.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_music.cpp
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_music.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_other.cpp
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_other.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_sound.cpp
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_sound.h
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_text.cpp
# End Source File
# Begin Source File

SOURCE=.\gamelib\Gp_text.h
# End Source File
# End Group
# Begin Group "Interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\interface\button.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\button.h
# End Source File
# Begin Source File

SOURCE=.\interface\check.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\check.h
# End Source File
# Begin Source File

SOURCE=.\interface\interface.h
# End Source File
# Begin Source File

SOURCE=.\interface\interface2.h
# End Source File
# Begin Source File

SOURCE=.\interface\listwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\listwindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\msgwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\msgwindow.h
# End Source File
# Begin Source File

SOURCE=.\interface\proess.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\proess.h
# End Source File
# Begin Source File

SOURCE=.\interface\scroll.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\scroll.h
# End Source File
# Begin Source File

SOURCE=.\interface\window.cpp
# End Source File
# Begin Source File

SOURCE=.\interface\window.h
# End Source File
# End Group
# Begin Group "Menu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\menu\arm.cpp
# End Source File
# Begin Source File

SOURCE=.\menu\arm.h
# End Source File
# Begin Source File

SOURCE=.\menu\goods.cpp
# End Source File
# Begin Source File

SOURCE=.\menu\goods.h
# End Source File
# Begin Source File

SOURCE=.\menu\help.cpp
# End Source File
# Begin Source File

SOURCE=.\menu\help.h
# End Source File
# Begin Source File

SOURCE=.\menu\loadsave.cpp
# End Source File
# Begin Source File

SOURCE=.\menu\loadsave.h
# End Source File
# Begin Source File

SOURCE=.\Menu\Magic.cpp
# End Source File
# Begin Source File

SOURCE=.\Menu\Magic.h
# End Source File
# Begin Source File

SOURCE=.\menu\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\menu\menu.h
# End Source File
# Begin Source File

SOURCE=.\menu\menu_inc.h
# End Source File
# Begin Source File

SOURCE=.\menu\menu_inc2.h
# End Source File
# Begin Source File

SOURCE=.\menu\menutop.cpp
# End Source File
# Begin Source File

SOURCE=.\menu\menutop.h
# End Source File
# Begin Source File

SOURCE=.\menu\rolesel.cpp
# End Source File
# Begin Source File

SOURCE=.\menu\rolesel.h
# End Source File
# Begin Source File

SOURCE=.\menu\state.cpp
# End Source File
# Begin Source File

SOURCE=.\menu\state.h
# End Source File
# Begin Source File

SOURCE=.\menu\system.cpp
# End Source File
# Begin Source File

SOURCE=.\menu\system.h
# End Source File
# Begin Source File

SOURCE=.\menu\title.cpp
# End Source File
# Begin Source File

SOURCE=.\menu\title.h
# End Source File
# End Group
# Begin Group "Fight"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fight\Fight.cpp

!IF  "$(CFG)" == "sword2 - Win32 Release"

# PROP Intermediate_Dir "..\temp\release\fight\"

!ELSEIF  "$(CFG)" == "sword2 - Win32 Debug"

# PROP Intermediate_Dir "..\temp\debug\Fight"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fight\Fight.h

!IF  "$(CFG)" == "sword2 - Win32 Release"

# PROP Intermediate_Dir "..\temp\release\fight\"

!ELSEIF  "$(CFG)" == "sword2 - Win32 Debug"

# PROP Intermediate_Dir "..\temp\debug\Fight"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fight\fightgoods.cpp

!IF  "$(CFG)" == "sword2 - Win32 Release"

# PROP Intermediate_Dir "..\temp\release\fight\"

!ELSEIF  "$(CFG)" == "sword2 - Win32 Debug"

# PROP Intermediate_Dir "..\temp\debug\Fight"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fight\fightgoods.h

!IF  "$(CFG)" == "sword2 - Win32 Release"

# PROP Intermediate_Dir "..\temp\release\fight\"

!ELSEIF  "$(CFG)" == "sword2 - Win32 Debug"

# PROP Intermediate_Dir "..\temp\debug\Fight"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fight\FightInit.cpp

!IF  "$(CFG)" == "sword2 - Win32 Release"

# PROP Intermediate_Dir "..\temp\release\fight\"

!ELSEIF  "$(CFG)" == "sword2 - Win32 Debug"

# PROP Intermediate_Dir "..\temp\debug\Fight"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Fight\FightMagic.cpp
# End Source File
# Begin Source File

SOURCE=.\Fight\FightMagic.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\README.TXT
# End Source File
# Begin Source File

SOURCE="..\打造你自己的RPG.doc"
# End Source File
# Begin Source File

SOURCE="..\圣II历程 －打造双刃剑.doc"
# End Source File
# Begin Source File

SOURCE="..\圣二源程序导读.doc"
# End Source File
# End Target
# End Project
