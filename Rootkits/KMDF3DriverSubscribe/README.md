## Rootkits - Subscribe

---

### Driver

_**DriverSubscribe.c**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver is responsible for subscribing to various kernel event notifications in the Windows operating system. It sets up event handlers to receive notifications related to process creation, image loading, and thread creation.

---

### Source Code

_**DriverSubscribe.c**_
```
// -----------------------------------------------------------
// Name: KMDF3DriverSubscribe
// Visual Studio Project: Template -> Kernel Mode Driver, Empty (KMDF)


// This header file is used by Windows file system and filter driver developers. 
#include <ntifs.h>																		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/
// This header is used by kernel.
#include <ntddk.h>																		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header contains reference material that includes specific details about the routines, structures, and data types that you will need to use to write kernel-mode drivers.
#include <wdm.h>																		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/


/**
	@brief		This function serves as a process creation and termination notification routine.

	@param		ppid		Parent process ID.
	@param		pid			Process ID.
	@param		create		A boolean flag to indicate whether a process is being created (TRUE) or teminated (FALSE).
**/

VOID
sCreateProcessNotifyRoutine(
	_In_	HANDLE		ppid,
	_In_	HANDLE		pid,
	_In_	BOOLEAN		create
)
{
	if (create)
	{
		// Variables
		PEPROCESS process = NULL;														// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/eprocess
		PUNICODE_STRING parentProcessName = NULL, processName = NULL;					// https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string

		// The PsLookupProcessByProcessId routine accepts the process ID of a process and returns a referenced pointer to EPROCESS structure of the process.
		PsLookupProcessByProcessId(ppid, &process);										// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-pslookupprocessbyprocessid
		// The SeLocateProcessImageName routine returns the full process image name for the specified process.
		SeLocateProcessImageName(process, &parentProcessName);							// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-selocateprocessimagename

		// The PsLookupProcessByProcessId routine accepts the process ID of a process and returns a referenced pointer to EPROCESS structure of the process.
		PsLookupProcessByProcessId(pid, &process);										// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-pslookupprocessbyprocessid
		// The SeLocateProcessImageName routine returns the full process image name for the specified process.
		SeLocateProcessImageName(process, &processName);								// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-selocateprocessimagename

		// Print a debug message to indicate a process was created
		DbgPrint("%d %wZ\n\t\t%d %wZ", ppid, parentProcessName, pid, processName);
	}
	else
	{
		// Print a debug message to indicate a process was terminated
		DbgPrint("Process %d lost child %d", ppid, pid);
	}
}


/**
	@brief		This function serves as an extended process creation notification routine.

	@param		process			Pointer to a process structure.
	@param		pid				Process ID.
	@param		createInfo		A pointer to a structure that holds detailed information about the new created process.
**/

VOID
sCreateProcessNotifyRoutineEx(
	_In_	PEPROCESS					process,										// https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/eprocess
	_In_	HANDLE						pid,
	_In_	PPS_CREATE_NOTIFY_INFO		createInfo										// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_ps_create_notify_info
)
{
	// Preventing compiler warnings for unused parameters
	UNREFERENCED_PARAMETER(process);
	UNREFERENCED_PARAMETER(pid);

	if (createInfo != NULL)
	{
		// Detect and handle the launch of a specific process
		if (wcsstr(createInfo->CommandLine->Buffer, L"mspaint") != NULL)
		{
			// Print a debug message to indicate an image was loaded
			DbgPrint("Rootkit POC: Access to launch mspaint was denied");

			// Deny access
			createInfo->CreationStatus = STATUS_ACCESS_DENIED;
		}
	}
}


/**
	@brief		This function serves as a notification routine for loaded image events.

	@param		imageName		Image name.
	@param		pid				Process ID.
	@param		imageInfo		A pointer to a structure that holds detailed information about the loaded image.
**/

VOID
sLoadImageNotifyRoutine(
	_In_	PUNICODE_STRING		imageName,												// https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	_In_	HANDLE				pid,
	_In_	PIMAGE_INFO			imageInfo												// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_image_info
)
{
	// Preventing compiler warnings for unused parameter
	UNREFERENCED_PARAMETER(imageInfo);

	// Variables
	PEPROCESS process = NULL;
	PUNICODE_STRING processName = NULL;

	// The PsLookupProcessByProcessId routine accepts the process ID of a process and returns a referenced pointer to EPROCESS structure of the process.
	PsLookupProcessByProcessId(pid, &process);											// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-pslookupprocessbyprocessid
	// The SeLocateProcessImageName routine returns the full process image name for the specified process.
	SeLocateProcessImageName(process, &processName);									// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-selocateprocessimagename

	// Print a debug message to indicate an image was loaded
	DbgPrint("%wZ (%d) loaded %wZ", processName, pid, imageName);
}


/**
	@brief		This function serves as a notification routine for thread creation and exit events.

	@param		pid			Process ID.
	@param		tid			Thread ID.
	@param		create		A boolean flag to indicate whether a thread is being created (TRUE) or teminated (FALSE).
**/

VOID
sCreateThreadNotifyRoutine(
	_In_	HANDLE		pid,
	_In_	HANDLE		tid,
	_In_	BOOLEAN		create
)
{
	if (create)
	{
		// Print a debug message to indicate a thread was created
		DbgPrint("%d created thread %d", pid, tid);
	}
	else
	{
		// Print a debug message to indicate a thread was terminated
		DbgPrint("Thread %d of process %d terminated", tid, pid);
	}
}


/**
    @brief      Unloads a Windows kernel-mode driver.

                This function is called when the driver is being unloaded from memory. It is responsible for cleaning up resources and performing necessary cleanup tasks before the driver is removed from the system. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
    
    @param      pDriverObject       Pointer to a DRIVER_OBJECT structure representing the driver.
**/

VOID
DriverUnload(
	_In_	PDRIVER_OBJECT		pDriverObject                                           // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
)
{
	// Preventing compiler warnings for unused parameter
    UNREFERENCED_PARAMETER(pDriverObject);

	// Print a debug message to indicate the unsubscription to various kernel event notifications
	DbgPrint("Rootkit POC: Unsubscribe to notifications");

	// The PsSetCreateProcessNotifyRoutine routine adds a driver-supplied callback routine to, or removes it from, a list of routines to be called whenever a process is created or deleted.
	PsSetCreateProcessNotifyRoutine(sCreateProcessNotifyRoutine, TRUE);					// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreateprocessnotifyroutine
	// The PsRemoveLoadImageNotifyRoutine routine removes a callback routine that was registered by the PsSetLoadImageNotifyRoutine routine.
	PsRemoveLoadImageNotifyRoutine(sLoadImageNotifyRoutine);							// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-psremoveloadimagenotifyroutine
	// The PsRemoveCreateThreadNotifyRoutine routine removes a callback routine that was registered by the PsSetCreateThreadNotifyRoutine routine.
	PsRemoveCreateThreadNotifyRoutine(sCreateThreadNotifyRoutine);						// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-psremovecreatethreadnotifyroutine
	// The PsSetCreateProcessNotifyRoutineEx routine registers or removes a callback routine that notifies the caller when a process is created or exits.
	// If PsSetCreateProcessNotifyRoutineEx is not working, you will need to add the option "/integritycheck" to "Configuration Properties" -> "Linker" -> "Command Line" (https://learn.microsoft.com/en-us/cpp/build/reference/integritycheck-require-signature-check)
	PsSetCreateProcessNotifyRoutineEx(sCreateProcessNotifyRoutineEx, TRUE);				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreateprocessnotifyroutineex

	// Print a debug message to indicate the driver has been unloaded
    DbgPrint("Rootkit POC: Unloading... Service has stopped");							// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
}


/**
    @brief      Entry point for a Windows kernel-mode driver.
    
                This function is called when the driver is loaded into memory. It initializes the driver and performs necessary setup tasks. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/wdf/driverentry-for-kmdf-drivers
    
    @param      pDriverObject       Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.
    @param      pRegistryPath       Pointer to a UNICODE_STRING structure, containing the driver's registry path as a Unicode string, indicating the driver's location in the Windows registry.

    @return     A NTSTATUS value indicating success or an error code if initialization fails.
**/

NTSTATUS
DriverEntry(
	_In_	PDRIVER_OBJECT		pDriverObject,                                          // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	_In_	PUNICODE_STRING		pRegistryPath                                           // https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
)
{
	// Preventing compiler warnings for unused parameter
    UNREFERENCED_PARAMETER(pRegistryPath);

	// Set DriverUnload routine
    pDriverObject->DriverUnload = DriverUnload;                                         // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload

	// Print a debug message to indicate the driver has been loaded
    DbgPrint("Rootkit POC: Loading... Hello World");                                    // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint

	// Print a debug message to indicate the subscription to various kernel event notifications
	DbgPrint("Rootkit POC: Subscribe to notifications");

	// The PsSetCreateProcessNotifyRoutine routine adds a driver-supplied callback routine to, or removes it from, a list of routines to be called whenever a process is created or deleted.
	PsSetCreateProcessNotifyRoutine(sCreateProcessNotifyRoutine, FALSE);				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreateprocessnotifyroutine
	// The PsSetLoadImageNotifyRoutine routine registers a driver-supplied callback that is subsequently notified whenever an image (for example, a DLL or EXE) is loaded (or mapped into memory).
	PsSetLoadImageNotifyRoutine(sLoadImageNotifyRoutine);								// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetloadimagenotifyroutine
	// The PsSetCreateThreadNotifyRoutine routine registers a driver-supplied callback that is subsequently notified when a new thread is created and when such a thread is deleted.
	PsSetCreateThreadNotifyRoutine(sCreateThreadNotifyRoutine);							// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreatethreadnotifyroutine
	// The PsSetCreateProcessNotifyRoutineEx routine registers or removes a callback routine that notifies the caller when a process is created or exits.
	// If PsSetCreateProcessNotifyRoutineEx is not working, you will need to add the option "/integritycheck" to "Configuration Properties" -> "Linker" -> "Command Line" (https://learn.microsoft.com/en-us/cpp/build/reference/integritycheck-require-signature-check)
	PsSetCreateProcessNotifyRoutineEx(sCreateProcessNotifyRoutineEx, FALSE);			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-pssetcreateprocessnotifyroutineex

	// Driver initialization was completed successfully
    return STATUS_SUCCESS;
}


// -----------------------------------------------------------
```

---

### Test

1. Build the Solutions:
    - Build -> Build Solution

2. Open DebugView as an Administrator:
    - Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output")

3. Open PowerShell as an Administrator and run the following commands to install the driver:
    - .\sc.exe stop KMDF3DriverSubscribe
    - .\sc.exe delete KMDF3DriverSubscribe
    - .\sc.exe create KMDF3DriverSubscribe type=kernel start=demand binpath="C:\Users\user1\Source\Repos\KMDF3DriverSubscribe\x64\Debug\KMDF3DriverSubscribe.sys"
    - .\sc.exe start KMDF3DriverSubscribe

4. Observe Messages in DebugView :
    - Monitor the output in DebugView for any driver messages.

---

### Files

```
├───KMDF3DriverSubscribe
    │   README.md
    │
    ├───Driver
    │       DriverSubscribe.c
    │
    └───x64
            DriverSubscribe.sys
```



