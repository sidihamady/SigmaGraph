@echo off

rem
rem SigmaGraph Build System
rem (C) 1997-2020  Pr. Sidi HAMADY
rem www.hamady.org
rem

attrib +R %~n0%~x0

pushd "%~dp0"

set CURDIR=%CD%
set CDIR=%CURDIR%
set DISTDIR=%CDIR%\Dist
set OUTPUTDIR=%CDIR%\output

set whatarg=%1
if "%whatarg%" == "" set whatarg=all


rem This batch was done for Visual C++ 2013
rem to adapt to the actual VC version (2015, 2017, ...)
rem run this batch file after starting a Visual Studio Tools Command Prompt
rem     if running from a standard command prompt, you need to call vcvarsall.bat by uncommenting and adapting the following line to your installed VC:
rem     call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86


rem delete binaries
if "%whatarg%" == "all" (
    del /f /q %DISTDIR%\SigmaGraph\bin\SigmaGraph.exe >nul 2>&1
    del /f /q %DISTDIR%\SigmaGraph\bin\SigmaConsole.exe >nul 2>&1
)

rem build LibCalc (Mathematical and scientific routines and the Lua engine)
set calc=false
if "%whatarg%" == "all" set calc=true
if "%whatarg%" == "calc" set calc=true
if "%calc%" == "true" (
    MSBuild %CDIR%\LibCalc\LibCalc.sln /t:rebuild
)


rem build LibGraph (2D plotting core)
set graph=false
if "%whatarg%" == "all" set graph=true
if "%whatarg%" == "graph" set graph=true
if "%graph%" == "true" (
    MSBuild %CDIR%\LibGraph\LibGraph.sln /t:rebuild
)


rem build LibScintilla (Lua editor control)
set lexer=false
if "%whatarg%" == "all" set lexer=true
if "%whatarg%" == "lexer" set lexer=true
if "%lexer%" == "true" (
    MSBuild %CDIR%\LibScintilla\win32\SciLexer.sln /t:rebuild
)


rem build SigmaConsole (mathematical expression-based calculator)
set console=false
if "%whatarg%" == "all" set console=true
if "%whatarg%" == "console" set console=true
if "%console%" == "true" (
    MSBuild %CDIR%\SigmaConsole\SigmaConsole.sln /t:rebuild
)


rem build SigmaGraph (the SigmaGraph main module)
set sigma=false
if "%whatarg%" == "all" set sigma=true
if "%whatarg%" == "sigma" set sigma=true
if "%sigma%" == "true" (
    MSBuild %CDIR%\SigmaGraph\SigmaGraph.sln /t:rebuild
)


rem build the binary distribution
set release=false
if "%whatarg%" == "all" set release=true
if "%whatarg%" == "release" set release=true
if "%release%" == "true" (

    del /f /q %DISTDIR%\SigmaGraph\help\sigmagraph.pdf >nul 2>&1
    del /f /q %DISTDIR%\SigmaGraph\help\sigmaconsole.pdf >nul 2>&1
    del /f /q %DISTDIR%\SigmaGraph\help\sigmagraph.chm >nul 2>&1
    del /f /q %DISTDIR%\SigmaGraph\help\sigmaconsole.chm >nul 2>&1

    del /f /q %DISTDIR%\SigmaGraph\bin\SigmaGraph.exe >nul 2>&1
    del /f /q %DISTDIR%\SigmaGraph\bin\SigmaConsole.exe >nul 2>&1
    del /f /q %DISTDIR%\SigmaGraph\sigmagraph_windows.zip >nul 2>&1

    copy /y %CDIR%\Help\sigmagraph.pdf %DISTDIR%\SigmaGraph\help\
    copy /y %CDIR%\Help\sigmaconsole.pdf %DISTDIR%\SigmaGraph\help\
    copy /y %CDIR%\Help\sigmagraph.chm %DISTDIR%\SigmaGraph\help\
    copy /y %CDIR%\Help\sigmaconsole.chm %DISTDIR%\SigmaGraph\help\

    copy /y %OUTPUTDIR%\bin\SigmaGraph.exe %DISTDIR%\SigmaGraph\bin\
    copy /y %OUTPUTDIR%\bin\SigmaConsole.exe %DISTDIR%\SigmaGraph\bin\

    copy /y %DISTDIR%\SigmaGraph\help\sigmagraph.pdf %OUTPUTDIR%\help\
    copy /y %DISTDIR%\SigmaGraph\help\sigmagraph.chm %OUTPUTDIR%\help\
    copy /y %DISTDIR%\SigmaGraph\help\sigmaconsole.pdf %OUTPUTDIR%\help\
    copy /y %DISTDIR%\SigmaGraph\help\sigmaconsole.chm %OUTPUTDIR%\help\
)


rem clean (delete compiled object files)
set clean=false
if "%whatarg%" == "all" set clean=true
if "%whatarg%" == "release" set clean=true
if "%whatarg%" == "clean" set clean=true
if "%clean%" == "true" (
    @del /s /f /q %OUTPUTDIR%\obj\*.* >nul 2>&1
    for /d %%p in (%OUTPUTDIR%\obj\*.*) do rmdir "%%p" /s /q >nul 2>&1
)

popd

attrib -R %~n0%~x0
