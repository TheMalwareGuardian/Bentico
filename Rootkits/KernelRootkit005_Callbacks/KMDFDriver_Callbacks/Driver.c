// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:                            KernelRootkit005_Callbacks
// IDE:                             Open Visual Studio
// Template:                        Create a new project -> Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:                         Project Name: KMDFDriver_Callbacks -> Solution Name: KernelRootkit005_Callbacks -> Create
// Source File:                     Source Files -> Add -> New Item... -> Driver.c
// Source Code:                     Open Driver.c and copy the corresponding source code
// Integrity Check:                 Properties -> Configuration Properties -> Linker -> Command Line -> Additional Options -> /integritycheck
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Locate Driver:                   C:\Users\%USERNAME%\source\repos\KernelRootkit005_Callbacks\x64\Release\KMDFDriver_Callbacks.sys
// Virtual Machine:                 Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move Driver:                     Copy KMDFDriver_Callbacks.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_Callbacks.sys (VM)
// Enable Test Mode:                Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:             Open a CMD window as Administrator -> sc.exe create WindowsKernelCallbacks type=kernel start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_Callbacks.sys"
// Registered Driver:               Open AutoRuns as Administrator -> Navigate to the Drivers tab -> Look for WindowsKernelCallbacks
// Service Status:                  Run in CMD as Administrator -> sc.exe query WindowsKernelCallbacks -> driverquery.exe
// Registry Entry:                  Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelCallbacks
// Monitor Messages:                Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Start Routine:                   Run in CMD as Administrator -> sc.exe start WindowsKernelCallbacks
// Clean:                           Run in CMD as Administrator -> sc.exe stop WindowsKernelCallbacks
// Remove:                          Run in CMD as Administrator -> sc.exe delete WindowsKernelCallbacks



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/
// This header file is used by Windows file system and filter driver developers.
#include <ntifs.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel.
#include <ntddk.h>

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/
// This header contains reference material that includes specific details about the routines, structures, and data types that you will need to use to write kernel-mode drivers.
#include <wdm.h>



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief		This function serves as a process creation and termination notification routine. Process-creation callback implemented by a driver to track the system-wide creation and deletion of processes against the driver's internal state. (https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nc-ntddk-pcreate_process_notify_routine).


	@see		HANDLE					https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#HANDLE
	@param[in]	parentProcessId			Parent process ID.

	@see		HANDLE					https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#HANDLE
	@param[in]	processId				Process ID.

	@see		BOOLEAN					https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#BOOLEAN
	@param[in]	create					A boolean flag to indicate whether a process is being created (TRUE) or teminated (FALSE).
**/
VOID
PcreateProcessNotifyRoutine(
	_In_		HANDLE					parentProcessId,
	_In_		HANDLE					processId,
	_In_		BOOLEAN					create
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	if (create)
	{
		// -----------------------------------------------------------------------------------------------------------------
		// Variables

		// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/eprocess
		PEPROCESS process = NULL;

		// https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
		PUNICODE_STRING parentProcessName = NULL, processName = NULL;


		// -----------------------------------------------------------------------------------------------------------------
		// Processes

		// The PsLookupProcessByProcessId routine accepts the process ID of a process and returns a referenced pointer to EPROCESS structure of the process.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-pslookupprocessbyprocessid
		PsLookupProcessByProcessId(parentProcessId, &process);

		// The SeLocateProcessImageName routine returns the full process image name for the specified process.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-selocateprocessimagename
		SeLocateProcessImageName(process, &parentProcessName);

		// The PsLookupProcessByProcessId routine accepts the process ID of a process and returns a referenced pointer to EPROCESS structure of the process.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-pslookupprocessbyprocessid
		PsLookupProcessByProcessId(processId, &process);

		// The SeLocateProcessImageName routine returns the full process image name for the specified process.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-selocateprocessimagename
		SeLocateProcessImageName(process, &processName);


		// -----------------------------------------------------------------------------------------------------------------
		// Print a debug message to indicate a process was created
		DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (PcreateProcessNotifyRoutine) - Process creation detected: Parent Process (ID: %d, Name: %wZ) spawned Child Process (ID: %d, Name: %wZ)", parentProcessId, parentProcessName, processId, processName);
	}
	// ---------------------------------------------------------------------------------------------------------------------
	else
	{
		// -----------------------------------------------------------------------------------------------------------------
		// Print a debug message to indicate a process was terminated
		DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (PcreateProcessNotifyRoutine) - Process termination detected: Parent Process (ID: %d) lost Child Process (ID: %d)", parentProcessId, processId);
	}
}



