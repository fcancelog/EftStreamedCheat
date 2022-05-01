#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <vector>

using namespace std;
#include "ConstantsAndOffsets.h"

#include "GameVariables.h"
#include "math.hpp"
#include "MemOperations.h"
#include "EFTSockServer.h"
#include "EFTData.h"

#include "utils.h"

EFTData Data;



int SockReadAndReply()
{
	char sendbuffer[DEFAULT_BUFLEN];
	char receivebuffer[DEFAULT_BUFLEN];
	uint64_t addresstoread = 0;
	uint32_t size = 0;
	
	iResult = recv(ClientSocket, receivebuffer, DEFAULT_BUFLEN, 0);
	
	if (iResult == 0x1) // request packet because the size of the packet is 0x1
	{

		if (receivebuffer[0] == PLAYERS_LIST_REQUEST)
		{
			
			if (!Data.ReadLoop())
				cout << "readloop error\n";
			else
				cout << "readloop ok\n";
		}
		if (receivebuffer[0] == ITEMS_REQUEST)
		{

			Data.ReadItemsLoop();
		}
		return 1;
	}
	else if (iResult == 0)
	{
		cerr << "connection with the client lost! \n";
	}
	else if (iResult != (0x1 + sizeof(uint64_t) + sizeof(uint32_t)))
	{
		cerr << "size of the received packet is different from 13 bytes \n";
		return 0;
	}
	if (iSendResult == SOCKET_ERROR)
		return 0;

	return 1;
}

int main()
{
	cout << "initializing and waiting for the client\n";
	if (int returnvalue = Init()) //driver handle and process Ids
	{
		if (returnvalue == DRIVER_NOT_LOADED)
			cout << "Driver not loaded\n";
		if (returnvalue == PROCESS_NOT_FOUND)
			cout << "Process not found\n";
		system("pause");
		return 0;
	}

	if (!Data.InitOffsets())
		cout << "offsets initialization error\n";
	InitSock();
	while (1)
	{
		SockReadAndReply();
	}
	getchar();
	return 1;
}