## Rootkits - IOCTLs

---

### Application

_**ApplicationIOCTLs.c**_ is an application file that serves as the user-level component of a basic rootkit. This application is designed to interact with a kernel-mode driver by sending various IOCTL (Input/Output Control) requests to the driver and receiving responses.

---

### Driver

_**DriverIOCTLs.c**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver is responsible for handling IOCTL requests initiated by a user-level application. It defines IOCTL command codes and implements corresponding functionality for each one.

---

### Source Code

_**ApplicationIOCTLs.c**_
```
// -----------------------------------------------------------
// Name: KMDF2ApplicationIOCTLs
// Visual Studio Project: Template -> Empty Project


// The primary C header file for accessing the Win32 API is the <windows.h> header file. To make a Win32 executable, the first step is to include this header file in your source code. The windows.h header file should be included before any other library include, even the C standard library files such as stdio.h or stdlib.h. This is because the windows.h file includes macros and other components that may modify, extend, or replace things in these libraries. This is especially true when dealing with UNICODE, because windows.h will cause all the string functions to use UNICODE instead. Also, because many of the standard C library functions are already included in the Windows kernel, many of these functions will be available to the programmer without needing to load the standard libraries. For example, the function sprintf is included in windows.h automatically. 
#include <windows.h>                                                // https://en.wikibooks.org/wiki/Windows_Programming/windows.h
// The C programming language provides many standard library functions for file input and output. These functions make up the bulk of the C standard library header <stdio.h>. The I/O functionality of C is fairly low-level by modern standards; C abstracts all file operations into operations on streams of bytes, which may be "input streams" or "output streams". Unlike some earlier programming languages, C has no direct support for random-access data files; to read from a record in the middle of a file, the programmer must create a stream, seek to the middle of the file, and then read bytes in sequence from the stream. 
#include <stdio.h>                                                  // https://en.wikibooks.org/wiki/C_Programming/stdio.h
// String.h is the header in the C standard library for the C programming language which contains macro definitions, constants and declarations of functions and types used not only for string handling but also various memory handling functions; the name is thus something of a misnomer. 
#include <string.h>                                                 // https://en.wikibooks.org/wiki/C_Programming/string.h


/**
    @brief      An I/O control code is a 32-bit value that consists of several fields (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-i-o-control-codes).

                If a new IOCTL will be available to user-mode software components, the IOCTL must be used with IRP_MJ_DEVICE_CONTROL requests. User-mode components send IRP_MJ_DEVICE_CONTROL requests by calling the DeviceIoControl, which is a Win32 function.
                If a new IOCTL will be available only to kernel-mode driver components, the IOCTL must be used with IRP_MJ_INTERNAL_DEVICE_CONTROL requests. Kernel-mode components create IRP_MJ_INTERNAL_DEVICE_CONTROL requests by calling IoBuildDeviceIoControlRequest.

                Use the system-supplied CTL_CODE macro, which is defined in Wdm.h and Ntddk.h, to define new I/O control codes. The definition of a new IOCTL code, whether intended for use with IRP_MJ_DEVICE_CONTROL or IRP_MJ_INTERNAL_DEVICE_CONTROL requests, uses the following format:

                #define IOCTL_Device_Function CTL_CODE(DeviceType, Function, Method, Access)

    @param      DeviceType          This value must match the value that is set in the DeviceType member of the driver's DEVICE_OBJECT structure (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/specifying-device-types).
    @param      FunctionCode        Identifies the function to be performed by the driver. Values of less than 0x800 are reserved for Microsoft. Values of 0x800 and higher can be used by vendors.
    @param      TransferType        Indicates how the system will pass data between the caller of DeviceIoControl (or IoBuildDeviceIoControlRequest) and the driver that handles the IRP (METHOD_BUFFERED, METHOD_IN_DIRECT, METHOD_OUT_DIRECT, METHOD_NEITHER).
    @param      RequiredAccess      Indicates the type of access that a caller must request when opening the file object that represents the device (FILE_ANY_ACCESS, FILE_READ_DATA, FILE_READ_DATA, FILE_READ_DATA and FILE_WRITE_DATA).
**/

#define IOCTL_COMMAND_0 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_COMMAND_1 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_COMMAND_2 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)


/**
    Main entry point
**/

int main()
{

    // Variables
    HANDLE hDevice;                                                 // Handle for interacting with the driver
    DWORD bytesReturned;                                            // Stores the number of bytes returned
    BOOL success;                                                   // Indicates the success of an operation
    int option;                                                     // Stores the user's menu selection
    char inbuffer[15] = { 0 };                                      // Input buffer for data to be sent to the driver
    char outbuffer[15] = { 0 };                                     // Output buffer for data received from the driver


    // Open I/O device
    // The function returns a handle that can be used to access the file or device for various types of I/O depending on the file or device and the flags and attributes specified.
    hDevice = CreateFile(L"\\\\.\\MyKernelDriver", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL); // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
    
    // Error
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        printf("Error: Failed to access the device (MyKernelDriver). Please make sure the driver is installed.\n");
        return 1;
    }

    // Loop
    while (1)
    {

        // Menu
        printf("\nMenu:\n");
        printf("0. Send IOCTL_COMMAND_0\n");
        printf("1. Send IOCTL_COMMAND_1\n");
        printf("2. Send IOCTL_COMMAND_2\n");
        printf("3. Exit\n");
        printf("Select an option: ");

        // Ask user
        scanf_s("%d", &option);                                     // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/scanf-s-scanf-s-l-wscanf-s-wscanf-s-l

        // Handle different options
        switch (option)
        {
        
            // Send IOCTL_COMMAND_0
            case 0:
                // Sends a control code directly to a specified device driver, causing the corresponding device to perform the corresponding operation.
                success = DeviceIoControl(hDevice, IOCTL_COMMAND_0, NULL, 0, NULL, 0, &bytesReturned, NULL); // https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-deviceiocontrol
                if (success)
                {
                    printf("IOCTL_COMMAND_0 sent successfully\n");
                }
                else
                {
                    printf("Error sending IOCTL_COMMAND_0\n");
                }
                break;
            
            // Send IOCTL_COMMAND_1 and receive a message from the kernel driver
            case 1:
                // Sends a control code directly to a specified device driver, causing the corresponding device to perform the corresponding operation.
                success = DeviceIoControl(hDevice, IOCTL_COMMAND_1, NULL, 0, outbuffer, sizeof(outbuffer), &bytesReturned, NULL); // https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-deviceiocontrol
                if (success)
                {
                    printf("IOCTL_COMMAND_1 sent successfully\n");
                    printf("Message received from kernel driver: %s\n", outbuffer);
                }
                else
                {
                    printf("Error sending IOCTL_COMMAND_1\n");
                }
                break;
            
            // Send IOCTL_COMMAND_2 with a message to the kernel driver and receive a response
            case 2:
                // Initialize input buffer
                strcpy_s(inbuffer, 13, "Hello Kernel");             // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strcpy-s-wcscpy-s-mbscpy-s?view=msvc-170
                // Sends a control code directly to a specified device driver, causing the corresponding device to perform the corresponding operation.
                success = DeviceIoControl(hDevice, IOCTL_COMMAND_2, inbuffer, sizeof(inbuffer), outbuffer, sizeof(outbuffer), &bytesReturned, NULL); // https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-deviceiocontrol
                if (success)
                {
                    printf("IOCTL_COMMAND_2 sent successfully\n");
                    printf("Message sent to kernel driver: %s\n", inbuffer);
                    printf("Message received from kernel driver: %s\n", outbuffer);
                }
                else
                {
                    printf("Error sending IOCTL_COMMAND_2\n");
                }
                break;
            
            // Exit
            case 3:
                // Closes an open object handle
                CloseHandle(hDevice);                               // https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
                return 0;
            
            // Invalid menu option
            default:
                printf("Invalid option\n");
                break;
        }
    }

    // Closes an open object handle
    CloseHandle(hDevice);                                           // https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle

    // Exit
    return 0;
}


// -----------------------------------------------------------
```

