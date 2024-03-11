#include <ntddk.h>
#include <wdm.h>

#define IOCTL_COMMAND_0 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_COMMAND_1 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_COMMAND_2 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

UNICODE_STRING G_DEVICE_NAME;
UNICODE_STRING G_DEVICE_SYMBOLIC_LINK;

VOID DriverUnload(
    _In_ PDRIVER_OBJECT pDriverObject)
{
    IoDeleteDevice(pDriverObject->DeviceObject);

    IoDeleteSymbolicLink(&G_DEVICE_SYMBOLIC_LINK);

    DbgPrint("Rootkit POC: Unloading... Service has stopped");
}

NTSTATUS
DriverPassthrough(
    _In_ PDEVICE_OBJECT pDeviceObject,
    _In_ PIRP pIrp)
{
    UNREFERENCED_PARAMETER(pDeviceObject);

    pIrp->IoStatus.Status = STATUS_SUCCESS;
    pIrp->IoStatus.Information = 0;

    IoCompleteRequest(pIrp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

NTSTATUS
DriverHandleIOCTL(
    _In_ PDEVICE_OBJECT pDeviceObject,
    _In_ PIRP pIrp)
{
    UNREFERENCED_PARAMETER(pDeviceObject);

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

    ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

    CHAR *message = "Hello User";

    switch (ControlCode)
    {

    case IOCTL_COMMAND_0:
        DbgPrint("Rootkit POC: Received IOCTL_COMMAND_0\n");
        pIrp->IoStatus.Information = 0;
        break;

    case IOCTL_COMMAND_1:
        DbgPrint("Rootkit POC: Received IOCTL_COMMAND_1\n");
        pIrp->IoStatus.Information = strlen(message);
        RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, message, strlen(message));
        break;

    case IOCTL_COMMAND_2:
        DbgPrint("Rootkit POC: Received IOCTL_COMMAND_2\n");
        DbgPrint("Rootkit POC: Input received from userland -> %s", (char *)pIrp->AssociatedIrp.SystemBuffer);
        pIrp->IoStatus.Information = strlen(message);
        RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, message, strlen(message));
        break;

    default:
        DbgPrint("Rootkit POC: Invalid IOCTL\n");
        break;
    }

    pIrp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(pIrp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT pDriverObject,
    _In_ PUNICODE_STRING pRegistryPath)
{
    UNREFERENCED_PARAMETER(pRegistryPath);

    NTSTATUS Status;

    RtlInitUnicodeString(&G_DEVICE_NAME, L"\\Device\\MyKernelDriver");
    RtlInitUnicodeString(&G_DEVICE_SYMBOLIC_LINK, L"\\DosDevices\\MyKernelDriver");

    pDriverObject->DriverUnload = DriverUnload;
    pDriverObject->MajorFunction[IRP_MJ_CREATE] = DriverPassthrough;
    pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverPassthrough;
    pDriverObject->MajorFunction[IRP_MJ_READ] = DriverPassthrough;
    pDriverObject->MajorFunction[IRP_MJ_WRITE] = DriverPassthrough;
    pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverHandleIOCTL;

    Status = IoCreateDevice(pDriverObject, 0, &G_DEVICE_NAME, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDriverObject->DeviceObject);
    if (!NT_SUCCESS(Status))
    {
        DbgPrint("Rootkit POC Failed: Error creating device -> Status: 0x%08X\n", Status);
        return Status;
    }

    Status = IoCreateSymbolicLink(&G_DEVICE_SYMBOLIC_LINK, &G_DEVICE_NAME);
    if (!NT_SUCCESS(Status))
    {
        DbgPrint("Rootkit POC Failed: Error creating symbolic link -> Status: 0x%08X\n", Status);
        return Status;
    }

    DbgPrint("Rootkit POC: Loading... Hello World");

    return STATUS_SUCCESS;
}
