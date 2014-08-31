# KrClass.mak
# (c) 1997 Wolfgang Kronberg
#
# This will build the library "Kroni's Classes"
#
# possible options:
#
#  scratch: use this if no .lib file exists
#  debug: stores debugging information in the code
#  static: do not use VAC++ DLLs for the code
#

library=$(lib1part)$(lib2part)

libpath = ..\lib
incpath = ..\include

!IFDEF scratch
linktype = +
!ELSE
linktype = -+
!ENDIF

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

cflags=/I..\include $(c1flags) $(c2flags) /Q /Gm /G5 /C /W2
# quiet, multithread, optimize for Pentium, compile only, warning level = 2

$(libpath)\$(library).lib : krprint.obj krgobj.obj krcto.obj krcstran.obj \
                            krbwin.obj krasyncm.obj krwc.obj iavlkss.obj \
                            krtrace.obj krprof.obj krmenu.obj
  !ilib /NOLOGO $(libpath)\$(library).lib $(linktype)$?;

krtrace.obj krprof.obj krmenu.obj krcto.obj : \
    $$(@B).cpp $(incpath)\$$(@B).hpp
  $(CC) $(cflags) $(@F:.obj=.cpp) 

krprint.obj krcstran.obj krasyncm.obj : \
    $$(@B).cpp $(incpath)\$$(@B).hpp $(incpath)\krtrace.hpp
  $(CC) $(cflags) $(@F:.obj=.cpp)

krgobj.obj : \
    krgobj.cpp $(incpath)\$$(@B).hpp $(incpath)\krcstran.hpp \
    $(incpath)\krtrace.hpp
  $(CC) $(cflags) $(@F:.obj=.cpp)

krbwin.obj : \
    krbwin.cpp $(incpath)\$$(@B).hpp $(incpath)\krcstran.hpp
  $(CC) $(cflags) $(@F:.obj=.cpp)

!IF [dir tempinc\iavlkss.cpp >nul 2>nul] == 0

krwc.obj tempinc\iavlkss.cpp : \
    krwc.cpp $(incpath)\krwc.hpp
  del tempinc\iavlkss.cpp >nul
  $(CC) $(cflags) krwc.cpp

!ELSE

krwc.obj tempinc\iavlkss.cpp : \
    krwc.cpp $(incpath)\krwc.hpp
  $(CC) $(cflags) krwc.cpp

!ENDIF

iavlkss.obj : tempinc\iavlkss.cpp
  $(CC) $(cflags) tempinc\iavlkss.cpp
