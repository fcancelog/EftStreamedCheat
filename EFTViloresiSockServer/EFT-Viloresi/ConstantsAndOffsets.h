#pragma once
#define DEFAULT_PORT "27016" // server GIUACOM
//#define DEFAULT_PORT "27015" // server GIORGIO

#define GAME_PROCESS_NAME L"EscapeFromTarkov.exe"
#define DRIVER_FILENAMEW L"\\\\.\\Fafaki"
#define IO_READorWRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0341, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_GET_UNITYPLAYER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0342, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

//#define DRIVER_FILENAMEW L"\\\\.\\Kikiki"
//#define IO_READorWRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0301, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
//#define IO_GET_UNITYPLAYER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0302, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// communication constants
#define PLAYERS_LIST_REQUEST 0x02
#define ITEMS_REQUEST 0x03
#define PLAYERS_MATRIX_PACKET_SIZE 28800 // 28 KB is the size of about 100 player structs 
#define ITEMS_PACKET_SIZE 106000
//others
#define PLAYER_SAVED_BONES_COUNT 23 // I've saved 23 different bones positions for each player
#define MAX_PLAYERS_SAVED 100
#define TOTAL_SAVED_BONES 22
#define headPosN 0
#define neckPosN 1
#define RupperarmPosN 2
#define LupperarmPosN 3
#define Rforearm1PosN 4
#define Lforearm1PosN 5
#define Rforearm2PosN 6
#define Lforearm2PosN 7
#define Rforearm3PosN 8
#define Lforearm3PosN 9
#define pelvisPosN 10
#define LCalfN 11
#define RCalfN 12
#define LfootN 13
#define RfootN 14
#define RPalmPosN 15
#define LPalmPosN 16
#define Spine1PosN 17
#define Spine2PosN 18
#define Spine3PosN 19
#define IS_LOCAL_PLAYER 20
#define IS_PLAYER 21

//game offsets

#define GAME_OBJECT_MANAGER_OFF 0x17F8D28
#define PLAYERCOUNT_OFF	0x18