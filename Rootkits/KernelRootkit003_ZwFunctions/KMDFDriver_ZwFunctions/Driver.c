// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:                            KernelRootkit003_ZwFunctions
// IDE:                             Open Visual Studio
// Template:                        Create a new project -> Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:                         Project Name: KMDFDriver_ZwFunctions -> Solution Name: KernelRootkit003_ZwFunctions -> Create
// Source File:                     Source Files -> Add -> New Item... -> Driver.c
// Source Code:                     Open Driver.c and copy the corresponding source code
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Locate Driver:                   C:\Users\%USERNAME%\source\repos\KernelRootkit003_ZwFunctions\x64\Release\KMDFDriver_ZwFunctions.sys
// Virtual Machine:                 Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move Driver:                     Copy KMDFDriver_ZwFunctions.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_ZwFunctions.sys (VM)
// Enable Test Mode:                Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:             Open a CMD window as Administrator -> sc.exe create WindowsKernelZwFunctions type=kernel start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_ZwFunctions.sys"
// Registered Driver:               Open AutoRuns as Administrator -> Navigate to the Drivers tab -> Look for WindowsKernelZwFunctions
// Service Status:                  Run in CMD as Administrator -> sc.exe query WindowsKernelZwFunctions -> driverquery.exe
// Registry Entry:                  Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelZwFunctions
// Monitor Messages:                Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Start Routine:                   Run in CMD as Administrator -> sc.exe start WindowsKernelZwFunctions
// Clean:                           Run in CMD as Administrator -> sc.exe stop WindowsKernelZwFunctions
// Remove:                          Run in CMD as Administrator -> sc.exe delete WindowsKernelZwFunctions



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/
// This header file is used by Windows file system and filter driver developers.
#include <ntifs.h>

// If you use the safe string functions instead of the string manipulation functions that are provided by C-language run-time libraries, you protect your code from buffer overrun errors that can make code untrustworthy. This header declares functions that copy, concatenate, and format strings in a manner that prevents buffer overrun errors.
// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/
#include <ntstrsafe.h>



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



UNICODE_STRING Global_FilePath = RTL_CONSTANT_STRING(L"\\??\\C:\\Hello.txt");



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief		Retrieves and formats the OS version information.
	@details	This function collects only the operating system version information and formats it into a provided buffer for further use or logging.


	@see		CHAR					https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#CHAR
	@param[out]	buffer					Pointer to a buffer where the formatted OS version information will be stored.

	@see		SIZE_T					https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#SIZE_T
	@param[in]	bufferSize				Size of the provided buffer in bytes to ensure no overflow occurs.
**/
VOID
LocalFunction_GetOSVersionInfo(
	_Out_		CHAR*					buffer,
	_In_		SIZE_T					bufferSize
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	SIZE_T offset = 0;
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_GetOSVersionInfo) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Retrieve OS Version Information

	// The RTL_OSVERSIONINFOW structure contains operating system version information. The information includes major and minor version numbers, a build number, a platform identifier, and descriptive text about the operating system. The RTL_OSVERSIONINFOW structure is used with RtlGetVersion.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_osversioninfow
	RTL_OSVERSIONINFOW versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);

	// The RtlGetVersion routine returns version information about the currently running operating system.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlgetversion
	status = RtlGetVersion(&versionInfo);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_GetOSVersionInfo) - ERROR: Failed to get system version. Status: 0x%08x.", status);
	}

	// Add to buffer
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_GetOSVersionInfo) - System Version %d.%d (Build %d)\n", versionInfo.dwMajorVersion, versionInfo.dwMinorVersion, versionInfo.dwBuildNumber);

	// The RtlStringCbPrintfW and RtlStringCbPrintfA functions create a byte-counted text string, with formatting that is based on supplied formatting information.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntstrsafe/nf-ntstrsafe-rtlstringcbprintfa
	status = RtlStringCbPrintfA(buffer + offset, bufferSize - offset, "Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_GetOSVersionInfo) - System Version %d.%d (Build %d)\n", versionInfo.dwMajorVersion, versionInfo.dwMinorVersion, versionInfo.dwBuildNumber);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_GetOSVersionInfo) - Bye");
}



/**
	@brief		Create a file.
	@details	This function creates a file at the specified path. If the file already exists, it will be overwritten.


	@see		UNICODE_STRING			https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param[in]	filePath				Pointer to a UNICODE_STRING structure representing the file path.


	@return		NTSTATUS				Status of the file creation operation.
**/
NTSTATUS
LocalFunction_CreateFile(
	_In_		UNICODE_STRING*			filePath
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables

	// A driver sets an IRP's I/O status block to indicate the final status of an I/O request, before calling IoCompleteRequest for the IRP. https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_io_status_block
	IO_STATUS_BLOCK ioStatusBlock;
	// The OBJECT_ATTRIBUTES structure specifies attributes that can be applied to objects or object handles by routines that create objects and/or return handles to objects. https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_object_attributes
	OBJECT_ATTRIBUTES objAttr;

	HANDLE fileHandle;
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_CreateFile) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize object attributes using the provided file path

	// The InitializeObjectAttributes macro initializes the opaque OBJECT_ATTRIBUTES structure, which specifies the properties of an object handle to routines that open handles.
	// https://learn.microsoft.com/en-us/windows/win32/api/ntdef/nf-ntdef-initializeobjectattributes
	InitializeObjectAttributes(&objAttr, filePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);


	// ---------------------------------------------------------------------------------------------------------------------
	// Create/Overwrite the file
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_CreateFile) - Create/Overwrite the file %wZ", filePath);

	// The ZwCreateFile routine creates a new file or opens an existing file.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwcreatefile
	status = ZwCreateFile(&fileHandle, GENERIC_WRITE, &objAttr, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OVERWRITE_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_CreateFile) - ERROR: Failed to create file. Status: 0x%08x.", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Close file

	// The ZwClose routine closes an object handle.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
	ZwClose(fileHandle);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_CreateFile) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return status;
}



