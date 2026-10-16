[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$area = Join-Path $root 'area'
$data = Join-Path $root 'data'
$encoding = [System.Text.UTF8Encoding]::new($false)
[System.IO.Directory]::CreateDirectory($data) | Out-Null

function Write-RuntimeFile([string]$name, [string]$content, [string]$replaceContent = '') {
    $path = Join-Path $data $name
    if (Test-Path $path) {
        $existing = [System.IO.File]::ReadAllText($path).Replace("`r`n", "`n").Trim()
        if (!$replaceContent -or $existing -ne $replaceContent.Replace("`r`n", "`n").Trim()) {
            Write-Host "Preserved data/$name"
            return
        }
        Copy-Item $path "$path.pre-init" -ErrorAction Stop
    }
    [System.IO.File]::WriteAllText($path, $content.Replace("`r`n", "`n").TrimEnd() + "`n", $encoding)
    Write-Host "Initialized data/$name"
}

$listed = @([System.IO.File]::ReadAllText((Join-Path $area 'area.lst')) -split '\s+' |
    Where-Object { $_ -and $_ -ne '$' })
$world = [System.Collections.Generic.List[string]]::new()
$helps = [System.Collections.Generic.List[string]]::new()
foreach ($name in $listed) {
    $path = Join-Path $area $name
    if (!(Test-Path $path)) { throw "area/area.lst references missing file: $name" }
    $text = [System.IO.File]::ReadAllText($path).Replace("`r`n", "`n")
    if ($text -match '(?m)^#(?:AREA|AREADATA)\s*$') { $world.Add($name) }
    foreach ($section in [regex]::Matches($text, '(?ms)^#HELPS\s*\n(.*?)^\s*-?\d+\s+\$~')) {
        $helps.Add($section.Groups[1].Value.Trim())
    }
    if ($text -match '(?m)^#HELPS\s*$' -and $text -notmatch '(?m)^\s*-?\d+\s+\$~') {
        throw "Unterminated help section: $name"
    }
}
if (!$world.Count -or !$helps.Count) { throw 'No world or help records found in area/area.lst.' }
Write-RuntimeFile 'area.lst' (($world -join "`n") + "`n`$`n") "olmsg.are`nlimbo.are`nproto.are`nrom.are"
Write-RuntimeFile 'help.dat' (($helps -join "`n`n") + "`n0 `$~`n") 'help.are'

$socials = [System.IO.File]::ReadAllText((Join-Path $area 'social.are')).Replace("`r`n", "`n")
$socialSection = [regex]::Match($socials, '(?ms)^#SOCIALS\s*\n(.*?)^#0\s*$')
if (!$socialSection.Success) { throw 'Missing or unterminated socials in area/social.are.' }
Write-RuntimeFile 'social.dat' ($socialSection.Groups[1].Value.Trim() + "`n#0`n")
Write-RuntimeFile 'guild.dat' "End`n"
Write-RuntimeFile 'quest.dat' "`$`n"
Write-RuntimeFile 'music.txt' ([System.IO.File]::ReadAllText((Join-Path $area 'music.txt')))
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

$publicCommands = @{}
foreach ($name in ('north east south west up down climb exits look glance map automap inventory equipment score ' +
    'affects help commands socials who whois where time weather areas worth skills spells practice train ' +
    'say tell reply whisper emote chat ooc channels gtell group follow split clantalk guild clans ' +
    'get drop give put wear wield hold remove eat drink buy sell list value compare examine ' +
    'open close lock unlock pick stand sit rest sleep wake recall save quit password description title ' +
    'prompt colour color ansi config autolist autoassist autoexit autogold autoloot autosac autosplit ' +
    'brief compact combine scroll deaf quiet afk notell nosummon nofollow ' +
    'note news changes idea typo bug report scan search consider kill flee rescue cast ' +
    'charsheet cs cs2 cs3 rotelist disclist gift fleshcraft').Split(' ', [System.StringSplitOptions]::RemoveEmptyEntries)) {
    $publicCommands[$name] = $true
}
$standingCommands = 'north east south west up down climb get drop give put wear wield hold remove eat drink buy sell practice train open close lock unlock pick recall'.Split(' ')
$header = [System.IO.File]::ReadAllText((Join-Path $root 'src/merc.h'))
$maxLevel = [regex]::Match($header, '(?m)^#define\s+MAX_LEVEL\s+(\d+)').Groups[1].Value
if (!$maxLevel) { throw 'Could not determine MAX_LEVEL.' }
$source = [System.IO.File]::ReadAllText((Join-Path $root 'src/cmdtable.c'))
$source = [regex]::Replace($source, '(?s)/\*.*?\*/|(?m)//[^\r\n]*', '')
$commands = [System.Collections.Generic.List[string]]::new()
$names = @{}
foreach ($entry in [regex]::Matches($source, '\{\s*"([^"]+)"\s*,\s*(do_\w+)\s*\}')) {
    $name = $entry.Groups[1].Value
    if ($names.ContainsKey($name)) { continue }
    $names[$name] = $true
    $level = $maxLevel
    $log = 1
    $show = 0
    $position = 0
    if ($publicCommands.ContainsKey($name)) {
        $level = 0
        $log = 0
        $show = 1
        $position = 6
        if ($standingCommands -contains $name) { $position = 9 }
        if ('help commands socials who whois time channels save quit password tell reply ooc afk'.Split(' ') -contains $name) { $position = 0 }
        if ($name -eq 'password') { $log = 2 }
        if ('kill flee rescue cast'.Split(' ') -contains $name) { $position = 8 }
    }
    $commands.Add("Name $name~`nDofun $name~`nPosition $position`nLevel $level`nLog $log`nShow $show`nCat 0`nEnd`n")
}
if (!$commands.Count) { throw 'No command handlers found.' }
Write-RuntimeFile 'command.dat' (($commands -join "`n") + "`n`$`n")
Write-Host "World files: $($world.Count); help sections: $($helps.Count); command handlers: $($commands.Count)"