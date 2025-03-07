// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:							KernelRootkit002_Threading
// IDE:								Open Visual Studio
// Template:						Create a new project -> Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:							Project Name: KMDFDriver_Threading -> Solution Name: KernelRootkit002_Threading -> Create
// Source File:						Source Files -> Add -> New Item... -> Driver.c
// Source Code:						Open Driver.c and copy the corresponding source code
// Build Project:					Set Configuration to Release, x64 -> Build -> Build Solution
// Locate Driver:					C:\Users\%USERNAME%\source\repos\KernelRootkit002_Threading\x64\Release\KMDFDriver_Threading.sys
// Virtual Machine:					Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move Driver:						Copy KMDFDriver_Threading.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_Threading.sys (VM)
// Enable Test Mode:				Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:				Open a CMD window as Administrator -> sc.exe create WindowsKernelThreading type=kernel start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_Threading.sys"
// Registered Driver:				Open AutoRuns as Administrator -> Navigate to the Drivers tab -> Look for WindowsKernelThreading
// Service Status:					Run in CMD as Administrator -> sc.exe query WindowsKernelThreading -> driverquery.exe
// Registry Entry:					Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelThreading
// Monitor Messages:				Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Start Routine:					Run in CMD as Administrator -> sc.exe start WindowsKernelThreading
// Clean:							Run in CMD as Administrator -> sc.exe stop WindowsKernelThreading
// Remove:							Run in CMD as Administrator -> sc.exe delete WindowsKernelThreading



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>



// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------
// START -> MACROS ----------------------------------------------------------------------------------------------------------------------------



#define MACRO_INTERVAL_SECONDS_PRINT_HELLO_WORLD 10



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



/**
	@brief		Global handle for the system thread.

				This variable stores the handle to the system thread created by the driver. It is used for managing the thread lifecycle, including waiting for its termination and closing the handle during cleanup.
**/
HANDLE Global_ThreadHandle = NULL;

/**
	@brief		Signal to stop the thread.

				This global boolean variable acts as a flag to gracefully stop the execution of the system thread. When set to TRUE, the thread terminates its main loop.
**/
BOOLEAN Global_ThreadStop = FALSE;

/**
	@brief		Pointer to the system thread object.

				This global pointer stores a reference to the KTHREAD object corresponding to the created system thread. It is useful for operations like cleanup or validation of the thread status.
**/
PKTHREAD Global_ThreadPointer = NULL;



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief		Periodically logs a "Hello World" message with a timestamp to the kernel debugger.

				This function runs in a separate system thread and repeatedly logs a message "Hello World" to the kernel debugger at fixed intervals defined by a macro.
**/
VOID
LocalFunction_PrintHelloWorldPeriodically()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (LocalFunction_PrintHelloWorldPeriodically) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables

	// Represents a 64-bit signed integer value.
	// https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-large_integer-r1
	LARGE_INTEGER interval;


	// ---------------------------------------------------------------------------------------------------------------------
	// Set the interval to the configured number of seconds
	interval.QuadPart = -(LONGLONG)(MACRO_INTERVAL_SECONDS_PRINT_HELLO_WORLD * 1000 * 1000 * 10);


	// ---------------------------------------------------------------------------------------------------------------------
	// Log message indicating the thread has started
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (LocalFunction_PrintHelloWorldPeriodically) - Thread started\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Loop
	while (!Global_ThreadStop)
	{


		// -----------------------------------------------------------------------------------------------------------------
		// Current System Time

		// The KeQuerySystemTime routine obtains the current system time.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-kequerysystemtime-r1
		LARGE_INTEGER currentTime;
		KeQuerySystemTime(&currentTime);


		// -----------------------------------------------------------------------------------------------------------------
		// Log the "Hello World" message along with the current timestamp
		DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (LocalFunction_PrintHelloWorldPeriodically) - Hello World, Timestamp: %lld\n", currentTime.QuadPart);


		// -----------------------------------------------------------------------------------------------------------------
		// Delay Execution

		// The KeDelayExecutionThread routine puts the current thread into an alertable or nonalertable wait state for a specified interval.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-kedelayexecutionthread
		KeDelayExecutionThread(KernelMode, FALSE, &interval);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Log message indicating the thread has stopped
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (LocalFunction_PrintHelloWorldPeriodically) - Thread stopped\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Terminate the current thread with STATUS_SUCCESS

	// The PsTerminateSystemThread routine terminates the current system thread.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-psterminatesystemthread
	PsTerminateSystemThread(STATUS_SUCCESS);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye - Not Reached

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (LocalFunction_PrintHelloWorldPeriodically) - Bye");
}



