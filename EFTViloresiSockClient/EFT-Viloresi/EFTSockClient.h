#pragma once
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512


WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
struct addrinfo* result = NULL,
    * ptr = NULL,
    hints;
const char* sendbuf = "this is a test";
char recvbuf[DEFAULT_BUFLEN];
int iResult;
int recvbuflen = DEFAULT_BUFLEN;

D3DXMATRIX temp_matrix, viewMatrix;

uint32_t savedplayersnum,saveditemsnum;

int InitSockClient()
{
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 0;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 0;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 0;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 0;
    }


    return 1;
}
int SockRequest(uint64_t addresstoread, uint64_t* addresstowrite, uint32_t size)
{
	char sendbuffer[DEFAULT_BUFLEN];
	char receivebuffer[DEFAULT_BUFLEN];
    sendbuffer[0] = 0x1; // 0x1 is the readrequest
	memcpy(&(sendbuffer[1]), &addresstoread, sizeof(uint64_t)); //copy address at the start of the buffer + 0x1 (the first byte indicates the operation to do)
	memcpy(&(sendbuffer[sizeof(uint64_t) + 0x1]), &size, sizeof(uint32_t)); // copy the size to read after the address
    iResult = send(ConnectSocket, sendbuffer, 0x1 + sizeof(uint64_t) + sizeof(uint32_t), 0);

	if (iResult == SOCKET_ERROR)
		return 0;

	iResult = recv(ConnectSocket, receivebuffer, DEFAULT_BUFLEN, 0);
	if (iResult != size)
	{
		cerr << "error: received size does not match! \n";
		return 0;
	}

	if (iResult > 0)
	{
		memcpy(addresstowrite, receivebuffer, size);
	}
	else if (iResult == 0)
		return 0;//connection closed
	else
		return 0; //whatever error

	return 1;
}
void ReassemblePlayersPacket(char* receivedpacket)
{
    memcpy(&savedplayersnum, receivedpacket, sizeof(int));
    int counter = 1; // gonna skip the first byte because it's the savedplayersnum
    for (int i = 0; i < savedplayersnum; i++)
    {
        for (int k = 0; k < TOTAL_SAVED_BONES; k++)
        {
            for (int j = 0; j < 3; j++,counter++)
            {
                memcpy(playersposarray[i][k] + j * 4, receivedpacket + counter * sizeof(float) , sizeof(float));
            }
        }
    }

    memcpy(&temp_matrix, receivedpacket + counter * sizeof(float), sizeof(D3DXMATRIX));
    D3DXMatrixTranspose(&viewMatrix, &temp_matrix);
}
void ReassembleItemsPacket(char* receivedpacket)
{
    memcpy(&saveditemsnum, receivedpacket, sizeof(int));
    int counter = 1, namesizecounter = 0; // gonna skip the first byte because it's the savedplayersnum
    for (int i = 0; i < saveditemsnum; i++)
    {
            for (int j = 0; j < 3; j++, counter++)
            {
                memcpy(itemCoord[i] + j * 4, receivedpacket + counter * sizeof(float) + namesizecounter * 50, sizeof(float));
            }
            memcpy(itemName[i] , receivedpacket + counter * sizeof(float) + namesizecounter * 50, 50); // 50 bytes is the ItemNameSize
            namesizecounter++;
    }
}
int SockGetPlayersAndMatrix()
{
    char sendbuffer[DEFAULT_BUFLEN];
    char receivebuffer[PLAYERS_MATRIX_PACKET_SIZE];
    sendbuffer[0] = PLAYERS_LIST_AND_MATRIX_REQUEST; // 0x2 is PlayersAndMatrix
    iResult = send(ConnectSocket, sendbuffer, 0x1, 0);

    if (iResult == SOCKET_ERROR)
        return 0;
    iResult = recv(ConnectSocket, receivebuffer, PLAYERS_MATRIX_PACKET_SIZE, 0);
    if (iResult > 0)
    {
        ReassemblePlayersPacket(receivebuffer);
    }
    else if (iResult == 0)
        return 0;//connection closed
    else
        return 0; //whatever error

    return 1;
}
int SockGetItems()
{
    char sendbuffer[DEFAULT_BUFLEN];
    char receivebuffer[ITEMS_PACKET_SIZE];
    sendbuffer[0] = ITEMS_REQUEST; // 0x3 is Items
    iResult = send(ConnectSocket, sendbuffer, 0x1, 0);

    if (iResult == SOCKET_ERROR)
        return 0;
    iResult = recv(ConnectSocket, receivebuffer, ITEMS_PACKET_SIZE, 0);
    if (iResult > 0)
    {
        ReassembleItemsPacket(receivebuffer);
    }
    else if (iResult == 0)
        return 0;//connection closed
    else
        return 0; //whatever error

    return 1;
}