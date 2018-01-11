@echo off

REM This script copies files from the repository and installs to the local
REM machine. System and user files are installed.

REM To avoid overwriting existing files already stored on the system, either
REM delete or change file extension on specific installation files.

REM copies executable
MKDIR C:\OTLS\SpanAnalyzer
XCOPY build\msvc\Release\*.exe C:\OTLS\SpanAnalyzer /y

REM copies resource files
MKDIR C:\OTLS\SpanAnalyzer\res
XCOPY res\help\*.htb C:\OTLS\SpanAnalyzer\res /y

REM copies example file(s)
MKDIR C:\OTLS\SpanAnalyzer\Examples
XCOPY test\cables\*.cable C:\OTLS\SpanAnalyzer\Examples /y 
XCOPY test\*.spananalyzer C:\OTLS\SpanAnalyzer\Examples /y

REM copies user file(s)
MKDIR %APPDATA%\OTLS\SpanAnalyzer
XCOPY *.xml %APPDATA%\OTLS\SpanAnalyzer /y

REM creates a shortcut
mklink %USERPROFILE%\Desktop\SpanAnalyzer.lnk C:\OTLS\SpanAnalyzer\SpanAnalyzer.exe
