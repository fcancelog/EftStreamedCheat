#pragma once
#include "ConstantsAndOffsets.h"
#include "utils.h"

void readmem(void* addresstoread, void* buffer, uint32_t size)
{
	if ((uint64_t)addresstoread < 0x10000) //address in non paged area
	{
		buffer = 0;
		return;
	}
	//if (!SockRequest((uint64_t)addresstoread, (uint64_t*)buffer, size))
	//	cerr << "error sockrequestfailed\n";
	//ReadVirtualMemory(hDriver, GAME_PROCESS_ID, (uint64_t)addresstoread, (uint64_t)buffer, size);
}
void readmem(uint64_t addresstoread, void* buffer, uint32_t size)
{
	if (addresstoread < 0x10000) //address in non paged area
	{
		buffer = 0;
		return;
	}
	//if(!SockRequest((uint64_t)addresstoread, (uint64_t*)buffer, size))
	//cerr << "error sockrequestfailed\n";
	//ReadVirtualMemory(hDriver, GAME_PROCESS_ID, (uint64_t)addresstoread, (uint64_t)buffer, size);
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