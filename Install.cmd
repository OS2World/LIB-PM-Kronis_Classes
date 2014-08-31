@echo off
echo.
echo Installing Kroni's Classes; this will take a few minutes...
echo.
cd source
echo.
echo Creating standard library...
echo.
nmake /NOLOGO scratch=1 KrClass.mak
del tempinc\*.cpp
rd tempinc
del *.obj
echo.
echo Creating debug library...
echo.
nmake /NOLOGO scratch=1 debug=1 KrClass.mak
del tempinc\*.cpp
rd tempinc
del *.obj
echo.
echo Creating static library...
echo.
nmake /NOLOGO scratch=1 static=1 KrClass.mak
del tempinc\*.cpp
rd tempinc
del *.obj
echo.
echo Creating static debug library...
echo.
nmake /NOLOGO scratch=1 debug=1 static=1 KrClass.mak
del tempinc\*.cpp
rd tempinc
del *.obj
cd ..\lib
del *.bak
cd ..\demo
echo.
echo Creating demo programs...
echo.
nmake /NOLOGO demo.mak
del tempinc\*.cpp
rd tempinc
del *.obj
cd ..
echo.
echo Please don't forget to change your BOOKSHELF according to readme.1st.
echo.
echo Installation finished! Please read doc/legal and readme.1st *now*.
echo.
pause
