@echo off
setlocal

set "SAVED_DIR=%CD%"
set "RSVARS_BAT="
set "TARGET_CONFIG=%~1"
set "TARGET_PLATFORM=%~2"

if not defined TARGET_CONFIG set "TARGET_CONFIG=Release"
if not defined TARGET_PLATFORM set "TARGET_PLATFORM=Win64"

if defined BDS if exist "%BDS%\bin\rsvars.bat" set "RSVARS_BAT=%BDS%\bin\rsvars.bat"

if not defined RSVARS_BAT (
  for /f "delims=" %%D in ('dir "%ProgramFiles(x86)%\Embarcadero\Studio" /ad /b /o-n 2^>nul') do (
    if exist "%ProgramFiles(x86)%\Embarcadero\Studio\%%D\bin\rsvars.bat" (
      set "RSVARS_BAT=%ProgramFiles(x86)%\Embarcadero\Studio\%%D\bin\rsvars.bat"
      goto :rsvars_found
    )
  )
)

:rsvars_found
if not defined RSVARS_BAT (
  echo rsvars.bat was not found. Set BDS or install RAD Studio.
  cd /d "%SAVED_DIR%"
  exit /b 1
)

call "%RSVARS_BAT%"
if errorlevel 1 (
  echo Failed to initialize RAD Studio environment.
  cd /d "%SAVED_DIR%"
  exit /b 1
)

cd /d "%~dp0src"
"C:\Windows\Microsoft.NET\Framework64\v4.0.30319\MSBuild.exe" feditor.cbproj /p:Config=%TARGET_CONFIG% /p:Platform=%TARGET_PLATFORM% /t:Build
set "BUILD_EXIT_CODE=%ERRORLEVEL%"
cd /d "%SAVED_DIR%"
exit /b %BUILD_EXIT_CODE%
