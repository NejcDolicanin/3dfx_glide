@echo off
echo.
echo #
echo # 3dfx Interactive Inc.
echo # win32 Glide makefile - KoolSmoky
echo # [koolsmoky@3dfxzone.it]
echo #
echo # "use: glide3x.bat [ all | gendate | minihwc | dll ] [ clean ]"
echo #
set FX_GLIDE_HW=h5
set BUILDDRIVE=%HOMEDRIVE%
set BUILD_ROOT=%BUILDDRIVE%\work\3dfx
set BUILD_ROOT_SWLIBS=%BUILD_ROOT%\swlibs
set DXSDKROOT=%BUILDDRIVE%\DXSDK9.0
set DIRECTXSDK=%DXSDKROOT%
set FX_HW_PROJECTS=glide3
set CC=%BUILDDRIVE%\WINDDK\2600\bin\x86\cl.exe
set AS=%BUILDDRIVE%\WINDDK\2600\bin\x86\ml.exe
rem set PATH=%BUILDDRIVE%\WINDDK\2600\bin\x86;%PATH%
if "%1" == "" goto error
goto %1
REM ----------build routine from here -------------
:all
if "%2" == "clean" goto clean
:gendate
if "%2" == "clean" goto clean_gendate
cd %BUILD_ROOT%\%FX_GLIDE_HW%\incsrc
nmake -f %BUILD_ROOT%\%FX_GLIDE_HW%\incsrc\makefile
if "%1" == "gendate" goto end
:minihwc
if "%2" == "clean" goto clean_minihwc
cd %BUILD_ROOT_SWLIBS%\fxmisc
nmake -f %BUILD_ROOT_SWLIBS%\fxmisc\makefile
cd %BUILD_ROOT_SWLIBS%\newpci\pcilib
nmake -f %BUILD_ROOT_SWLIBS%\newpci\pcilib\makefile
cd %BUILD_ROOT%\%FX_GLIDE_HW%\minihwc
nmake -f %BUILD_ROOT%\%FX_GLIDE_HW%\minihwc\makefile
if "%1" == "minihwc" goto end
:dll
if "%2" == "clean" goto clean_dll
cd %BUILD_ROOT%\%FX_GLIDE_HW%\%FX_HW_PROJECTS%\src
nmake -f %BUILD_ROOT%\%FX_GLIDE_HW%\%FX_HW_PROJECTS%\src\makefile
if "%1" == "dll" goto end
goto end
REM ----------clean routine from here -------------
:clean
cd %BUILD_ROOT%\%FX_GLIDE_HW%\include
nmake -f %BUILD_ROOT%\%FX_GLIDE_HW%\include\makefile clean
cd %BUILD_ROOT%\%FX_GLIDE_HW%\lib
nmake -f %BUILD_ROOT%\%FX_GLIDE_HW%\lib\makefile clean
cd %BUILD_ROOT%\%FX_GLIDE_HW%\bin
nmake -f %BUILD_ROOT%\%FX_GLIDE_HW%\bin\makefile clean
cd %BUILD_ROOT_SWLIBS%\include
nmake -f %BUILD_ROOT_SWLIBS%\include\makefile clean
:clean_gendate
cd %BUILD_ROOT%\%FX_GLIDE_HW%\incsrc
nmake -f %BUILD_ROOT%\%FX_GLIDE_HW%\incsrc\makefile clean
if "%1" == "gendate" goto end
:clean_minihwc
cd %BUILD_ROOT_SWLIBS%\fxmisc
nmake -f %BUILD_ROOT_SWLIBS%\fxmisc\makefile clean
cd %BUILD_ROOT_SWLIBS%\newpci\pcilib
nmake -f %BUILD_ROOT_SWLIBS%\newpci\pcilib\makefile clean
cd %BUILD_ROOT%\%FX_GLIDE_HW%\minihwc
nmake -f %BUILD_ROOT%\%FX_GLIDE_HW%\minihwc\makefile clean
if "%1" == "minihwc" goto end
:clean_dll
cd %BUILD_ROOT%\%FX_GLIDE_HW%\%FX_HW_PROJECTS%\src
nmake -f %BUILD_ROOT%\%FX_GLIDE_HW%\%FX_HW_PROJECTS%\src\makefile clean
if "%1" == "dll" goto end
goto end
:error
echo # oops!
echo #
echo.
goto realend
:end
echo.
echo #
echo # 3dfx Interactive Inc.
echo # win32 Glide makefile - KoolSmoky
echo # [koolsmoky@3dfxzone.it]
echo #
echo # DONE!
echo #
echo.
goto realend
:realend
cd %BUILD_ROOT%