_**DriverIOCTLs.c**_
```
// -----------------------------------------------------------
// Name: KMDF2DriverIOCTLs
// Visual Studio Project: Template -> Kernel Mode Driver, Empty (KMDF)


// This header is used by kernel
#include <ntddk.h>                                                                      // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/
// This header contains reference material that includes specific details about the routines, structures, and data types that you will need to use to write kernel-mode drivers.
#include <wdm.h>                                                                        // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/


/**
    @brief      An I/O control code is a 32-bit value that consists of several fields (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/defining-i-o-control-codes).

                If a new IOCTL will be available to user-mode software components, the IOCTL must be used with IRP_MJ_DEVICE_CONTROL requests. User-mode components send IRP_MJ_DEVICE_CONTROL requests by calling the DeviceIoControl, which is a Win32 function.
                If a new IOCTL will be available only to kernel-mode driver components, the IOCTL must be used with IRP_MJ_INTERNAL_DEVICE_CONTROL requests. Kernel-mode components create IRP_MJ_INTERNAL_DEVICE_CONTROL requests by calling IoBuildDeviceIoControlRequest.

                Use the system-supplied CTL_CODE macro, which is defined in Wdm.h and Ntddk.h, to define new I/O control codes. The definition of a new IOCTL code, whether intended for use with IRP_MJ_DEVICE_CONTROL or IRP_MJ_INTERNAL_DEVICE_CONTROL requests, uses the following format:

                #define IOCTL_Device_Function CTL_CODE(DeviceType, Function, Method, Access)

    @param      DeviceType          This value must match the value that is set in the DeviceType member of the driver's DEVICE_OBJECT structure (https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/specifying-device-types).
    @param      FunctionCode        Identifies the function to be performed by the driver. Values of less than 0x800 are reserved for Microsoft. Values of 0x800 and higher can be used by vendors.
    @param      TransferType        Indicates how the system will pass data between the caller of DeviceIoControl (or IoBuildDeviceIoControlRequest) and the driver that handles the IRP (METHOD_BUFFERED, METHOD_IN_DIRECT, METHOD_OUT_DIRECT, METHOD_NEITHER).
    @param      RequiredAccess      Indicates the type of access that a caller must request when opening the file object that represents the device (FILE_ANY_ACCESS, FILE_READ_DATA, FILE_READ_DATA, FILE_READ_DATA and FILE_WRITE_DATA).
**/

#define IOCTL_COMMAND_0 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_COMMAND_1 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_COMMAND_2 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)


// Device name and symbolic link
UNICODE_STRING G_DEVICE_NAME;
UNICODE_STRING G_DEVICE_SYMBOLIC_LINK;


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
    // Remove symbolic link from system
    IoDeleteSymbolicLink(&G_DEVICE_SYMBOLIC_LINK);                                      // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletesymboliclink

    // Remove device object from system
    IoDeleteDevice(pDriverObject->DeviceObject);                                        // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iodeletedevice

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
    // Preventing compiler warnings for unused parameter
    UNREFERENCED_PARAMETER(pDeviceObject);

    // Set I/O status information
    pIrp->IoStatus.Status = STATUS_SUCCESS;
    pIrp->IoStatus.Information = 0;
    
    // Complete IRP processing and indicate no increment in stack location
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);                                           // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocompleterequest

    // Operation was completed successfully
    return STATUS_SUCCESS;
}


/**
    @brief      Handles IOCTLs (Input/Output Control) requests from userland.

    @param      pDeviceObject       Pointer to a DEVICE_OBJECT structure representing the device.
    @param      pIrp                Pointer to an IRP (I/O Request Packet) to be processed.

    @return     A NTSTATUS value indicating success or an error code if operation fails.
**/

NTSTATUS
DriverHandleIOCTL(
    _In_    PDEVICE_OBJECT      pDeviceObject,                                          // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_device_object
    _In_    PIRP                pIrp                                                    // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_irp
)
{
    // Preventing compiler warnings for unused parameter
    UNREFERENCED_PARAMETER(pDeviceObject);

    // Get current stack location in IRP
    // IO_STACK_LOCATION structure defines an I/O stack location, which is an entry in the I/O stack that is associated with each IRP. Each I/O stack location in an IRP has some common members and some request-type-specific members.
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);                      // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_io_stack_location, https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iogetcurrentirpstacklocation

    // Get IOCTL code from IRP
    ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

    // Initialize a message variable with the greeting message intended for userland communication
    CHAR* message = "Hello User";

    // Handle different IOCTL codes
    switch (ControlCode)
    {

        // Handle IOCTL_COMMAND_0
        case IOCTL_COMMAND_0:
            DbgPrint("Rootkit POC: Received IOCTL_COMMAND_0\n");
            pIrp->IoStatus.Information = 0;
            break;

        // Handle IOCTL_COMMAND_1
        case IOCTL_COMMAND_1:
            DbgPrint("Rootkit POC: Received IOCTL_COMMAND_1\n");
            pIrp->IoStatus.Information = strlen(message);

            // Copy the content of a source memory block to a destination memory block
            RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, message, strlen(message));  // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlcopymemory
            break;

        // Handle IOCTL_COMMAND_2
        case IOCTL_COMMAND_2:
            DbgPrint("Rootkit POC: Received IOCTL_COMMAND_2\n");
            DbgPrint("Rootkit POC: Input received from userland -> %s", (char*)pIrp->AssociatedIrp.SystemBuffer);
            pIrp->IoStatus.Information = strlen(message);

            // Copy the content of a source memory block to a destination memory block
            RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, message, strlen(message));  // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-rtlcopymemory
            break;

        // Handle invalid IOCTL requests
        default:
            DbgPrint("Rootkit POC: Invalid IOCTL\n");
            break;
    }

    // Set I/O status information
    pIrp->IoStatus.Status = STATUS_SUCCESS;

    // Complete IRP processing and indicate no increment in stack location
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);                                           // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocompleterequest

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
    pDriverObject->MajorFunction[IRP_MJ_CREATE] = DriverPassthrough;                    //https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-create
    pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverPassthrough;                     //https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-close
    pDriverObject->MajorFunction[IRP_MJ_READ] = DriverPassthrough;                      //https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-read
    pDriverObject->MajorFunction[IRP_MJ_WRITE] = DriverPassthrough;                     //https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-write
    pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverHandleIOCTL;            //https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/irp-mj-device-control

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

    // Print a debug message to indicate the driver has been loaded
    DbgPrint("Rootkit POC: Loading... Hello World");                                    // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint

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
    - .\sc.exe stop KMDF2DriverIOCTLs
    - .\sc.exe delete KMDF2DriverIOCTLs
    - .\sc.exe create KMDF2DriverIOCTLs type=kernel start=demand binpath="C:\Users\user1\Source\Repos\KMDF2DriverIOCTLs\x64\Debug\KMDF2DriverIOCTLs.sys"
    - .\sc.exe start KMDF2DriverIOCTLs

4. Open PowerShell and run the following command to execute the application and communicate with the driver:
    - .\ApplicationIOCTLs.exe

5. Observe Messages in DebugView :
    - Monitor the output in DebugView for any driver messages.

---

### Files

```
├───KMDF2DriverIOCTLs
    │   README.md
    │
    ├───Application
    │       ApplicationIOCTLs.c
    │
    ├───Driver
    │       DriverIOCTLs.c
    │
    └───x64
            ApplicationIOCTLs.exe
            DriverIOCTLs.sys
```
