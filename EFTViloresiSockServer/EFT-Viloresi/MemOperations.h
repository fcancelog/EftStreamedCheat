#pragma once
#include "ConstantsAndOffsets.h"
#include "utils.h"

#define DRIVER_NOT_LOADED 1
#define PROCESS_NOT_FOUND 2
DWORD GAME_PROCESS_ID;
DWORD CURRENT_PROCESS_ID;
HANDLE hDriver;

typedef struct _KERNEL_READorWRITE_REQUEST
{
	uint32_t sourceProcessId;
	uint32_t destProcessId;
	uint64_t sourceAddress;
	uint64_t destAddress;
	uint32_t Size;
} KERNEL_READorWRITE_REQUEST, * PKERNEL_READorWRITE_REQUEST;

typedef struct _KERNEL_GETUNITYPLAYER_REQUEST
{
	uint32_t sourceProcessId; //gameprocessId
	uint32_t destProcessId; //usermode hack process id
	uint64_t sourceAddress; // usermode hack buffer

} KERNEL_GETUNITYPLAYER_REQUEST, * PKERNEL_GETUNITYPLAYER_REQUEST;

bool ReadVirtualMemory(HANDLE hDriver, uint32_t sourceProcessId, uint64_t sourceAddress, uint64_t readbuffer, uint32_t Size)
{
	KERNEL_READorWRITE_REQUEST ReadRequest;
	DWORD Bytes;
	ReadRequest.sourceProcessId = sourceProcessId;
	ReadRequest.sourceAddress = sourceAddress;
	ReadRequest.destProcessId = GetCurrentProcessId();
	ReadRequest.destAddress = readbuffer;
	ReadRequest.Size = Size;
	if (DeviceIoControl(hDriver, IO_READorWRITE_REQUEST, &ReadRequest, sizeof(ReadRequest), 0, 0, &Bytes, NULL))
		return true;
	else
		return false;
}

bool GetUnityPlayer(HANDLE hDriver, uint32_t sourceProcessId, uint64_t readbuffer)
{
	KERNEL_GETUNITYPLAYER_REQUEST GetUnityRequest;
	DWORD Bytes;
	GetUnityRequest.sourceProcessId = sourceProcessId;
	GetUnityRequest.sourceAddress = readbuffer;
	GetUnityRequest.destProcessId = GetCurrentProcessId();

	if (DeviceIoControl(hDriver, IO_GET_UNITYPLAYER, &GetUnityRequest, sizeof(GetUnityRequest), 0, 0, &Bytes, NULL))
		return true;
	else
		return false;
}

int Init()
{
	hDriver = CreateFile(DRIVER_FILENAMEW, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	GAME_PROCESS_ID = GetProcessIdByName(GAME_PROCESS_NAME);
	CURRENT_PROCESS_ID = GetCurrentProcessId();
	if (hDriver == INVALID_HANDLE_VALUE)
		return DRIVER_NOT_LOADED;
	else if (GAME_PROCESS_ID == 0)
		return PROCESS_NOT_FOUND;
	else
		return 0;
}

void readmem(void* addresstoread, void* buffer, uint32_t size)
{
	if ((uint64_t)addresstoread < 0x10000) //address in non paged area
	{
		buffer = 0;
		return;
	}
	ReadVirtualMemory(hDriver, GAME_PROCESS_ID, (uint64_t)addresstoread, (uint64_t)buffer, size);
	//SockRequest((uint64_t)addresstoread, (uint64_t)buffer, size);

}
void readmem(uint64_t addresstoread, void* buffer, uint32_t size)
{
	if (addresstoread < 0x10000) //address in non paged area
	{
		buffer = 0;
		return;
	}
		
	ReadVirtualMemory(hDriver, GAME_PROCESS_ID, (uint64_t)addresstoread, (uint64_t)buffer, size);
	//SockRequest((uint64_t)addresstoread, (uint64_t)buffer, size);

}

uint64_t ReadChain(uint64_t base, const std::vector<uint64_t>& offsets) {
	uint64_t result = 0;
	readmem(base + offsets.at(0), &result, sizeof(uint64_t));
	if (!result)
		return 0;
	for (int i = 1; i < offsets.size(); i++) {
		readmem(result + offsets.at(i), &result, sizeof(uint64_t));
		if (!result)
			return 0;
	}
	return result;
}