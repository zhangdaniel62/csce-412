# Requires Powershell 2.0 or higher, with 5.1 reccomended

<#
This file contains a script that will be run on setup to configure the VM to do the following:

  1. Install a code editor: Visual Studio Code
  2. Install a compiler: G++
  3. Install an IDE: Visual Studio
  4. Install a document editor: Libreoffice
  5. Install a PDF viewer: Adobe Acrobat Reader

The following resources were used in the creation of this script:

  1. https://chocolatey.org/install#individual

MAKE SURE TO ADD THE FOLLOWING TAG TO THE LAUNCH SCRIPT OF THE VPS INSTANCE FROM AWS LIGHTHOUSE:

  <powershell></powershell>
#>

# Init arrays to store which apps were installed successfully and not
$installedApps = @()
$notInstalledApps = @()

<#
  =============================================================================
  Functions
  =============================================================================
#>
# Function to install Chocolatey
function InstallChocolatey {
  Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
}

# Function to check if Chocolatey is installed and install if not
function ChocolateyInstalled {
  $check = Get-Command choco.exe -ErrorAction SilentlyContinue
  if ($check) {
    return $true
  } 
  # Attempt to install Chocolatey one more time if it is not installed
  else {
    InstallChocolatey

    $check = Get-Command choco.exe -ErrorAction SilentlyContinue
    if ($check) {
      return $true
    } else {
      return $false
    }
  }
}

function Install {
  param(
    [string]$appName
  )

  $choco = "$env:ProgramData\chocolatey\bin\choco.exe"
  & $choco install $appName -y --no-progress --limit-output

  if ($LASTEXITCODE -eq 0 -or $LASTEXITCODE -eq 3010) {
    $script:installedApps += $appName
  }
  else {
    $script:notInstalledApps += $appName
  } 
}

<#
  =============================================================================
  Main Script
  =============================================================================
#>
# Install Chocolatey
Write-Host "Installing Chocolatey"
if (ChocolateyInstalled) {
  Write-Host "Chocolatey installed successfully`n" 
  $choco = "$env:ProgramData\chocolatey\bin\choco.exe"
  & $choco upgrade chocolatey -y --no-progress --limit-output
  Write-Host "----------------------------------------" -ForegroundColor Gray

  Install -appName "vscode" 
  Install -appName "mingw" 
  Install -appName "visualstudio2019community"
  Install -appName "libreoffice-fresh"
  Install -appName "adobereader"

  Write-Host "----------------------------------------" -ForegroundColor Gray

  Write-Host "Installed: $($installedApps -join ", ")" -ForegroundColor Green
  Write-Host "Not Installed: $($notInstalledApps -join ", ")" -ForegroundColor Red
}
else {
  Write-Host "Chocolatey failed to install. Exiting" -ForegroundColor Red
  exit 1
}