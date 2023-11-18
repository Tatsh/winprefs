if (${env:NUGET_API_KEY}.Length -eq 0) {
  Write-Output 'Set $env:NUGET_API_KEY'
  return 1
}
$loc = $(Get-Location) -Replace "\\", "/"
Remove-Item -Force -Recurse out
New-Item -ItemType Directory out
New-Item -ItemType Directory out\build
Set-Location out\build
cmake -G 'Visual Studio 17 2022' -A x64 "-DCMAKE_INSTALL_PREFIX=$loc/out/install" -DCMAKE_BUILD_TYPE=Release ../..
cmake --build . --config Release -v
cmake --install . --config Release -v
Set-Location ..\install
Remove-Item -Path WinPrefs\*.deps.json
Remove-Item -Path WinPrefs\*.lib
Remove-Item -Path WinPrefs\*.pdb
Remove-Item -Recurse -Path WinPrefs\ref
Remove-Item -Recurse -Path WinPrefs\runtimes
Write-Output Publish-Module -Name .\WinPrefs -NuGetApiKey ${env:NUGET_API_KEY}
Set-Location ..\..
