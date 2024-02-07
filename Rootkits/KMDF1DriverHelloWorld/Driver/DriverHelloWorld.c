#include <ntddk.h>


VOID
DriverUnload(
    _In_    PDRIVER_OBJECT      pDriverObject
)
{
    UNREFERENCED_PARAMETER(pDriverObject);

    DbgPrint("Rootkit POC: Unloading... Service has stopped");
}


NTSTATUS
DriverEntry(
    _In_    PDRIVER_OBJECT      pDriverObject,
    _In_    PUNICODE_STRING     pRegistryPath
)
{
    UNREFERENCED_PARAMETER(pRegistryPath);

    pDriverObject->DriverUnload = DriverUnload;

    DbgPrint("Rootkit POC: Loading... Hello World");

    return STATUS_SUCCESS;
}
