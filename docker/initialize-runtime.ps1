[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$area = Join-Path $root 'area'
$data = Join-Path $root 'data'
$encoding = [System.Text.UTF8Encoding]::new($false)

foreach ($dir in @('area', 'data', 'player', 'player/bk', 'gods', 'log')) {
    [System.IO.Directory]::CreateDirectory((Join-Path $root $dir)) | Out-Null
}

function Write-RuntimeFile([string]$name, [string]$content, [string]$replaceContent = '') {
    $path = Join-Path $data $name
    if (Test-Path $path) {
        $existing = [System.IO.File]::ReadAllText($path).Replace("`r`n", "`n").Trim()
        if ($existing.Length -gt 0 -and (!$replaceContent -or $existing -ne $replaceContent.Replace("`r`n", "`n").Trim())) {
            [System.IO.File]::WriteAllText($path, $existing + "`n", $encoding)
            Write-Host "Verified data/$name (LF normalized)"
            return
        }
        Copy-Item $path "$path.pre-init" -ErrorAction Stop
    }
    [System.IO.File]::WriteAllText($path, $content.Replace("`r`n", "`n").TrimEnd() + "`n", $encoding)
    Write-Host "Initialized data/$name"
}

Write-RuntimeFile 'area.lst' "limbo.are`ncodespecii.are`n`$`n"
Write-RuntimeFile 'guild.dat' "End`n"
Write-RuntimeFile 'quest.dat' "`$`n"
Write-RuntimeFile 'config.cfg' @'
Slaughter 0
Doubledam 0
Doubleexp 0
Nosun 0
Spookums 0
Arena 0
Resolver 0
Wizlock 0
Newlock 0
Gxp 0
Gqp 0
Xpmult 1
Manualxp 0
Maxon 0
Carnival 0
End
'@

foreach ($areaFile in Get-ChildItem -Path $area -Filter '*.are') {
    $text = [System.IO.File]::ReadAllText($areaFile.FullName).Replace("`r`n", "`n")
    [System.IO.File]::WriteAllText($areaFile.FullName, $text, $encoding)
}

Write-Host "Runtime directories and LF line endings verified for Docker development."
