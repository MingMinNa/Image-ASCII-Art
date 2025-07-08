# clean.ps1

$BUILD_DIR = "build"
$TARGET = @("main.exe", "main")

Remove-Item -Recurse -Force $BUILD_DIR -ErrorAction SilentlyContinue
Remove-Item -Force $TARGET -ErrorAction SilentlyContinue

Write-Host "Clean done."