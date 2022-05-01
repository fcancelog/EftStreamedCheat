/* Cheat that uses a driver for reading / writing virtual memory,
instead of using Win32API Functions. Written By Zer0Mem0ry,
https://www.youtube.com/watch?v=sJdBtPosWQs */




#define IO_READorWRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0341, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_GET_UNITYPLAYER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0342, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#include "ntos.h"
#include "stdint.h"
#include "ntstrsafe.h"
#include "imports.h"
DWORD64  ClientAddress, PID;
PEPROCESS process;
UNICODE_STRING dev, dos; // Driver registry paths
UNICODE_STRING DLLName;
PDEVICE_OBJECT pDeviceObject; // our driver object




uint64_t RDrvGetModuleEntry(PEPROCESS Process, UNICODE_STRING
	module_name)
{
	if (!Process) return 0;
	PPEB peb = PsGetProcessPeb(Process);

	if (!peb) {
		return 0;
	}
	KAPC_STATE state;
	KeStackAttachProcess(Process, &state);
	PPEB_LDR_DATA ldr = peb->Ldr;

	if (!ldr)
	{
		KeUnstackDetachProcess(&state);
		return 0; // failed
	}

	for (PLIST_ENTRY listEntry = (PLIST_ENTRY)ldr->ModuleListLoadOrder.Flink;
		listEntry != &ldr->ModuleListLoadOrder;
		listEntry = (PLIST_ENTRY)listEntry->Flink)
	{

		PLDR_DATA_TABLE_ENTRY ldrEntry = CONTAINING_RECORD(listEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);
		if (RtlCompareUnicodeString(&ldrEntry->BaseDllName, &module_name, TRUE) ==
			0) {
			ULONG64 baseAddr = (ULONG64)ldrEntry->DllBase;
			KeUnstackDetachProcess(&state);
			return baseAddr;
		}

	}

	KeUnstackDetachProcess(&state);

	return 0;
}



typedef struct _KERNEL_GETUNITYPLAYER_REQUEST
{
	uint32_t gameProcessId; //gameprocessId
	uint32_t destProcessId; //usermode hack process id
	uint64_t sourceAddress; // usermode hack buffer

} KERNEL_GETUNITYPLAYER_REQUEST, * PKERNEL_GETUNITYPLAYER_REQUEST;
typedef struct _KERNEL_READorWRITE_REQUEST
{
	uint32_t sourceProcessId;
	uint32_t destProcessId;
	uint64_t sourceAddress;
	uint64_t destAddress;
	uint32_t Size;

} KERNEL_READorWRITE_REQUEST, * PKERNEL_READorWRITE_REQUEST;

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp);
NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp);

NTSTATUS KeWriteVirtualMemory(PEPROCESS sourceProcess, PVOID SourceAddress, PEPROCESS destProcess, PVOID TargetAddress, SIZE_T Size)
{
	PSIZE_T Bytes;
	if (NT_SUCCESS(MmCopyVirtualMemory(sourceProcess, SourceAddress, destProcess,
		TargetAddress, Size, KernelMode, &Bytes)))
		return STATUS_SUCCESS;
	else
		return STATUS_ACCESS_DENIED;
}

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	NTSTATUS Status;
	ULONG BytesIO = 0;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	if (ControlCode == IO_READorWRITE_REQUEST)
	{
		PKERNEL_READorWRITE_REQUEST WriteInput = (PKERNEL_READorWRITE_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		PEPROCESS sourceProcess, destProcess;
		NTSTATUS status;
		PsLookupProcessByProcessId(WriteInput->destProcessId, &destProcess);
		PsLookupProcessByProcessId(WriteInput->sourceProcessId, &sourceProcess);
		status = KeWriteVirtualMemory(sourceProcess, WriteInput->sourceAddress, destProcess, WriteInput->destAddress, WriteInput->Size);
		Status = STATUS_SUCCESS;
		BytesIO = sizeof(KERNEL_READorWRITE_REQUEST);
	}
	else if (ControlCode == IO_GET_UNITYPLAYER)
	{

		PKERNEL_GETUNITYPLAYER_REQUEST GetUnityInput = (PKERNEL_GETUNITYPLAYER_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		PEPROCESS gameProcess, destProcess;
		NTSTATUS status;
		PsLookupProcessByProcessId(GetUnityInput->destProcessId, &destProcess);
		PsLookupProcessByProcessId(GetUnityInput->gameProcessId, &gameProcess);
		
		RtlInitUnicodeString(&DLLName, L"UnityPlayer.dll");
		uint64_t base_address = RDrvGetModuleEntry(gameProcess, DLLName);
		KeWriteVirtualMemory(PsGetCurrentProcess(), &base_address, destProcess, GetUnityInput->sourceAddress, sizeof(uint64_t));
	}
	else
	{
		Status = STATUS_INVALID_PARAMETER;
		BytesIO = 0;
	}
	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = BytesIO;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Status;
}

NTSTATUS DriverInitialize(PDRIVER_OBJECT pDriverObject,
	PUNICODE_STRING pRegistryPath)
{
	RtlInitUnicodeString(&dos, L"\\DosDevices\\Fafaki");
	RtlInitUnicodeString(&dev, L"\\Device\\Fafaki");
	
	IoCreateDevice(pDriverObject, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	IoCreateSymbolicLink(&dos, &dev);
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	pDeviceObject->Flags |= DO_DIRECT_IO;
	return STATUS_SUCCESS;
}
void createdummydevice()
{
	UNICODE_STRING drv_name;
	RtlInitUnicodeString(&drv_name, L"\\Device\\ddmas");
	IoCreateDriver(&drv_name, &DriverInitialize);
}
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING RegistryPath) {
	HANDLE out;
	NTSTATUS result = PsCreateSystemThread(&out, GENERIC_ALL, NULL, NULL, NULL, createdummydevice, NULL);
	return result;
}
NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
