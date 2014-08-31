# demo.mak
# (c) 1997 Wolfgang Kronberg
#
# This will build the demo programs for Kroni's Classes
#
# possible options:
#
#  debug: stores debugging information in the code
#  static: do not use VAC++ DLLs for the code
#

programs = demo.exe hello.exe ini.exe dialog.exe dialog2.exe dialog3.exe

krlibrary = ..\lib\$(library).lib
krinclude = ..\include

library=$(lib1part)$(lib2part)

!IFDEF debug
c1flags = /Ti+ /Gh
# debug = on, profileinfo = on
lib2part = d
!ELSE
c1flags = /Ti- /O /Oc
# debug = off, optimize = on, optimize for size also = on
lib2part =
!ENDIF

!IFDEF static
c2flags = /Gd-
# link statically
lib1part = skrclas
!ELSE
c2flags = /Gd+
# link with VAC++ DLLs
lib1part = krclas
!ENDIF

cflags=/I$(krinclude) $(c1flags) $(c2flags) /Q /Gm /G5 /C /W2
# quiet, multithread, optimize for Pentium, compile only, warning level = 2

linkflags=/Q /B" /exepack:2 /pmtype:pm /packd /nologo"
# quiet, pass options to linker: high compression, PM program, pack data, quiet

.obj.exe:
  icc.exe $(linkflags) $< $(krlibrary)
# inference rule for .obj -> .exe: just link the input file with the library

.cpp.obj:
  $(CC) $(cflags) $<
# inference rule for .cpp -> .obj: just compile the input file

all: $(programs)
# 'nmake all' makes all programs

$(programs) : $$(@R).obj $(krlibrary)
# each .exe file depends on its .obj file and the library

$(programs:.exe=.obj) : $$(@R).cpp $(krinclude)\*.hpp
# each .obj file depends on its .cpp file and the library's header files

