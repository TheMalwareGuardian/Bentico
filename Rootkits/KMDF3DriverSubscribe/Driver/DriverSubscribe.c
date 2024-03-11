#include <ntifs.h>
#include <ntddk.h>
#include <wdm.h>

VOID sCreateProcessNotifyRoutine(
	_In_ HANDLE ppid,
	_In_ HANDLE pid,
	_In_ BOOLEAN create)
{
	if (create)
	{
		PEPROCESS process = NULL;
		PUNICODE_STRING parentProcessName = NULL, processName = NULL;

		PsLookupProcessByProcessId(ppid, &process);
		SeLocateProcessImageName(process, &parentProcessName);

		PsLookupProcessByProcessId(pid, &process);
		SeLocateProcessImageName(process, &processName);

		DbgPrint("%d %wZ\n\t\t%d %wZ", ppid, parentProcessName, pid, processName);
	}
	else
	{
		DbgPrint("Process %d lost child %d", ppid, pid);
	}
}

VOID sCreateProcessNotifyRoutineEx(
	_In_ PEPROCESS process,
	_In_ HANDLE pid,
	_In_ PPS_CREATE_NOTIFY_INFO createInfo)
{
	UNREFERENCED_PARAMETER(process);
	UNREFERENCED_PARAMETER(pid);

	if (createInfo != NULL)
	{
		if (wcsstr(createInfo->CommandLine->Buffer, L"mspaint") != NULL)
		{
			DbgPrint("Rootkit POC: Access to launch mspaint was denied");

			createInfo->CreationStatus = STATUS_ACCESS_DENIED;
		}
	}
}

VOID sLoadImageNotifyRoutine(
	_In_ PUNICODE_STRING imageName,
	_In_ HANDLE pid,
	_In_ PIMAGE_INFO imageInfo)
{
	UNREFERENCED_PARAMETER(imageInfo);

	PEPROCESS process = NULL;
	PUNICODE_STRING processName = NULL;

	PsLookupProcessByProcessId(pid, &process);
	SeLocateProcessImageName(process, &processName);

	DbgPrint("%wZ (%d) loaded %wZ", processName, pid, imageName);
}

VOID sCreateThreadNotifyRoutine(
	_In_ HANDLE pid,
	_In_ HANDLE tid,
	_In_ BOOLEAN create)
{
	if (create)
	{
		DbgPrint("%d created thread %d", pid, tid);
	}
	else
	{
		DbgPrint("Thread %d of process %d terminated", tid, pid);
	}
}

VOID DriverUnload(
	_In_ PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	DbgPrint("Rootkit POC: Unsubscribe to notifications");

	PsSetCreateProcessNotifyRoutine(sCreateProcessNotifyRoutine, TRUE);
	PsRemoveLoadImageNotifyRoutine(sLoadImageNotifyRoutine);
	PsRemoveCreateThreadNotifyRoutine(sCreateThreadNotifyRoutine);
	PsSetCreateProcessNotifyRoutineEx(sCreateProcessNotifyRoutineEx, TRUE);

	DbgPrint("Rootkit POC: Unloading... Service has stopped");
}

NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT pDriverObject,
	_In_ PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	pDriverObject->DriverUnload = DriverUnload;

	DbgPrint("Rootkit POC: Loading... Hello World");

	DbgPrint("Rootkit POC: Subscribe to notifications");

	PsSetCreateProcessNotifyRoutine(sCreateProcessNotifyRoutine, FALSE);
	PsSetLoadImageNotifyRoutine(sLoadImageNotifyRoutine);
	PsSetCreateThreadNotifyRoutine(sCreateThreadNotifyRoutine);
	PsSetCreateProcessNotifyRoutineEx(sCreateProcessNotifyRoutineEx, FALSE);

	return STATUS_SUCCESS;
}
