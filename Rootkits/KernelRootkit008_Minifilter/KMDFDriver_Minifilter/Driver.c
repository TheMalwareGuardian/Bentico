// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:							KernelRootkit008_Minifilter
// IDE:								Open Visual Studio
// Template:						Create a new project -> Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:							Project Name: KMDFDriver_Minifilter -> Solution Name: KernelRootkit008_Minifilter -> Create
// Source File:						Source Files -> Add -> New Item... -> Driver.c
// Source Code:						Open Driver.c and copy the corresponding source code
// Library Dependencies:			Open Project Properties -> Linker -> Additional Dependencies -> Add '$(DDK_LIB_PATH)fltMgr.lib;'
// Build Project:					Set Configuration to Release, x64 -> Build -> Build Solution
// Add Project:						In Solution Explorer -> Right-click the solution (KernelRootkit008_Minifilter) -> Add -> New Project...
// Template:						Search for templates (Alt + S) -> Console App -> Next
// Project:							Project Name: ConsoleApp_MinifilterInstallation -> Create
// Source File:						In Solution Explorer -> Find ConsoleApp_MinifilterApplication.cpp -> Rename to Application.c
// Source Code:						Open Application.c and copy the corresponding source code
// Build Project:					Set Configuration to Release, x64 -> Build -> Build Solution
// Locate Driver:					C:\Users\%USERNAME%\source\repos\KernelRootkit008_Minifilter\x64\Release\KMDFDriver_Minifilter.sys
// Locate App:						C:\Users\%USERNAME%\source\repos\KernelRootkit008_Minifilter\x64\Release\ConsoleApp_MinifilterInstallation.exe
// Virtual Machine:					Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move Driver:						Copy KMDFDriver_Minifilter.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_Minifilter.sys (VM)
// Move App:						Move ConsoleApp_MinifilterInstallation.exe (Host) to C:\Users\%USERNAME%\Downloads\ConsoleApp_MinifilterInstallation.exe (VM)
// Enable Test Mode:				Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:
	// Option 1: Manual Registration with sc.exe
			// 1.1 Register Minifilter:	Open a CMD window as Administrator -> sc.exe create WindowsKernelMinifilter type=filesys start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_Minifilter.sys"
			// 1.2 Set Required Registry Keys: Run ConsoleApp_MinifilterInstallation.exe as Administrator
	// or
	// Option 2: Using OSR Driver Loader
			// 2.1 Open OSR Driver Loader -> Select KMDFDriver_Minifilter.sys
			// 2.2 Set type to MiniFilter -> Click Register Service
			// 2.3 Click Start Service to load the driver
	// or
	// Option 3: Auto-Configuration via Driver Entry
			// 3.1 Uncomment the registry setup code inside DriverEntry in KMDFDriver_Minifilter
			// 3.2 Compile and rebuild the driver
			// 3.3 Loading the driver will automatically set the required registry keys
// Check Registered Driver:			Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelMinifilter
// Monitor Minifilter Messages:		Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Load:							Run in CMD as Administrator -> fltmc load WindowsKernelMinifilter
// List Minifilters:				Run in CMD as Administrator -> fltmc
// Unload:							Run in CMD as Administrator -> fltmc unload WindowsKernelMinifilter
// Remove:							Run in CMD as Administrator -> sc.exe delete WindowsKernelMinifilter



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/
// This header is used by ifsk.
#include <fltkernel.h>
#include <dontuse.h>



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



// Global handle for the minifilter
PFLT_FILTER gFilterHandle = NULL;



// List of filenames to block from being deleted
const wchar_t* DeletionBlockList[] = {
	L"DENY_DELETION.TXT"
};



// List of filenames to block from being accessed
const wchar_t* AccessBlockList[] = {
	L"DENY_ACCESS.TXT"
};



// Function Prototypes
NTSTATUS MinifilterUnload(FLT_FILTER_UNLOAD_FLAGS Flags);
FLT_PREOP_CALLBACK_STATUS CallbackPreSetInformation(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext);
FLT_PREOP_CALLBACK_STATUS CallbackPreCreate(PFLT_CALLBACK_DATA Data, PCFLT_RELATED_OBJECTS FltObjects, PVOID* CompletionContext);

