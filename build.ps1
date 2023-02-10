if (-not (Test-Path "build")){
    .\gen.ps1
}
cmake --build build
