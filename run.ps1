if (-not (Test-Path "build")){
    .\build.ps1
}
$loc = Get-Location
.\build\Debug\behold.exe $loc\assets
