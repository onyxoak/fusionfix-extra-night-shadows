function Set-ShadowIniKeys {
    param([Parameter(Mandatory)][AllowEmptyString()][string]$Text,
          [Parameter(Mandatory)][hashtable]$Sections)
    $newline = if ($Text.Contains("`r`n")) { "`r`n" } else { "`n" }
    $lines = [System.Collections.Generic.List[string]]::new()
    foreach ($line in [regex]::Split($Text, '\r?\n')) { $lines.Add($line) }
    foreach ($section in $Sections.Keys) {
        $start = -1; $end = $lines.Count
        for ($i=0; $i -lt $lines.Count; $i++) {
            if ($lines[$i] -match '^\s*\[([^\]]+)\]') {
                if ($start -ge 0) { $end = $i; break }
                if ($Matches[1] -ieq $section) { $start = $i }
            }
        }
        if ($start -lt 0) { $lines.Add(''); $lines.Add("[$section]"); $start=$lines.Count-1; $end=$lines.Count }
        foreach ($key in $Sections[$section].Keys) {
            $found = $false
            for ($i=$start+1; $i -lt $end; $i++) {
                if ($lines[$i] -match ('^\s*' + [regex]::Escape($key) + '\s*=')) {
                    $lines[$i] = "$key = $($Sections[$section][$key])"
                    $found = $true
                }
            }
            if (!$found) { $lines.Insert($end, "$key = $($Sections[$section][$key])"); $end++ }
        }
    }
    return ($lines -join $newline)
}
Export-ModuleMember -Function Set-ShadowIniKeys
