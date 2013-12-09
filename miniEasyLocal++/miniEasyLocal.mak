# Microsoft Developer Studio Generated NMAKE File, Based on miniEasyLocal.dsp
!IF "$(CFG)" == ""
CFG=miniEasyLocal - Win32 Debug
!MESSAGE No configuration specified. Defaulting to miniEasyLocal - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "miniEasyLocal - Win32 Debug" && "$(CFG)" != "miniEasyLocal - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "miniEasyLocal.mak" CFG="miniEasyLocal - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "miniEasyLocal - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "miniEasyLocal - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "miniEasyLocal - Win32 Debug"

OUTDIR=.\lib\debug
INTDIR=.\lib\debug

ALL : ".\lib\libEasyLocal_debug.lib"


CLEAN :
	-@erase "$(INTDIR)\EasyLocal.obj"
	-@erase "$(INTDIR)\ExpSpec.tab.obj"
	-@erase "$(INTDIR)\ExpSpec.yy.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase ".\lib\libEasyLocal_debug.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I ".\etc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\miniEasyLocal.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\miniEasyLocal.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lib\libEasyLocal_debug.lib" 
LIB32_OBJS= \
	"$(INTDIR)\EasyLocal.obj" \
	"$(INTDIR)\ExpSpec.tab.obj" \
	"$(INTDIR)\ExpSpec.yy.obj"

".\lib\libEasyLocal_debug.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "miniEasyLocal - Win32 Release"

OUTDIR=.\lib\release
INTDIR=.\lib\release

ALL : ".\lib\libEasyLocal.lib"

CLEAN :
	-@erase "$(INTDIR)\EasyLocal.obj"
	-@erase "$(INTDIR)\ExpSpec.tab.obj"
	-@erase "$(INTDIR)\ExpSpec.yy.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\lib\libEasyLocal.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /GX /O2 /I ".\etc" /D "WIN32" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\miniEasyLocal.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\miniEasyLocal.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lib\libEasyLocal.lib" 
LIB32_OBJS= \
	"$(INTDIR)\EasyLocal.obj" \
	"$(INTDIR)\ExpSpec.tab.obj" \
	"$(INTDIR)\ExpSpec.yy.obj"

".\lib\libEasyLocal.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("miniEasyLocal.dep")
!INCLUDE "miniEasyLocal.dep"
!ELSE 
!MESSAGE Warning: cannot find "miniEasyLocal.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "miniEasyLocal - Win32 Debug" || "$(CFG)" == "miniEasyLocal - Win32 Release"
SOURCE=.\src\EasyLocal.cpp

"$(INTDIR)\EasyLocal.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\ExpSpec.tab.cpp

"$(INTDIR)\ExpSpec.tab.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\ExpSpec.yy.cpp

"$(INTDIR)\ExpSpec.yy.obj" : $(SOURCE) "$(INTDIR)" ".\src\ExpSpec.tab.h"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\ExpSpec.lex

!IF  "$(CFG)" == "miniEasyLocal - Win32 Debug"

InputPath=.\src\ExpSpec.lex
InputName=ExpSpec

".\src\ExpSpec.yy.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	cd src 
	..\bin\flex -o$(InputName).yy.cpp $(InputName).lex 
<< 
	

!ELSEIF  "$(CFG)" == "miniEasyLocal - Win32 Release"

InputPath=.\src\ExpSpec.lex
InputName=ExpSpec

".\src\ExpSpec.yy.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	cd src 
	..\bin\flex -o$(InputName).yy.cpp $(InputName).lex 
<< 
	

!ENDIF 

SOURCE=.\src\ExpSpec.y

!IF  "$(CFG)" == "miniEasyLocal - Win32 Debug"

InputPath=.\src\ExpSpec.y
InputName=ExpSpec

".\src\ExpSpec.tab.h"	".\src\ExpSpec.tab.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	cd src 
	..\bin\bison -s..\etc -d $(InputName).y 
	move $(InputName).tab.c $(InputName).tab.cpp
<< 
	

!ELSEIF  "$(CFG)" == "miniEasyLocal - Win32 Release"

InputPath=.\src\ExpSpec.y
InputName=ExpSpec

".\src\ExpSpec.tab.h"	".\src\ExpSpec.tab.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	cd src 
	..\bin\bison -s..\etc -d $(InputName).y 
	move $(InputName).tab.c $(InputName).tab.cpp
<< 
	

!ENDIF 


!ENDIF 

