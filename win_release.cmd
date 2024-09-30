@echo off
setlocal EnableDelayedExpansion

rem ----------------------------------------------------------
rem   KLAYOUT build script
rem   See build.bat -h for details
rem

rem ----------------------------------------------------------
rem parse command lines

rem --- 不修改
set CONFIG=release
rem --- 不修改 结束


rem ---配置区域 开始
set PYTHONVERSION=311
set PYTHONINCLUDE=D:\ProgramData\Anaconda3\envs\myenv\include
set PYTHONLIBFILE=D:\ProgramData\Anaconda3\envs\myenv\libs\python311.lib
set QUQUAN_LAYOUT_INC=D:\project\ququan_layout\src
set QUQUAN_LAYOUT_LIB_DIR=D:\project\ququan_layout\src\build\msvc2017_release
rem ---配置区域 结束

set "inst_path=%~dp0"
set arch=x64
set compiler=msvc2017
set option-qmake=qmake
set "option-build=%inst_path%\build\%compiler%_%CONFIG%"

set "KLAYOUT_VERSION_DATE=%date:~0,10%"
set KLAYOUT_VERSION_REV=LatestSourcePackage

rem ----------------------------------------------------------
rem locate MSVC on the system

set MSVC_COMPILER_INST=notfound
rem VS 2017 sets exactly one install as the "main" install, so we may find MSBuild in there.
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\SxS\VS7" /v 15.0 /reg:32 >nul 2>nul
if NOT ERRORLEVEL 1 (
  for /F "tokens=1,2*" %%i in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\SxS\VS7" /v 15.0 /reg:32') do (
    if "%%i"=="15.0" (
      if exist "%%k\VC\Auxiliary\Build" (
        set "MSVC_COMPILER_INST=%%k\VC\Auxiliary\Build"
        set "msg=Found MSVC installation at !MSVC_COMPILER_INST!"
        echo !msg!
      )
    )
  )
)

rem alternative way (VS2019)
if "%MSVC_COMPILER_INST%" == "notfound" (
  set vs_path=notfound
  for /f "delims=" %%i in ('"c:\program files (x86)\microsoft visual studio\installer\vswhere" -latest -prerelease -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath -products *') do (
    set "vs_path=%%i"
  )
  if not "vs_path" == "notfound" (
    set "MSVC_COMPILER_INST=!vs_path!\vc\Auxiliary\Build"
  )
)
if "%MSVC_COMPILER_INST%" == "notfound" (
  echo ERROR: Unable to find MSVC installation
  goto :eof
)
if %arch% equ x64 (
  call "%MSVC_COMPILER_INST%\vcvars64"
) else (
  call "%MSVC_COMPILER_INST%\vcvars32"
)



mkdir "%option-build%" 2>nul
cd "%option-build%"
if not exist "%option-build%" (
  echo ERROR: build directory does not exists and cannot be created
  goto :eof
)


rem ----------------------------------------------------------
rem check the qmake binary

"%option-qmake%" -v >nul 2>nul
if ERRORLEVEL 1 (
  echo ERROR: Not a valid qmake: %option-qmake%
  echo Use -qmake option to specify the path to the qmake binary.
  goto :eof
)
echo Using qmake from %option-qmake%

rem ----------------------------------------------------------
rem Run qmake
echo on
"%option-qmake%" ^
  -recursive ^
  -spec win32-msvc ^
  "CONFIG+=%CONFIG%" ^
  "PYTHONINCLUDE=%PYTHONINCLUDE%" ^
  "PYTHONLIBFILE=%PYTHONLIBFILE%" ^
  "QUQUAN_LAYOUT_INC=%QUQUAN_LAYOUT_INC%" ^
  "QUQUAN_LAYOUT_LIB_DIR=%QUQUAN_LAYOUT_LIB_DIR%" ^
  "KLAYOUT_VERSION_DATE=%KLAYOUT_VERSION_DATE%" ^
  "KLAYOUT_VERSION_REV=%KLAYOUT_VERSION_REV%" ^
  %inst_path%\ququan_shapes.pro || exit /b 1 
  
  
rem start the build
set CL=/MP
nmake "" || exit /b 2


rem copy depending mingw dll
set "softwareName=mingw64"
set softPath=
set softDir=

set "regPath=HKEY_CURRENT_USER\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Compatibility Assistant\Store"


for /f %%i in ('reg query "HKEY_CURRENT_USER\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Compatibility Assistant\Store" /f "mingw64" /s') do (
    set softPath=%%i
    rem echo !softPath!
	echo !softPath!	| findstr /C:"mingw64" >nul
	
	if errorlevel 1 (
		rem string not found
	) else (
		 set softPath=!softPath:%softwareName%.exe=!
		 
		 set softDir=!softPath!
	)
)

copy /Y "%softDir%mingw64\bin\libgcc_s_seh-1.dll" "%option-build%\"
copy /Y "%softDir%mingw64\bin\libstdc++-6.dll" "%option-build%\"
copy /Y "%softDir%mingw64\bin\libwinpthread-1.dll" "%option-build%\"

rem copy depending python dll
set python_dir=%PYTHONINCLUDE%
set python_dir=%python_dir:include=%
copy /Y "%python_dir%python%PYTHONVERSION%.dll" "%option-build%\"

rem copy depending layout dll
copy /Y "%QUQUAN_LAYOUT_LIB_DIR%\calc.dll" "%option-build%\"
copy /Y "%QUQUAN_LAYOUT_LIB_DIR%\klayout_tl.dll" "%option-build%\"
copy /Y "%QUQUAN_LAYOUT_LIB_DIR%\klayout_db.dll" "%option-build%\"
copy /Y "%QUQUAN_LAYOUT_LIB_DIR%\klayout_gsi.dll" "%option-build%\"

rem copy depending QT dll
rem todo

