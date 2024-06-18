#include "Mayhem.h"

PDEVICE_OBJECT gpDeviceObject;
UNICODE_STRING gDeviceString, gDosDeviceString;

NTSTATUS MayhemUnload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	IoDeleteSymbolicLink(&gDosDeviceString);
	IoDeleteDevice(gpDeviceObject);

	return STATUS_SUCCESS;
}

NTSTATUS MayhemCreateCloseIrp(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS MayhemDispatchIrp(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);

	NTSTATUS Status = STATUS_INVALID_PARAMETER;
	ULONG_PTR BytesReturned = 0;
	PIO_STACK_LOCATION pIO = IoGetCurrentIrpStackLocation(pIrp);

	PVOID pSystemBuffer = pIrp->AssociatedIrp.SystemBuffer;
	ULONG InputBufferLength = pIO->Parameters.DeviceIoControl.InputBufferLength;
	ULONG OutputBufferLength = pIO->Parameters.DeviceIoControl.OutputBufferLength;

	// Unused as of now.
	UNREFERENCED_PARAMETER(OutputBufferLength);

	switch (pIO->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_MEMCPY:
	{
		IOCTL_MEMCPY_BUFFER Buffer;

		if (!pSystemBuffer || InputBufferLength != sizeof(Buffer))
		{
			break;
		}

		memcpy(&Buffer, pSystemBuffer, sizeof(Buffer));

		// Allow arbitrary read/write
		memcpy(Buffer.pDestination, Buffer.pSource, Buffer.Size);

		Status = STATUS_SUCCESS;
		break;
	}
	case IOCTL_ELEVATE_PROCESS:
	{
		// TODO: ...

		Status = STATUS_NOT_IMPLEMENTED;
		break;
	}
	}

	pIrp->IoStatus.Status = Status;
	pIrp->IoStatus.Information = BytesReturned;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return Status;
}

// TODO: Add code that patches back g_CiOptions to its default value.
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	NTSTATUS Status = STATUS_SUCCESS;

	RtlInitUnicodeString(&gDeviceString, L"\\Device\\Mayhem");
	RtlInitUnicodeString(&gDosDeviceString, L"\\DosDevices\\Mayhem");

	Status = IoCreateDevice(pDriverObject, 0, &gDeviceString, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &gpDeviceObject);
	if (!NT_SUCCESS(Status))
	{
		return Status;
	}

	Status = IoCreateSymbolicLink(&gDosDeviceString, &gDeviceString);
	if (!NT_SUCCESS(Status))
	{
		return Status;
	}

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = MayhemCreateCloseIrp;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = MayhemCreateCloseIrp;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MayhemDispatchIrp;
	pDriverObject->DriverUnload = MayhemUnload;

	return Status;
}