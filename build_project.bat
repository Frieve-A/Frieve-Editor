@echo off
call "C:\Program Files (x86)\Embarcadero\Studio\22.0\bin\rsvars.bat"
cd /d "%~dp0src"
"C:\Windows\Microsoft.NET\Framework64\v4.0.30319\MSBuild.exe" feditor.cbproj /p:Config=Release /p:Platform=Win64 /t:Build
