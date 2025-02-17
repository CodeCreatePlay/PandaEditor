# Set variables
$gitUrl = "https://github.com/git-for-windows/git/releases/download/v2.48.1.windows.1/PortableGit-2.48.1-64-bit.7z.exe"
$buildToolsDir = Join-Path -Path $PSScriptRoot -ChildPath "build_tools"
$gitDir = Join-Path -Path $buildToolsDir -ChildPath "PortableGit"
$buildScript = Join-Path -Path $PSScriptRoot -ChildPath "build.sh"

# Function to handle errors and exit
function Handle-Error {
    param([string]$message)
    Write-Host "Error: $message" -ForegroundColor Red
    Write-Host "Press any key to exit."
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
    exit
}

# Ensure the build_tools directory exists
if (-not (Test-Path $buildToolsDir)) {
    try {
        New-Item -ItemType Directory -Path $buildToolsDir -Force
    } catch {
        Handle-Error "Failed to create build_tools directory."
    }
}

# Extract Portable Git if not already extracted
if (-not (Test-Path "$gitDir\bin\git.exe")) {
	
	$gitArchive = Join-Path -Path $buildToolsDir -ChildPath "PortableGit-2.48.1-64-bit.7z.exe"
	
	Write-Host "Downloading Portable Git."
	
	# Download Portable Git if it doesn't exist
	if (-not (Test-Path $gitArchive)) {
		try {
			Invoke-WebRequest -Uri $gitUrl -OutFile $gitArchive
		} catch {
			Handle-Error "Failed to download Portable Git."
		}
	}
	
    try {
        Write-Host "Extracting..."
        # Start the installer and wait for it to finish
        $process = Start-Process -FilePath $gitArchive -ArgumentList "/SILENT", "/VERYSILENT", "/SUPPRESSMSGBOXES", "/DIR=$gitDir" -PassThru

        # Wait for the process to exit
        $process.WaitForExit()

        # Check if the extraction was successful
        if ($process.ExitCode -ne 0) {
            Handle-Error "Portable Git extraction failed with exit code $($process.ExitCode)."
        }
    } catch {
        Handle-Error "Failed to extract Portable Git."
    }

    # Remove the downloaded archive after extraction
    try {
        Remove-Item -Path $gitArchive -Force
        Write-Host "Deleted archive: $gitArchive`n"
    } catch {
        Handle-Error "Failed to delete the archive after extraction."
    }
}

# Add Git to PATH for this session
$env:PATH = "$gitDir\bin;$gitDir\cmd;$env:PATH"

# Verify Git installation
try {
    $gitVersion = git --version
    # Write-Host "Git version: $gitVersion"
} catch {
    Handle-Error "Git installation verification failed."
}

# Check if build.sh exists and run it
if (Test-Path $buildScript) {
    try {
        bash $buildScript
    } catch {
        Handle-Error "Failed to execute build.sh."
    }
} else {
    Handle-Error "build.sh not found in the script directory."
}
