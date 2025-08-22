@echo off
REM update-token.bat
REM Usage: update-token.bat <TOKEN>

IF "%~1"=="" (
    echo Usage: %0 TOKEN
    exit /b 1
)

SET TOKEN=%~1
SET USERNAME=joseramiro
SET REPO_URL=flash_sst26v.git

REM Build the new URL
SET NEW_URL=https://%USERNAME%:%TOKEN%@github.com/%USERNAME%/%REPO_URL%

echo.
echo This is the updated Git URL with your token:
echo %NEW_URL%
echo.

REM Update the Git remote
git remote set-url origin %NEW_URL%

IF %ERRORLEVEL% EQU 0 (
    echo OK Git remote 'origin' successfully updated.
) ELSE (
    echo ERROR Failed to update Git remote. Make sure you are in a Git repository.
)