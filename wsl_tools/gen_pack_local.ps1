# Local Pack Generation using WSL
# Double-click or run this script from PowerShell to generate a CMSIS Pack.

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "letter_shell Pack local generation tool (WSL)" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

try {
    $null = wsl --status 2>&1
    if ($LASTEXITCODE -ne 0) { throw }
} catch {
    Write-Host "Error: WSL is not installed or not accessible." -ForegroundColor Red
    Write-Host "Install WSL first: wsl --install" -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

$winPath = Split-Path -Parent $PSScriptRoot
$driveLetter = $winPath.Substring(0, 1).ToLower()
$restOfPath = $winPath.Substring(2) -replace '\\', '/'
$wslPath = "/mnt/$driveLetter$restOfPath"

Write-Host "Working directory: $winPath" -ForegroundColor Yellow
Write-Host ""

Write-Host "Generating Pack..." -ForegroundColor Green
wsl bash -c "cd '$wslPath/wsl_tools' && sed -i 's/\r$//' gen_pack_wsl.sh && chmod +x gen_pack_wsl.sh"
wsl bash -c "cd '$wslPath' && bash wsl_tools/gen_pack_wsl.sh"

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "Generation successful." -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""

    if (Test-Path "$winPath\cmsis-pack") {
        Write-Host "Output files:" -ForegroundColor Cyan
        Get-ChildItem "$winPath\cmsis-pack\*.pack" | ForEach-Object {
            $size = [math]::Round($_.Length / 1KB, 1)
            Write-Host "  $($_.Name) ($size KB)" -ForegroundColor White
        }
    }
} else {
    Write-Host ""
    Write-Host "Generation failed. Check the error output above." -ForegroundColor Red
}

Write-Host ""
Read-Host "Press Enter to exit"
