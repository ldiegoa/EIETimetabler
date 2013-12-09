CFG_debug="miniEasyLocal - Win32 Debug"
CFG_release="miniEasyLocal - Win32 Release"
LIB=lib\\libEasyLocal
AUXMAKEFILE=miniEasyLocal.mak

all : $(LIB)_debug.lib $(LIB).lib

$(LIB)_debug.lib : 
	$(MAKE) /f $(AUXMAKEFILE) /E CFG=$(CFG_debug) 

$(LIB).lib :
	$(MAKE) /f $(AUXMAKEFILE) /E CFG=$(CFG_release)

clean :
	$(MAKE) /f $(AUXMAKEFILE) /E CFG=$(CFG_debug) clean
	$(MAKE) /f $(AUXMAKEFILE) /E CFG=$(CFG_release) clean