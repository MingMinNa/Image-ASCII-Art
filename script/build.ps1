# build.ps1

$CC = "gcc"
$CFLAGS = @("-Wall", "-O2", "-std=c17")

$SRC_DIR = "src"
$BUILD_DIR = "build"

$SRCS = Get-ChildItem "$SRC_DIR\*.c"
$TARGET = "main.exe"

foreach ($src in $SRCS) {
    $objName = $src.BaseName + ".o"
    $objPath = Join-Path $BUILD_DIR $objName

    if (-not (Test-Path $BUILD_DIR)) {
        New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
    }

    Write-Host "Compiling $($src.Name) → $objName"
    & $CC $CFLAGS -c $src.FullName -o $objPath
}

$OBJS = Get-ChildItem "$BUILD_DIR\*.o"

Write-Host "Linking → $TARGET"
& $CC $CFLAGS $OBJS.FullName -o $TARGET -lm

Write-Host "Done!"