/**
	@brief		Append data to a file.
	@details	This function appends data to the specified file. If the file does not exist, it will fail.


	@see		UNICODE_STRING			https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param[in]	filePath				Pointer to a UNICODE_STRING structure representing the file path.

	@see		VOID*					https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#VOID
	@param[in]	data					Pointer to the data buffer to append.

	@see		SIZE_T					https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#SIZE_T
	@param[in]	dataSize				Size of the data buffer in bytes.


	@return		NTSTATUS				Status of the file append operation.
**/
NTSTATUS
LocalFunction_AppendToFile(
	_In_		UNICODE_STRING*			filePath,
	_In_		VOID*					data,
	_In_		SIZE_T					dataSize
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables

	// A driver sets an IRP's I/O status block to indicate the final status of an I/O request, before calling IoCompleteRequest for the IRP. https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_io_status_block
	IO_STATUS_BLOCK ioStatusBlock;
	// The OBJECT_ATTRIBUTES structure specifies attributes that can be applied to objects or object handles by routines that create objects and/or return handles to objects. https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_object_attributes
	OBJECT_ATTRIBUTES objAttr;

	HANDLE fileHandle;
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_AppendToFile) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize object attributes using the provided file path

	// The InitializeObjectAttributes macro initializes the opaque OBJECT_ATTRIBUTES structure, which specifies the properties of an object handle to routines that open handles.
	// https://learn.microsoft.com/en-us/windows/win32/api/ntdef/nf-ntdef-initializeobjectattributes
	InitializeObjectAttributes(&objAttr, filePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);


	// ---------------------------------------------------------------------------------------------------------------------
	// Open the file
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_AppendToFile) - Open the file %wZ", filePath);

	// The ZwCreateFile routine creates a new file or opens an existing file.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwcreatefile
	status = ZwCreateFile(&fileHandle, FILE_APPEND_DATA | SYNCHRONIZE, &objAttr, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_AppendToFile) - ERROR: Failed to open file. Status: 0x%08x", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Write data to file
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_AppendToFile) - Write data to file %wZ", filePath);

	// The ZwWriteFile routine writes data to an open file.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwwritefile
	status = ZwWriteFile(fileHandle, NULL, NULL, NULL, &ioStatusBlock, data, (ULONG)dataSize, NULL, NULL);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_AppendToFile) - ERROR: Failed to write to file. Status: 0x%08x", status);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Close file

	// The ZwClose routine closes an object handle.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
	ZwClose(fileHandle);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_AppendToFile) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return status;
}



/**
	@brief		Delete a file.


	@see		UNICODE_STRING			https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param[in]	filePath				Pointer to a UNICODE_STRING structure representing the file path.


	@return		NTSTATUS				Status of the file deletion operation.
**/
NTSTATUS
LocalFunction_DeleteFile(
	_In_		UNICODE_STRING*			filePath
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables

	// The OBJECT_ATTRIBUTES structure specifies attributes that can be applied to objects or object handles by routines that create objects and/or return handles to objects. https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_object_attributes
	OBJECT_ATTRIBUTES objAttr;
	
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_DeleteFile) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Initialize object attributes using the provided file path

	// The InitializeObjectAttributes macro initializes the opaque OBJECT_ATTRIBUTES structure, which specifies the properties of an object handle to routines that open handles.
	// https://learn.microsoft.com/en-us/windows/win32/api/ntdef/nf-ntdef-initializeobjectattributes
	InitializeObjectAttributes(&objAttr, filePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);


	// ---------------------------------------------------------------------------------------------------------------------
	// Deleting the file
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_DeleteFile) - Deleting the file %wZ", filePath);

	// The ZwDeleteFile routine deletes the specified file.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwdeletefile
	status = ZwDeleteFile(&objAttr);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_DeleteFile) - ERROR: Failed to delete file. Status: 0x%08x.", status);
	}

	
	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (LocalFunction_DeleteFile) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return status;
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
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverUnload) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate the driver is being unloaded
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverUnload) - Unload routine invoked");
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverUnload) - Unloading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Delete file
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverUnload) - Delete file");
	LocalFunction_DeleteFile(&Global_FilePath);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverUnload) - Bye");
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
	CHAR osInfoBuffer[512];


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverEntry) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate DriverEntry (main) function has been invoked
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverEntry) - DriverEntry routine invoked");
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverEntry) - Loading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Set DriverUnload routine
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverEntry) - Set DriverUnload routine");

	// The Unload routine performs any operations that are necessary before the system unloads the driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
	pDriverObject->DriverUnload = DriverUnload;


	// ---------------------------------------------------------------------------------------------------------------------
	// Create file
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverEntry) - Create file");
	LocalFunction_CreateFile(&Global_FilePath);


	// ---------------------------------------------------------------------------------------------------------------------
	// Get OS Version Information
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverEntry) - Get OS version information");
	LocalFunction_GetOSVersionInfo(osInfoBuffer, sizeof(osInfoBuffer));


	// ---------------------------------------------------------------------------------------------------------------------
	// Append OS Version Information to file
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverEntry) - Append OS Version Information to file");
	LocalFunction_AppendToFile(&Global_FilePath, osInfoBuffer, strlen(osInfoBuffer));


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone ZwFunctions -> KMDFDriverZwFunctions [Driver.c] (DriverEntry) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Driver initialization was completed successfully
	return STATUS_SUCCESS;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
