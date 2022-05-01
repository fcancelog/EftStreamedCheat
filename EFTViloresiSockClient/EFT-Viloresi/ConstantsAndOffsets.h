#pragma once
//#define SERVER_IP "192.168.1.6" // PC GIACOMO
#define SERVER_IP "192.168.1.9" // PC GIORGIO
//#define DEFAULT_PORT "27015" // PORTA GIACOMO
#define DEFAULT_PORT "27016" // PORTA GIORGIO

//#define SERVER_IP "192.168.37.129"

// communication constants
#define PLAYERS_LIST_AND_MATRIX_REQUEST 0x02
#define ITEMS_REQUEST 0x03
#define PLAYERS_MATRIX_PACKET_SIZE 28800 // 28 KB is the size of about 100 player structs 
#define ITEMS_PACKET_SIZE 106000
#define MAX_PLAYERS_SAVED 100
#define TOTAL_SAVED_BONES 22

#define GAME_OBJECT_MANAGER_OFF 0x17F8D28
#define PLAYERCOUNT_OFF	0x18

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

#define TOTAL_SAVED_ITEMIDS 30

wchar_t itemIds[TOTAL_SAVED_ITEMIDS][25] = {
		L"59faff1d86f7746c51718c9c",
		L"5c0530ee86f774697952d952",
		L"5c12613b86f7743bbe2c3f76",
		L"5c052e6986f7746b207bc3c9",
		L"5c1267ee86f77416ec610f72",
		L"5c05308086f7746b2101e90b",
		L"5af0534a86f7743b6f354284",
		L"5c12620d86f7743f8b198b72",
		L"5c05300686f7746dce784e5d",
		L"5c0126f40db834002a125382",
		L"5d1b2f3f86f774252167a52c",
		L"5d0378d486f77420421a5ff4",
		L"5c052f6886f7746b1e3db148",
		L"5d0376a486f7747d8050965c",
		L"5d03775b86f774203e7e0c4b",
		L"5d03794386f77420415576f5",
		L"59e3639286f7741777737013",
		L"5c1d0c5f86f7744bb2683cf0",
		L"5c94bbff86f7747ee735c08f",
		L"5c1d0efb86f7744baf2e7b7b",
		L"5c1e495a86f7743109743dfb",
		L"5c1d0d6d86f7744bb2683e1f",
		L"5c1d0f4986f7744bb01837fa",
		L"5c1d0dc586f7744baf2e7b79",
		L"5780cf7f2459777de4559322",
		L"5d1b36a186f7742523398433",
		L"590c60fc86f77412b13fddcf",
		L"59fb023c86f7746d0d4b423c",
		L"59fb042886f7746c5005a7b2",
		L"590a3efd86f77437d351a25b"
};
string itemRealName[TOTAL_SAVED_ITEMIDS] = {
	"Bitcoin",
	"LEDX",
	"Intelligence folder",
	"Portable Defibrillator",
	"Prokill medallion",
	"Virtex processor",
	"Ophthalmoscope",
	"Tetriz",
	"VPX Flash Storage",
	"R. R. Ice Pick",
	"FP-100 filter absorber",
	"Military power filter",
	"Military COFDM",
	"Military circuit board",
	"Phased array element",
	"military battery",
	"Bronze lion",
	"Terragroup LabKey BLUE",
	"Terragroup LabKey Access",
	"Terragroup LabKey RED",
	"Terragroup LabKey VIOLET",
	"Terragroup LabKey YELLOW",
	"Terragroup LabKey BLACK",
	"Terragroup LabKey GREEN",
	"Dorm Room 314 MARKED KEY",
	"Metal Fuel Tank",
	"Documents Case",
	"Weapon Case",
	"Item Case",
	"Gas Analyzer"
};