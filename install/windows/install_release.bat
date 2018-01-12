@echo off

REM This script copies files from a release and installs to the local machine.
REM System and user files are installed.

REM To avoid overwriting existing files already stored on the system, either
REM delete or change file extension on specific installation files.

REM creates otls directory
SET DIR_OTLS=C:\OTLS
MKDIR %DIR_OTLS%

REM copies executable
SET DIR_APP=%DIR_OTLS%\SpanAnalyzer
MKDIR %DIR_APP%
XCOPY *.exe %DIR_APP% /y

REM copies resource files
SET DIR_RESOURCES=%DIR_APP%\res
MKDIR %DIR_RESOURCES%
XCOPY res\*.htb %DIR_RESOURCES% /y

REM copies example file(s)
SET DIR_EXAMPLES=%DIR_APP%\Examples
MKDIR %DIR_EXAMPLES%
XCOPY Examples\*.cable %DIR_EXAMPLES% /y
XCOPY Examples\*.spananalyzer %DIR_EXAMPLES% /y

REM copies user file(s)
SET DIR_USER=%APPDATA%\OTLS\SpanAnalyzer
MKDIR %DIR_USER%
XCOPY *.xml %DIR_USER% /y

REM creates a shortcut
MKLINK %USERPROFILE%\Desktop\SpanAnalyzer.lnk %DIR_APP%\SpanAnalyzer.exe
