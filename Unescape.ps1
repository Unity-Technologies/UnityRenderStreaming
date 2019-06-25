Add-Type -AssemblyName System.Web
(gc $Args[0]) | Out-String | % { [System.Web.HttpUtility]::HtmlDecode($_) } | Set-Content -Path $Args[0]