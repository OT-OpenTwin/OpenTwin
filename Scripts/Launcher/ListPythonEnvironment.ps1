
$temp = Join-Path -Path $PWD.Path -ChildPath "Python"
$env:PYTHONHOME = $temp

Start-Process -FilePath .\Python.exe -ArgumentList "-i -m pip freeze"
 
pause