/**
	@brief		This function serves as an extended process creation notification routine. A callback routine implemented by a driver to inspect and optionally block certain processes based on their command-line parameters or executable names (https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nc-ntddk-pcreate_process_notify_routine_ex).


	@see		PEPROCESS				https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/eprocess
	@param[in]	process					Pointer to a process structure.

	@see		HANDLE					https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#HANDLE
	@param[in]	pid						Process ID.

	@see		PPS_CREATE_NOTIFY_INFO	https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_ps_create_notify_info
	@param[in]	createInfo				A pointer to a structure that holds detailed information about the new created process.
**/
VOID
PcreateProcessNotifyRoutineEx(
	_In_		PEPROCESS				process,
	_In_		HANDLE					processId,
	_In_		PPS_CREATE_NOTIFY_INFO	createInfo
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameters
	UNREFERENCED_PARAMETER(process);
	UNREFERENCED_PARAMETER(processId);


	// ---------------------------------------------------------------------------------------------------------------------
	// Detect and handle the launch of a specific process
	if (createInfo != NULL)
	{


		// -----------------------------------------------------------------------------------------------------------------
		// Array of tools to block
		const wchar_t* blockedTools[] = {
			// Paint
			L"mspaint.exe",
			// Process Monitor
			L"Procmon.exe",
		};


		// -----------------------------------------------------------------------------------------------------------------
		// Iterate through the list
		for (int i = 0; i < sizeof(blockedTools) / sizeof(blockedTools[0]); ++i)
		{


			// -------------------------------------------------------------------------------------------------------------
			// Search for their presence in the command line.

			// Returns a pointer to the first occurrence of a search string in a string.
			// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strstr-wcsstr-mbsstr-mbsstr-l?view=msvc-170
			if (wcsstr(createInfo->CommandLine->Buffer, blockedTools[i]) != NULL)
			{

				// ---------------------------------------------------------------------------------------------------------
				// Print a debug message to indicate the blocking of a specific process
				DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (PcreateProcessNotifyRoutineEx) - Process creation blocked: Tool %ws is contained in command line %ws", blockedTools[i], createInfo->CommandLine->Buffer);

				// ---------------------------------------------------------------------------------------------------------
				// Deny the process creation
				createInfo->CreationStatus = STATUS_ACCESS_DENIED;

				// ---------------------------------------------------------------------------------------------------------
				// Return
				return;
			}
		}
	}
}



/**
	@brief		Unloads a Windows kernel-mode driver.
	@details	This function is called when the driver is being unloaded from memory. It is responsible for cleaning up resources and performing necessary cleanup tasks before the driver is removed from the system. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
	

	@see		PDRIVER_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param[in]	pDriverObject			Pointer to a DRIVER_OBJECT structure representing the driver.
**/
VOID
DriverUnload(
	_In_		PDRIVER_OBJECT			pDriverObject
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter. We're not using the driver object, so we need to mark it as unreferenced.
	UNREFERENCED_PARAMETER(pDriverObject);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverUnload) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate the driver is being unloaded
	DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverUnload) - Unload routine invoked");
	DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverUnload) - Unloading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Unsubscription to kernel event notifications
	DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverUnload) - Unsubscription to kernel event notifications");

	// The PsSetCreateProcessNotifyRoutine routine adds a driver-supplied callback routine to, or removes it from, a list of routines to be called whenever a process is created or deleted.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreateprocessnotifyroutine
	status = PsSetCreateProcessNotifyRoutine(PcreateProcessNotifyRoutine, TRUE);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverUnload) - ERROR: Failed to remove process creation notify routine. Status: 0x%08X\n", status);
	}

	// The PsSetCreateProcessNotifyRoutineEx routine registers or removes a callback routine that notifies the caller when a process is created or exits.
	// If PsSetCreateProcessNotifyRoutineEx is not working, you will need to add the option "/integritycheck" to "Configuration Properties" -> "Linker" -> "Command Line" (https://learn.microsoft.com/en-us/cpp/build/reference/integritycheck-require-signature-check)
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreateprocessnotifyroutineex
	status = PsSetCreateProcessNotifyRoutineEx(PcreateProcessNotifyRoutineEx, TRUE);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverUnload) - ERROR: Failed to remove extended process creation notify routine. Status: 0x%08X\n", status);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverUnload) - Bye");
}



/**
	@brief		Entry point for a Windows kernel-mode driver.
	@details	This function is called when the driver is loaded into memory. It initializes the driver and performs necessary setup tasks. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/wdf/driverentry-for-kmdf-drivers


	@see		PDRIVER_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param[in]	pDriverObject			Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.

	@see		PUNICODE_STRING			https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param[in]	pRegistryPath			Pointer to a UNICODE_STRING structure, containing the driver's registry path as a Unicode string, indicating the driver's location in the Windows registry.


	@return		A NTSTATUS value indicating success or an error code if initialization fails.
**/
NTSTATUS
DriverEntry(
	_In_		PDRIVER_OBJECT			pDriverObject,
	_In_		PUNICODE_STRING			pRegistryPath
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter. We're not using the registry path, so we need to mark it as unreferenced.
	UNREFERENCED_PARAMETER(pRegistryPath);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverEntry) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate DriverEntry (main) function has been invoked
	DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverEntry) - DriverEntry routine invoked");
	DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverEntry) - Loading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Set DriverUnload routine
	DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverEntry) - Set DriverUnload routine");

	// The Unload routine performs any operations that are necessary before the system unloads the driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
	pDriverObject->DriverUnload = DriverUnload;


	// ---------------------------------------------------------------------------------------------------------------------
	// Subscription to kernel event notifications
	DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverEntry) - Subscription to kernel event notifications");

	// The PsSetCreateProcessNotifyRoutine routine adds a driver-supplied callback routine to, or removes it from, a list of routines to be called whenever a process is created or deleted.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreateprocessnotifyroutine
	status = PsSetCreateProcessNotifyRoutine(PcreateProcessNotifyRoutine, FALSE);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverEntry) - ERROR: Failed to register process creation notify routine. Status: 0x%08X\n", status);
		return status;
	}

	// The PsSetCreateProcessNotifyRoutineEx routine registers or removes a callback routine that notifies the caller when a process is created or exits.
	// If PsSetCreateProcessNotifyRoutineEx is not working, you will need to add the option "/integritycheck" to "Configuration Properties" -> "Linker" -> "Command Line" (https://learn.microsoft.com/en-us/cpp/build/reference/integritycheck-require-signature-check)
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreateprocessnotifyroutineex
	status = PsSetCreateProcessNotifyRoutineEx(PcreateProcessNotifyRoutineEx, FALSE);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverEntry) - ERROR: Failed to register extended process creation notify routine. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone Callbacks -> KMDFDriverCallbacks [Driver.c] (DriverEntry) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Driver initialization was completed successfully
	return STATUS_SUCCESS;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------