param([string]$GameDirectory, [switch]$Restore)
$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'ShadowInstallConfig.psm1') -Force
$expectedDll = '571A6F69501BEED5D14BA5C971B65FB9E476806B67818EFDC811DABA12D5E925'
$expectedExe = '08759A5516F9837920EA504436236BBAB89D0826A8E4D04FF106345177B5345D'

function Assert-GameClosed {
    if (Get-Process GTAIV,EFLC -ErrorAction SilentlyContinue) {
        throw 'Close GTA IV before installing or restoring. The installer will not close it for you.'
    }
}
function Write-IniMerge([string]$Path, [hashtable]$Sections) {
    $reader = [System.IO.StreamReader]::new($Path, [System.Text.Encoding]::UTF8, $true)
    try { $text=$reader.ReadToEnd(); $encoding=$reader.CurrentEncoding } finally { $reader.Dispose() }
    $merged=Set-ShadowIniKeys -Text $text -Sections $Sections
    [System.IO.File]::WriteAllText($Path, $merged, $encoding)
}
try {
    if (!$GameDirectory) {
        Add-Type -AssemblyName System.Windows.Forms
        $dialog = [System.Windows.Forms.OpenFileDialog]::new()
        $dialog.Title = 'Select GTAIV.exe in the game where FusionFix is installed'
        $dialog.Filter = 'GTA IV executable (GTAIV.exe)|GTAIV.exe'
        if ($dialog.ShowDialog() -ne [System.Windows.Forms.DialogResult]::OK) { Write-Host 'Cancelled.'; exit 0 }
        $GameDirectory = [System.IO.Path]::GetDirectoryName($dialog.FileName)
    }
    $GameDirectory = (Resolve-Path -LiteralPath $GameDirectory).Path
    Assert-GameClosed
    $plugins=Join-Path $GameDirectory 'plugins'
    $backupRoot=Join-Path $GameDirectory 'ExtraNightShadows-backups'
    $names=@('GTAIV.EFLC.FusionFix.asi','GTAIV.EFLC.FusionFix.ini','GTAIV.EFLC.FusionFix.cfg')
    if ($Restore) {
        $last=Join-Path $backupRoot 'latest.txt'
        if (!(Test-Path -LiteralPath $last)) { throw 'No backup made by this installer was found.' }
        $stamp=(Get-Content -LiteralPath $last -Raw).Trim()
        if ($stamp -notmatch '^\d{8}-\d{6}-\d{3}$') { throw 'Invalid backup identifier.' }
        $backup=Join-Path $backupRoot $stamp
        $receipt=Get-Content -LiteralPath (Join-Path $backup 'receipt.json') -Raw | ConvertFrom-Json
        foreach ($item in $receipt.files) {
            if ($item.name -notin $names) { throw 'Unexpected backup file.' }
            if ((Get-FileHash -LiteralPath (Join-Path $backup $item.name)).Hash -ne $item.sha256) { throw 'Backup checksum failed.' }
        }
        Assert-GameClosed
        foreach ($item in $receipt.files) { Copy-Item -LiteralPath (Join-Path $backup $item.name) -Destination (Join-Path $plugins $item.name) -Force }
        Write-Host 'Previous FusionFix files restored.'
        exit 0
    }
    $exe=Join-Path $GameDirectory 'GTAIV.exe'
    if ((Get-FileHash -LiteralPath $exe).Hash -ne $expectedExe) { throw 'This release supports only the audited GTA IV Complete Edition 1.2.0.59 executable.' }
    foreach ($name in $names) {
        if (!(Test-Path -LiteralPath (Join-Path $plugins $name))) { throw "Missing $name. Install compatible FusionFix and run it once first." }
    }
    $oldDll=Join-Path $plugins $names[0]
    $version=(Get-Item -LiteralPath $oldDll).VersionInfo.ProductVersion
    if ($version -notmatch '^5\.0\.1([.\-]|$)') { throw "Unvalidated FusionFix version: $version. This release targets the 5.0.1 baseline." }
    $newDll=Join-Path $PSScriptRoot 'install\plugins\GTAIV.EFLC.FusionFix.asi'
    if ((Get-FileHash -LiteralPath $newDll).Hash -ne $expectedDll) { throw 'Release DLL checksum failed. Extract the complete archive again.' }
    $stamp=Get-Date -Format 'yyyyMMdd-HHmmss-fff'
    $backup=Join-Path $backupRoot $stamp
    New-Item -ItemType Directory -Path $backup -Force | Out-Null
    $records=@()
    foreach ($name in $names) {
        $src=Join-Path $plugins $name; $dst=Join-Path $backup $name
        $hash=(Get-FileHash -LiteralPath $src).Hash
        Copy-Item -LiteralPath $src -Destination $dst
        if ((Get-FileHash -LiteralPath $dst).Hash -ne $hash) { throw "Backup failed: $name" }
        $records+=@{name=$name;sha256=$hash}
    }
    @{files=$records;release='Unofficial build 18';createdAtUtc=[DateTime]::UtcNow.ToString('o')} |
        ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $backup 'receipt.json') -Encoding UTF8
    try {
        Assert-GameClosed
        Write-IniMerge (Join-Path $plugins $names[1]) @{SHADOWS=@{ExperimentalPlayerShadowAllocation=2;ExperimentalOwnHeadlightCasterFix=1;ExperimentalShadowDiagnostics=1}}
        Write-IniMerge (Join-Path $plugins $names[2]) @{SHADOWS=@{ExtraNightShadows=3};UPDATE=@{CheckForUpdates=0}}
        Copy-Item -LiteralPath $newDll -Destination $oldDll -Force
        if ((Get-FileHash -LiteralPath $oldDll).Hash -ne $expectedDll) { throw 'Installed DLL checksum failed.' }
        [System.IO.File]::WriteAllText((Join-Path $backupRoot 'latest.txt'),$stamp)
    } catch {
        foreach ($name in $names) { Copy-Item -LiteralPath (Join-Path $backup $name) -Destination (Join-Path $plugins $name) -Force }
        throw
    }
    Write-Host 'Unofficial Extra Night Shadows build 18 installed and verified.'
    Write-Host 'Your other graphics settings were retained. Official update checks are disabled to preserve this replacement.'
    Write-Host "Rollback backup: $backup"
    Write-Host 'Start GTA IV normally. Check GTAIV-shadow-candidate18.log to confirm activation.'
} catch { Write-Error $_; exit 1 }
