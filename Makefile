INT_DIR = int/
OUTPUT_DIR = bin/
MF 		= mkdir -p
RF      = rd /s /q
CC = cl
LK = link
CFlags = /c /Fo$(INT_DIR) /std:c++17 /MD
Defines = /D /DWINDOWS_IGNORE_PACKING_MISMATCH
IncludeDirs = /Isrc /Iinclude
LibDirs = 
Libs = GDI32.lib Shell32.lib kernel32.lib User32.lib
LFlags = $(INT_DIR)*.obj /out:$(OUTPUT_DIR)$(TargetName).exe
ProgramArgs = 

#Configuration Only
ifeq ($(Configuration), Debug)
CFlags += /Od
DEFINES += /DDEBUG
else
Defines += /DNDEBUG

ifeq ($(Configuration), Release)
CFlags += /Ot /Oi
LFLAGS += /LTCG /INCREMENTAL:NO /NODEFAULTLIB /Gy
else
CFlags += /Ot /Oi /O2 /GL /Gw
LFLAGS += /LTCG /INCREMENTAL:NO /NODEFAULTLIB /OPT:REF /OPT:ICF /Gy
endif
endif
SRC_DIR = src/
CORE_DIR = $(SRC_DIR)Core/
Files+= $(SRC_DIR)Example1.cpp

IncludeDirs += $(HBUFFER_LIB_SRC)

default: build

clean:
	$(RF) bin-int
make_folders:
	$(MF) $(INT_DIR)
	$(MF) $(OUTPUT_DIR)
build: make_folders
	$(CC) $(Files) $(CFlags) $(Defines) $(IncludeDirs)
	$(LK) $(LFlags) $(LibDirs) $(Libs)
run:
ifneq ($(Configuration), Dist)
	gdb -ex run -ex quit -ex "set args $(ProgramArgs)" $(OUTPUT_DIR)$(TargetName).exe
else
	$(OUTPUT_DIR)$(TargetName).exe
endif

buildnrun: make_folders build run
rebuild: make_folders buildpch build
rebuildnrun:make_folders buildpch build run