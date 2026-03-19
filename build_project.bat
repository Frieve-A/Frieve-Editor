@echo off
setlocal

set "SAVED_DIR=%CD%"
set "RSVARS_BAT="
set "TARGET_CONFIG=%~1"
set "TARGET_PLATFORM=%~2"

if not defined TARGET_CONFIG set "TARGET_CONFIG=Release"
if not defined TARGET_PLATFORM set "TARGET_PLATFORM=Win64"
set "SCRIPT_DIR=%~dp0"
set "BUILD_OUTPUT_DIR=%SCRIPT_DIR%build\%TARGET_PLATFORM%\%TARGET_CONFIG%"
set "RESOURCE_DIR=%SCRIPT_DIR%resource"
set "DIST_DIR=%SCRIPT_DIR%dist"
set "PACKAGE_DIR=%DIST_DIR%\feditor"
set "ZIP_NAME=feditor_%TARGET_PLATFORM%_%TARGET_CONFIG%.zip"
set "ZIP_PATH=%DIST_DIR%\%ZIP_NAME%"

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
if not "%BUILD_EXIT_CODE%"=="0" goto :end

if not exist "%BUILD_OUTPUT_DIR%\feditor.exe" (
  echo Build output not found: "%BUILD_OUTPUT_DIR%\feditor.exe"
  set "BUILD_EXIT_CODE=1"
  goto :end
)

if not exist "%RESOURCE_DIR%" (
  echo Resource folder not found: "%RESOURCE_DIR%"
  set "BUILD_EXIT_CODE=1"
  goto :end
)

if not exist "%DIST_DIR%" mkdir "%DIST_DIR%"
if exist "%PACKAGE_DIR%" rmdir /s /q "%PACKAGE_DIR%"
mkdir "%PACKAGE_DIR%"

copy /y "%BUILD_OUTPUT_DIR%\feditor.exe" "%PACKAGE_DIR%\" >nul
copy /y "%RESOURCE_DIR%\help.fip" "%PACKAGE_DIR%\" >nul
copy /y "%RESOURCE_DIR%\setting2.ini" "%PACKAGE_DIR%\" >nul
xcopy "%RESOURCE_DIR%\help" "%PACKAGE_DIR%\help\" /e /i /y >nul
xcopy "%RESOURCE_DIR%\lng" "%PACKAGE_DIR%\lng\" /e /i /y >nul
xcopy "%RESOURCE_DIR%\wallpaper" "%PACKAGE_DIR%\wallpaper\" /e /i /y >nul

if exist "%ZIP_PATH%" del /q "%ZIP_PATH%"
powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%PACKAGE_DIR%' -DestinationPath '%ZIP_PATH%' -Force"
if errorlevel 1 (
  echo Failed to create zip: "%ZIP_PATH%"
  set "BUILD_EXIT_CODE=1"
  goto :end
)

echo Packaging completed:
echo   "%PACKAGE_DIR%"
echo   "%ZIP_PATH%"

:end
cd /d "%SAVED_DIR%"
exit /b %BUILD_EXIT_CODE%