// Callback registration array
const FLT_OPERATION_REGISTRATION OperationCallbacks[] = {
	{IRP_MJ_SET_INFORMATION, 0, CallbackPreSetInformation, NULL},	// For blocking file deletion
	{IRP_MJ_CREATE, 0, CallbackPreCreate, NULL},					// For blocking file access
	{IRP_MJ_OPERATION_END}
};



// Filter registration structure
const FLT_REGISTRATION FilterRegistration = {
	sizeof(FLT_REGISTRATION),
	FLT_REGISTRATION_VERSION,
	0,
	NULL,
	OperationCallbacks,
	MinifilterUnload,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief		Unloads the minifilter driver and performs cleanup operations.
**/
NTSTATUS
MinifilterUnload(
	_In_		FLT_FILTER_UNLOAD_FLAGS		Flags
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Prevent warnings for unused parameters
	UNREFERENCED_PARAMETER(Flags);


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate the driver is being unloaded
	DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (MinifilterUnload) - Unload routine invoked");
	DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (MinifilterUnload) - Unloading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the global filter handle is valid before attempting to unregister
	if (gFilterHandle) {

		// -----------------------------------------------------------------------------------------------------------------
		// The FltUnregisterFilter function is being called to unregister the minifilter driver and clean up its internal structures
		DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (MinifilterUnload) - The FltUnregisterFilter function is being called to unregister the minifilter driver and clean up its internal structures\n");

		// A registered minifilter driver calls FltUnregisterFilter to unregister itself so that the Filter Manager no longer calls it to process I/O operations.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltunregisterfilter
		FltUnregisterFilter(gFilterHandle);

		// -----------------------------------------------------------------------------------------------------------------
		// Nullify the global handle to avoid dangling pointers
		gFilterHandle = NULL;


		// -----------------------------------------------------------------------------------------------------------------
		// Message
		DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (MinifilterUnload) - Filter successfully unregistered\n");
	}
	else
	{
		DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (MinifilterUnload) - ERROR: No filter handle found to unregister.\n");
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief		Pre-operation callback to intercept and potentially block file deletion requests.

				This function is invoked for file operations associated with setting file information, specifically when attempting to delete a file. It examines the requested file operation, checks if the target file is in the predefined block list for deletion, and if so, prevents the deletion by denying access.


				FLT_CALLBACK_DATA			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_callback_data
	@param		Data						Pointer to the callback data structure containing details about the file operation.


				PCFLT_RELATED_OBJECTS		https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_related_objects
	@param		FltObjects					Pointer to related objects for the filter operation.


				PVOID						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param		CompletionContext			Pointer to a context for the completion of the operation.


	@return		A FLT_PREOP_CALLBACK_STATUS value indicating whether the operation is allowed or blocked.
**/
FLT_PREOP_CALLBACK_STATUS
CallbackPreSetInformation(
	_In_		PFLT_CALLBACK_DATA			Data,
	_In_		PCFLT_RELATED_OBJECTS		FltObjects,
	_Out_		PVOID*						CompletionContext
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Prevent warnings for unused parameters
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	PFLT_FILE_NAME_INFORMATION filenameInfo = NULL;
	NTSTATUS status;
	WCHAR fileName[200] = {0};


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if the operation is related to file deletion
	if (Data->Iopb->Parameters.SetFileInformation.FileInformationClass == FileDispositionInformation ||
		Data->Iopb->Parameters.SetFileInformation.FileInformationClass == FileDispositionInformationEx)
	{


		// -----------------------------------------------------------------------------------------------------------------
		// The FILE_DISPOSITION_INFORMATION structure is used as an argument to the ZwSetInformationFile routine.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_disposition_information
		PFILE_DISPOSITION_INFORMATION dispositionInfo = (PFILE_DISPOSITION_INFORMATION)Data->Iopb->Parameters.SetFileInformation.InfoBuffer;


		// -----------------------------------------------------------------------------------------------------------------
		// If delete flag is set, proceed with blocking logic
		if (dispositionInfo->DeleteFile) {


			// -------------------------------------------------------------------------------------------------------------
			// File deletion request detected
			// DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (CallbackPreSetInformation) - File deletion request detected\n");


			// -------------------------------------------------------------------------------------------------------------
			// Retrieve the filename information

			// The FltGetFileNameInformation routine returns name information for a file or directory.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltgetfilenameinformation
			status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &filenameInfo);

			// Success
			if (NT_SUCCESS(status))
			{
				// FltParseFileNameInformation parses the contents of a FLT_FILE_NAME_INFORMATION structure.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltparsefilenameinformation
				status = FltParseFileNameInformation(filenameInfo);

				// Success
				if (NT_SUCCESS(status))
				{
					if (filenameInfo->Name.MaximumLength < sizeof(fileName))
					{
						// The RtlCopyMemory routine copies the contents of a source memory block to a destination memory block.
						// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlcopymemory
						RtlCopyMemory(fileName, filenameInfo->Name.Buffer, filenameInfo->Name.MaximumLength);

						// Converts a string to uppercase.
						// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strupr-strupr-l-mbsupr-mbsupr-l-wcsupr-l-wcsupr?view=msvc-170
						_wcsupr(fileName);

						// Iterate over the deletion block list
						for (int i = 0; i < sizeof(DeletionBlockList) / sizeof(DeletionBlockList[0]); ++i)
						{
							// Returns a pointer to the first occurrence of a search string in a string.
							// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strstr-wcsstr-mbsstr-mbsstr-l?view=msvc-170
							if (wcsstr(fileName, DeletionBlockList[i]) != NULL)
							{
								DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (CallbackPreSetInformation) - Deletion operation blocked for %ws\n", fileName);

								// Deny the deletion operation
								Data->IoStatus.Status = STATUS_ACCESS_DENIED;
								Data->IoStatus.Information = 0;

								// Cleanup before exit
								// FltReleaseFileNameInformation releases a file name information structure.
								// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltreleasefilenameinformation
								FltReleaseFileNameInformation(filenameInfo);

								// Return
								return FLT_PREOP_COMPLETE;
							}
						}
					}
				}

				// Release file name information to avoid memory leaks

				// FltReleaseFileNameInformation releases a file name information structure.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltreleasefilenameinformation
				FltReleaseFileNameInformation(filenameInfo);
			}
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// File deletion allowed
	// DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (CallbackPreSetInformation) - File deletion allowed\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}



/**
	@brief		Pre-operation callback to intercept and potentially block file access requests.

				This function is invoked when a file access operation is initiated (IRP_MJ_CREATE). It examines the requested file operation, checks if the target file is in the predefined block list for access restriction, and if so, prevents access by denying it.


				FLT_CALLBACK_DATA			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_callback_data
	@param		Data						Pointer to the callback data structure containing details about the file operation.


				PCFLT_RELATED_OBJECTS		https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/ns-fltkernel-_flt_related_objects
	@param		FltObjects					Pointer to related objects for the filter operation.


				PVOID						https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param		CompletionContext			Pointer to a context for the completion of the operation.


	@return		A FLT_PREOP_CALLBACK_STATUS value indicating whether the operation is allowed or blocked.
**/
FLT_PREOP_CALLBACK_STATUS
CallbackPreCreate(
	_In_		PFLT_CALLBACK_DATA			Data,
	_In_		PCFLT_RELATED_OBJECTS		FltObjects,
	_Out_		PVOID*						CompletionContext
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Prevent warnings for unused parameters
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	PFLT_FILE_NAME_INFORMATION filenameInfo = NULL;
	NTSTATUS status;
	WCHAR fileName[200] = {0};


	// ---------------------------------------------------------------------------------------------------------------------
	// Retrieve the filename information

	// The FltGetFileNameInformation routine returns name information for a file or directory.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltgetfilenameinformation
	status = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &filenameInfo);

	// Success
	if (NT_SUCCESS(status))
	{
		// The FltParseFileNameInformation routine parses the contents of a FLT_FILE_NAME_INFORMATION structure.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltparsefilenameinformation
		status = FltParseFileNameInformation(filenameInfo);

		// Success
		if (NT_SUCCESS(status))
		{
			if (filenameInfo->Name.MaximumLength < sizeof(fileName))
			{
				// The RtlCopyMemory routine copies the contents of a source memory block to a destination memory block.
				// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlcopymemory
				RtlCopyMemory(fileName, filenameInfo->Name.Buffer, filenameInfo->Name.MaximumLength);

				// Converts a string to uppercase.
				// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strupr-strupr-l-mbsupr-mbsupr-l-wcsupr-l-wcsupr?view=msvc-170
				_wcsupr(fileName);

				// Iterate over the access block list
				for (int i = 0; i < sizeof(AccessBlockList) / sizeof(AccessBlockList[0]); ++i)
				{
					// Returns a pointer to the first occurrence of a search string in a string.
					// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strstr-wcsstr-mbsstr-mbsstr-l?view=msvc-170
					if (wcsstr(fileName, AccessBlockList[i]) != NULL)
					{
						DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (CallbackPreCreate) - Access operation blocked for %ws\n", fileName);

						// Deny the file access operation
						Data->IoStatus.Status = STATUS_ACCESS_DENIED;
						Data->IoStatus.Information = 0;

						// Cleanup before exit

						// The FltReleaseFileNameInformation routine releases a file name information structure.
						// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltreleasefilenameinformation
						FltReleaseFileNameInformation(filenameInfo);

						// Return
						return FLT_PREOP_COMPLETE;
					}
				}
			}
		}

		// Release file name information to avoid memory leaks
		// The FltReleaseFileNameInformation routine releases a file name information structure.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltreleasefilenameinformation
		FltReleaseFileNameInformation(filenameInfo);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// File access allowed
	// DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (CallbackPreCreate) - File access allowed\n");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}



/**
	@brief		Entry point for the minifilter driver.

				This function is called when the minifilter driver is loaded into memory. It initializes the driver, registers the filter with the Windows Filter Manager, and starts filtering operations.


				PDRIVER_OBJECT				https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param		DriverObject				Pointer to the driver object representing the minifilter driver in the Windows kernel.


				PUNICODE_STRING				https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
	@param		RegistryPath				Pointer to the registry path where the driver’s configuration settings are stored.


	@return		A NTSTATUS value indicating success or failure of the driver initialization.
**/
NTSTATUS
DriverEntry(
	_In_		PDRIVER_OBJECT		DriverObject,
	_In_		PUNICODE_STRING		RegistryPath
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Prevent warnings for unused parameters
	UNREFERENCED_PARAMETER(RegistryPath);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello message
	DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message indicating that DriverEntry (main function) has been invoked
	DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - Initializing minifilter driver");


	// ---------------------------------------------------------------------------------------------------------------------
	// (Optional) Register Minifilter registry keys
	// Uncomment the following block if you want the driver to create registry keys automatically
	/*

	DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - Creating registry keys for minifilter");

	HANDLE hKey = NULL, hSubKey = NULL;
	OBJECT_ATTRIBUTES keyAttr = RTL_CONSTANT_OBJECT_ATTRIBUTES(RegistryPath, OBJ_KERNEL_HANDLE);

	// The ZwOpenKey routine opens an existing registry key.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwopenkey
	status = ZwOpenKey(&hKey, KEY_WRITE, &keyAttr);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - ERROR: Failed to open registry key. Status: 0x%08X\n", status);
		return status;
	}

	UNICODE_STRING subKey = RTL_CONSTANT_STRING(L"Instances");
	OBJECT_ATTRIBUTES subKeyAttr;
	// The InitializeObjectAttributes macro initializes the opaque OBJECT_ATTRIBUTES structure, which specifies the properties of an object handle to routines that open handles.
	// https://learn.microsoft.com/en-us/windows/win32/api/ntdef/nf-ntdef-initializeobjectattributes
	InitializeObjectAttributes(&subKeyAttr, &subKey, OBJ_KERNEL_HANDLE, hKey, NULL);

	// The ZwCreateKey routine creates a new registry key or opens an existing one.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwcreatekey
	status = ZwCreateKey(&hSubKey, KEY_WRITE, &subKeyAttr, 0, NULL, 0, NULL);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - ERROR: Failed to create Instances registry key. Status: 0x%08X\n", status);
		// The ZwClose routine closes an object handle.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
		ZwClose(hKey);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Set DefaultInstance value
	UNICODE_STRING valueName = RTL_CONSTANT_STRING(L"DefaultInstance");
	WCHAR name[] = L"MinifilterInstance";
	// The ZwSetValueKey routine creates or replaces a registry key's value entry.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwsetvaluekey
	status = ZwSetValueKey(hSubKey, &valueName, 0, REG_SZ, name, sizeof(name));

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - ERROR: Failed to set DefaultInstance registry key. Status: 0x%08X\n", status);
		// The ZwClose routine closes an object handle.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
		ZwClose(hSubKey);
		ZwClose(hKey);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Create instance key under Instances
	UNICODE_STRING instKeyName;

	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlinitunicodestring
	RtlInitUnicodeString(&instKeyName, name);
	HANDLE hInstKey;
	// The InitializeObjectAttributes macro initializes the opaque OBJECT_ATTRIBUTES structure, which specifies the properties of an object handle to routines that open handles.
	// https://learn.microsoft.com/en-us/windows/win32/api/ntdef/nf-ntdef-initializeobjectattributes
	InitializeObjectAttributes(&subKeyAttr, &instKeyName, OBJ_KERNEL_HANDLE, hSubKey, NULL);

	// The ZwCreateKey routine creates a new registry key or opens an existing one.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwcreatekey
	status = ZwCreateKey(&hInstKey, KEY_WRITE, &subKeyAttr, 0, NULL, 0, NULL);

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - ERROR: Failed to create instance registry key. Status: 0x%08X\n", status);
		// The ZwClose routine closes an object handle.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
		ZwClose(hSubKey);
		ZwClose(hKey);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Set Altitude
	WCHAR altitude[] = L"415342";
	UNICODE_STRING altitudeName = RTL_CONSTANT_STRING(L"Altitude");

	// The ZwSetValueKey routine creates or replaces a registry key's value entry.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwsetvaluekey
	status = ZwSetValueKey(hInstKey, &altitudeName, 0, REG_SZ, altitude, sizeof(altitude));

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - ERROR: Failed to set Altitude value in registry. Status: 0x%08X\n", status);
		// The ZwClose routine closes an object handle.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
		ZwClose(hInstKey);
		ZwClose(hSubKey);
		ZwClose(hKey);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Set Flags
	UNICODE_STRING flagsName = RTL_CONSTANT_STRING(L"Flags");
	ULONG flags = 0;

	// The ZwSetValueKey routine creates or replaces a registry key's value entry.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwsetvaluekey
	status = ZwSetValueKey(hInstKey, &flagsName, 0, REG_DWORD, &flags, sizeof(flags));

	// Failed
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - ERROR: Failed to set Flags value in registry. Status: 0x%08X\n", status);
		// The ZwClose routine closes an object handle.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
		ZwClose(hInstKey);
		ZwClose(hSubKey);
		ZwClose(hKey);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Close keys

	// The ZwClose routine closes an object handle.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwclose
	ZwClose(hInstKey);
	ZwClose(hSubKey);
	ZwClose(hKey);


	// ---------------------------------------------------------------------------------------------------------------------
	DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - Registry keys created successfully.");
	*/


	// ---------------------------------------------------------------------------------------------------------------------
	// Register the minifilter with the Filter Manager
	DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - Registering filter with Filter Manager");

	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltregisterfilter
	// FltRegisterFilter registers a minifilter driver.
	status = FltRegisterFilter(DriverObject, &FilterRegistration, &gFilterHandle);

	if (NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - Filter registered successfully");

		// Start filtering operations
		DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - Starting filtering operations");

		// FltStartFiltering starts filtering for a registered minifilter driver.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltstartfiltering
		status = FltStartFiltering(gFilterHandle);

		if (!NT_SUCCESS(status))
		{
			DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - ERROR: Failed to start filtering operations. Unregistering filter. Status: 0x%08X\n", status);
			// A registered minifilter driver calls FltUnregisterFilter to unregister itself so that the Filter Manager no longer calls it to process I/O operations.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nf-fltkernel-fltunregisterfilter
			FltUnregisterFilter(gFilterHandle);
			gFilterHandle = NULL;
		}
		else
		{
			DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - Filtering operations started successfully");
		}
	}
	else
	{
		DbgPrint("Benthic Zone Minifilter -> KMDFDriverMinifilter [Driver.c] (DriverEntry) - ERROR: Failed to register filter");
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return status;
}
