## Rootkits - Hello World

---

### Driver

_**DriverHelloWorld.c**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. The driver's primary purpose is to load into the kernel and print a "Hello World" debug message as part of its initialization process. It also includes a DriverUnload function to handle the unloading and cleanup of the driver.

---

### Source Code

_**DriverHelloWorld.c**_
```
// -----------------------------------------------------------
// Name: KMDF1DriverHelloWorld
// Visual Studio Project: Template -> Kernel Mode Driver, Empty (KMDF)
// Source Code:
// https://learn.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/writing-a-very-small-kmdf--driver


// This header is used by kernel
#include <ntddk.h>                                                  // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/


/**
    @brief      Unloads a Windows kernel-mode driver.

                This function is called when the driver is being unloaded from memory. It is responsible for cleaning up resources and performing necessary cleanup tasks before the driver is removed from the system. For guidelines and implementation details, see the Microsoft documentation at: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload
    
    @param      pDriverObject       Pointer to a DRIVER_OBJECT structure representing the driver.
**/

VOID
DriverUnload(
    _In_    PDRIVER_OBJECT      pDriverObject                       // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
)
{
    // Preventing compiler warnings for unused parameter
    UNREFERENCED_PARAMETER(pDriverObject);

    // Print a debug message to indicate the driver has been unloaded
    DbgPrint("Rootkit POC: Unloading... Service has stopped");      // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
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
    _In_    PDRIVER_OBJECT      pDriverObject,                      // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
    _In_    PUNICODE_STRING     pRegistryPath                       // https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
)
{
    // Preventing compiler warnings for unused parameter
    UNREFERENCED_PARAMETER(pRegistryPath);

    // Set DriverUnload routine
    pDriverObject->DriverUnload = DriverUnload;                     // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload

    // Print a debug message to indicate the driver has been loaded
    DbgPrint("Rootkit POC: Loading... Hello World");                // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint
    
    // Driver initialization was completed successfully
    return STATUS_SUCCESS;
}


// -----------------------------------------------------------
```

---

### Test

1. Build the Solution:
    - Build -> Build Solution

2. Open DebugView as an Administrator:
    - Enable options ("Capture -> Capture Kernel" and "Capture -> Enable Verbose Kernel Output")

3. Open PowerShell as an Administrator and run the following commands to install the driver:
    - .\sc.exe stop KMDF1DriverHelloWorld
    - .\sc.exe delete KMDF1DriverHelloWorld
    - .\sc.exe create KMDF1DriverHelloWorld type=kernel start=demand binpath="C:\Users\user1\Source\Repos\KMDF1DriverHelloWorld\x64\Debug\KMDF1DriverHelloWorld.sys"
    - .\sc.exe start KMDF1DriverHelloWorld

4. Observe Messages in DebugView:
    - Monitor the output in DebugView for any driver messages.

---

### Files

```
├───KMDF1DriverHelloWorld
    │   README.md
    │
    ├───Driver
    │       DriverHelloWorld.c
    │
    └───x64
            DriverHelloWorld.sys
```
