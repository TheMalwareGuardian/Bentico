
function Show-Menu {
    Clear-Host
    Write-Host "=== Options ==="
    Write-Host "1. Requirements -> Visual Studio 2022 Community + SDK + WDK"
    Write-Host "2. Set Up Environment -> Debugging and Signing Mode"
    Write-Host "3. Debug -> WinDbg Preview"
    Write-Host "4. Tools -> Microsoft Sysinternals Suite"
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

    Write-Host "You have selected the option 'Tools -> Microsoft Sysinternals Suite'" -ForegroundColor Green
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
				Write-Host "The folder '$folderToolsPath' already exists in this directory. Unable to proceed." -ForegroundColor Red
			}
		}
    }
}



function OptionKernelModeDriverHellowWorld {

    Write-Host "You have selected the option 'Kernel-Mode Driver -> Hello World'" -ForegroundColor Green
	$response = Read-Host "Do you want to proceed? (Press 'Y')"
	if ($response -eq "Y") {
        Write-Host "Creating files" -ForegroundColor Magenta

		$folderDriverHello = "KernelModeDriverHelloWorld"
		$folderDriverHelloPath = Join-Path -Path $PWD -ChildPath $folderDriverHello
		New-Item -ItemType Directory -Path $folderDriverHelloPath | Out-Null

		$contentDrivercHello = "I2luY2x1ZGUgPG50ZGRrLmg+CgoKVk9JRCBEcml2ZXJVbmxvYWQoX0luXyBQRFJJVkVSX09CSkVDVCBEcml2ZXJPYmplY3QpCnsKICAgIFVOUkVGRVJFTkNFRF9QQVJBTUVURVIoRHJpdmVyT2JqZWN0KTsKCiAgICBEYmdQcmludCgiUm9vdGtpdCBQT0MgZGVzY2FyZ2FuZG8uLi4gU2UgaGEgcGFyYWRvIGVsIHNlcnZpY2lvIik7Cn0KCgpOVFNUQVRVUyBEcml2ZXJFbnRyeShfSW5fIFBEUklWRVJfT0JKRUNUIERyaXZlck9iamVjdCwgX0luXyBQVU5JQ09ERV9TVFJJTkcgUmVnaXN0cnlQYXRoKQp7CiAgICBVTlJFRkVSRU5DRURfUEFSQU1FVEVSKFJlZ2lzdHJ5UGF0aCk7CgogICAgRHJpdmVyT2JqZWN0LT5Ecml2ZXJVbmxvYWQgPSBEcml2ZXJVbmxvYWQ7CgogICAgRGJnUHJpbnQoIlJvb3RraXQgUE9DIG5vcyBzYWx1ZGE6IEhvbGEgbXVuZG8hISEiKTsKCiAgICByZXR1cm4gU1RBVFVTX1NVQ0NFU1M7Cn0K"
		$helloDrivercBytes = [System.Convert]::FromBase64String($contentDrivercHello)
		[System.IO.File]::WriteAllBytes("$folderDriverHelloPath\Driver.c", $helloDrivercBytes)
		
		$contentDrivercExplainHello = "LyoKLy8gLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0KCi8vIE5vbWJyZTogS01ERiBEcml2ZXIgMSBIb2xhIE11bmRvCi8vIFByb3llY3RvIGVuIFZpc3VhbCBTdHVkaW86IFBsYW50aWxsYSAtPiBLZXJuZWwgTW9kZSBEcml2ZXIsIEVtcHR5IChLTURGKQovLyBDw7NkaWdvIEZ1ZW50ZToKCgovLyBMaWJyZXLDrWEgcHJpbmNpcGFsIHF1ZSBpbmNsdXllIGxhcyBkZWZpbmljaW9uZXMgbmVjZXNhcmlhcyBwYXJhIGVsIGRlc2Fycm9sbG8gZGUgZHJpdmVycwojaW5jbHVkZSA8bnRkZGsuaD4KCgovLyBGdW5jacOzbiBkZSBzYWxpZGEsIGVqZWN1dGFkYSBjdWFuZG8gZWwgZHJpdmVyIHNlICJkZXNjYXJnYSIgKGN1YW5kbyBzZSBwYXJlIGVsIHNlcnZpY2lvKQpWT0lEIERyaXZlclVubG9hZCgKICAgIC8vIFB1bnRlcm8gYSB1bmEgZXN0cnVjdHVyYSBEUklWRVJfT0JKRUNUIHF1ZSByZXByZXNlbnRhIGxhIHByb3BpYSBpbWFnZW4gZGVsIGRyaXZlciBlbiBlbCBrZXJuZWwgZGVsIHNpc3RlbWEgb3BlcmF0aXZvCiAgICBfSW5fIFBEUklWRVJfT0JKRUNUICAgICBEcml2ZXJPYmplY3QKKQp7CiAgICAvLyBTZSBldml0YSBsYSBhZHZlcnRlbmNpYSBkZSAicGFyw6FtZXRybyBEcml2ZXJPYmplY3Qgbm8gdXRpbGl6YWRvIgogICAgVU5SRUZFUkVOQ0VEX1BBUkFNRVRFUihEcml2ZXJPYmplY3QpOwogICAgCiAgICAvLyBTZSBpbXByaW1lIHVuYSBjYWRlbmEgZGUgdGV4dG8gY3VhbmRvIHNlIHBhcmEgZWwgc2VydmljaW8KICAgIERiZ1ByaW50KCJSb290a2l0IFBPQyBkZXNjYXJnYW5kby4uLiBTZSBoYSBwYXJhZG8gZWwgc2VydmljaW8iKTsKfQoKCi8vIFB1bnRvIGRlIGVudHJhZGEgZGUgdW4gZHJpdmVyIGVuIGVsIG1vZG8ga2VybmVsIGRlIFdpbmRvd3MsIGVzIGNvbW8gdW4gbWFpbiBlbiBjIG8gcHl0aG9uCk5UU1RBVFVTIERyaXZlckVudHJ5KAogICAgLy8gUHVudGVybyBhIHVuYSBlc3RydWN0dXJhIERSSVZFUl9PQkpFQ1QgcXVlIHJlcHJlc2VudGEgbGEgcHJvcGlhIGltYWdlbiBkZWwgZHJpdmVyIGVuIGVsIGtlcm5lbCBkZWwgc2lzdGVtYSBvcGVyYXRpdm8KICAgIF9Jbl8gUERSSVZFUl9PQkpFQ1QgICAgIERyaXZlck9iamVjdCwKICAgIC8vIFB1bnRlcm8gYSB1bmEgZXN0cnVjdHVyYSBVTklDT0RFX1NUUklORywgcXVlIGNvbnRpZW5lIGxhIHJ1dGEgZGVsIHJlZ2lzdHJvIGRlbCBkcml2ZXIgY29tbyB1bmEgY2FkZW5hIGRlIGNhcmFjdGVyZXMgdW5pY29kZSwgZW4gbGEgcXVlIHNlIGluZGljYSBsYSBsb2NhbGl6YWNpw7NuIGRlbCBkcml2ZXIgZW4gZWwgcmVnaXN0cm8gZGUgV2luZG93cwogICAgX0luXyBQVU5JQ09ERV9TVFJJTkcgICAgUmVnaXN0cnlQYXRoCikKewogICAgLy8gU2UgZXZpdGEgbGEgYWR2ZXJ0ZW5jaWEgZGUgInBhcsOhbWV0cm8gUmVnaXN0cnlQYXRoIG5vIHV0aWxpemFkbyIKICAgIFVOUkVGRVJFTkNFRF9QQVJBTUVURVIoUmVnaXN0cnlQYXRoKTsKICAgIAogICAgLy8gU2UgZXN0YWJsZWNlIGxhIHJ1dGluYSBkZSBzYWxpZGEKICAgIERyaXZlck9iamVjdC0+RHJpdmVyVW5sb2FkID0gRHJpdmVyVW5sb2FkOwoKICAgIC8vIFNlIGltcHJpbWUgdW5hIGNhZGVuYSBkZSB0ZXh0byBwYXJhIGluZGljYXIgcXVlIGVsIGRyaXZlciBzZSBoYSBjYXJnYWRvCiAgICBEYmdQcmludCgiUm9vdGtpdCBQT0Mgbm9zIHNhbHVkYTogSG9sYSBtdW5kbyEhISIpOwoKICAgIC8vIEluZGljYSBxdWUgZWwgZHJpdmVyIHNlIGhhIGluaWNpYWRvIGNvcnJlY3RhbWVudGUKICAgIHJldHVybiBTVEFUVVNfU1VDQ0VTUzsKfQoKLy8gLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0KKi8="
		$helloDrivercExplainBytes = [System.Convert]::FromBase64String($contentDrivercExplainHello)
		[System.IO.File]::WriteAllBytes("$folderDriverHelloPath\DriverExplicado.c", $helloDrivercExplainBytes)
		
		$contentExecuteHello = "CkPDs21vIHByb2JhciBlbCBkcml2ZXI6CgoxICAgLSBDb21waWxhciAtPiBDb21waWxhciBzb2x1Y2nDs24KCjIgICAtIEFicmlyIERlYnVnVmlldyBjb21vIGFkbWluaXN0cmFkb3IKMi4xIC0gTWFyY2FyIGxhcyBvcGNpb25lczogQ2FwdHVyZSAtPiBDYXB0dXJlIEtlcm5lbCB5IENhcHR1cmUgLT4gRW5hYmxlIFZlcmJvc2UgS2VybmVsIE91dHB1dAoKMyAgIC0gQWJyaXIgcG93ZXJzaGVsbCBjb21vIGFkbWluaXN0cmFkb3IKMy4xIC0gLlxzYy5leGUgc3RvcCBLTURGRHJpdmVyMUhvbGFNdW5kbwozLjIgLSAuXHNjLmV4ZSBkZWxldGUgS01ERkRyaXZlcjFIb2xhTXVuZG8KMy4zIC0gLlxzYy5leGUgY3JlYXRlIEtNREZEcml2ZXIxSG9sYU11bmRvIHR5cGU9IGtlcm5lbCBzdGFydD0gZGVtYW5kIGJpbnBhdGg9IkM6XFVzZXJzXHJldnk2NFxTb3VyY2VcUmVwb3NcS01ERiBEcml2ZXIxIEhvbGEgTXVuZG9ceDY0XERlYnVnXEtNREZEcml2ZXIxSG9sYU11bmRvLnN5cyIKMy40IC0gLlxzYy5leGUgc3RhcnQgS01ERkRyaXZlcjFIb2xhTXVuZG8KCjQgICAtIE9ic2VydmFyIG1lbnNhamVzIGVuIERldnVnVmlldwo="
		$helloExecuteBytes = [System.Convert]::FromBase64String($contentExecuteHello)
		[System.IO.File]::WriteAllBytes("$folderDriverHelloPath\Ejecucion.txt", $helloExecuteBytes)
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