/**
	@brief		Unloads a Windows kernel-mode driver.

				This function is called when the driver is being unloaded from memory. It is responsible for cleaning up resources and performing necessary cleanup tasks before the driver is removed from the system. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload


				PDRIVER_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param		pDriverObject			Pointer to a DRIVER_OBJECT structure representing the driver.
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
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverUnload) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate the driver is being unloaded
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverUnload) - Unload routine invoked");
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverUnload) - Unloading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Signal the thread to stop
	Global_ThreadStop = TRUE;


	// ---------------------------------------------------------------------------------------------------------------------
	// Thread Cleanup
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverUnload) - Thread Cleanup");
	if (Global_ThreadHandle)
	{


		// -----------------------------------------------------------------------------------------------------------------
		// Wait for the thread to terminate
		DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverUnload) - Wait for the thread to terminate");
		if (Global_ThreadPointer)
		{


			// -------------------------------------------------------------------------------------------------------------
			// Wait for the thread to signal termination

			// The KeWaitForSingleObject routine puts the current thread into a wait state until the given dispatcher object is set to a signaled state or (optionally) until the wait times out.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-kewaitforsingleobject
			status = KeWaitForSingleObject(Global_ThreadPointer, Executive, KernelMode, FALSE, NULL);

			// Failed
			if (!NT_SUCCESS(status))
			{
				DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverUnload) - ERROR: Failed to wait for thread termination. Status: 0x%08x.", status);
			}


			// -------------------------------------------------------------------------------------------------------------
			// Dereference the thread object

			// The ObDereferenceObject routine decrements the given object's reference count and performs retention checks.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-obdereferenceobject
			ObDereferenceObject(Global_ThreadPointer);


			// -------------------------------------------------------------------------------------------------------------
			// Clear the thread pointer
			Global_ThreadPointer = NULL;
		}


		// -----------------------------------------------------------------------------------------------------------------
		// Close the thread handle

		// The ZwClose routine closes an object handle.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
		ZwClose(Global_ThreadHandle);


		// -----------------------------------------------------------------------------------------------------------------
		// Clear the thread handle
		Global_ThreadHandle = NULL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverUnload) - Bye");
}



/**
	@brief		Entry point for a Windows kernel-mode driver.
	
				This function is called when the driver is loaded into memory. It initializes the driver and performs necessary setup tasks. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/wdf/driverentry-for-kmdf-drivers


				PDRIVER_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param		pDriverObject			Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.


				PUNICODE_STRING			https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param		pRegistryPath			Pointer to a UNICODE_STRING structure, containing the driver's registry path as a Unicode string, indicating the driver's location in the Windows registry.


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
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverEntry) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate DriverEntry (main) function has been invoked
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverEntry) - DriverEntry routine invoked");
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverEntry) - Loading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Set DriverUnload routine
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverEntry) - Set DriverUnload routine");

	// The Unload routine performs any operations that are necessary before the system unloads the driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
	pDriverObject->DriverUnload = DriverUnload;


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize Thread Stop Signal
	Global_ThreadStop = FALSE;


	// ---------------------------------------------------------------------------------------------------------------------
	// Create System Thread
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverEntry) - Create System Thread");

	// The PsCreateSystemThread routine creates a system thread that executes in kernel mode and returns a handle for the thread.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-pscreatesystemthread
	status = PsCreateSystemThread(&Global_ThreadHandle, THREAD_ALL_ACCESS, NULL, NULL, NULL, (PKSTART_ROUTINE)LocalFunction_PrintHelloWorldPeriodically, NULL);

	// Failed
	if (!NT_SUCCESS(status) || Global_ThreadHandle == NULL)
	{
		DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverEntry) - ERROR: Failed to create thread. Status: 0x%08x.", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Reference Thread Object

	// The ObReferenceObjectByHandle routine provides access validation on the object handle, and, if access can be granted, returns the corresponding pointer to the object's body.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-obreferenceobjectbyhandle
	status = ObReferenceObjectByHandle(Global_ThreadHandle, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, (PVOID*)&Global_ThreadPointer, NULL);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverEntry) - ERROR: Failed to reference thread object. Status: 0x%08x.", status);

		// -----------------------------------------------------------------------------------------------------------------
		// Close the thread handle
		// The ZwClose routine closes an object handle.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
		ZwClose(Global_ThreadHandle);

		// -----------------------------------------------------------------------------------------------------------------
		// Clear the thread handle
		Global_ThreadHandle = NULL;

		// -----------------------------------------------------------------------------------------------------------------
		// Return
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Log message indicating the thread has been created
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverEntry) - Thread created");


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone Threading -> KMDFDriverThreading [Driver.c] (DriverEntry) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Driver initialization was completed successfully
	return STATUS_SUCCESS;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------