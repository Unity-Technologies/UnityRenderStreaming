(gc $Args[0]).Replace($Args[1], $Args[2]) | Out-String | % { [Text.Encoding]::UTF8.GetBytes($_) } | Set-Content -Path $Args[0] -Encoding Byte
