@echo off

rem
rem SigmaGraph Build System
rem (C) 1997-2020  Pr. Sidi HAMADY
rem www.hamady.org
rem

attrib +R %~n0%~x0

pushd "%~dp0"

set CURDIR=%CD%
set HHCompile="C:\Program Files (x86)\HTML Help Workshop\hhc" 

rem This batch was done for HTML Help workshop
rem to download from: https://www.microsoft.com/en-us/download/details.aspx?id=21138


rem delete compiled html help files
del /f /q %CURDIR%\sigmagraph.chm >nul 2>&1
del /f /q %CURDIR%\sigmaconsole.chm >nul 2>&1

rem build the html help files
%HHCompile% %CURDIR%\sigmagraph.hhp
rem hhc return 1 (TRUE) if it succeeds...
if not errorlevel 1 (
    goto :onerror
)
%HHCompile% %CURDIR%\sigmagraph.hhp

:onerror

popd

attrib -R %~n0%~x0
