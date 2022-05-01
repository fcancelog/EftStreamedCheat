#pragma once
#include "math.hpp"
struct EFT_Offsets
{
	uint64_t gameObjectManager = 0x0;
	uint64_t gameWorld = 0x0;
	uint64_t localGameWorld = 0x0;
	uint64_t fpsCamera = 0x0;
	uint64_t OpticCamera = 0x0;
	struct
	{
		static constexpr uint64_t ExfiltrationController = 0x18;//private \uE334 \uE000;
		static constexpr uint64_t LootList = 0x60;//public readonly List<\uE1B5> LootList = new List<\uE1B5>(1000);
		static constexpr uint64_t RegisteredPlayers = 0x80;//public readonly List<Player> RegisteredPlayers = new List<Player>(40);
	} localGameWorld_offsets;

	struct
	{
		static constexpr uint64_t length = 0x10; //good
		static constexpr uint64_t stringBase = 0x14; //good
	} unicodeString;

	struct
	{
		static constexpr uint64_t information = 0x28; //updated
		static constexpr uint64_t id = 0x10; //updated
	} profile;

	struct
	{
		static constexpr uint64_t playerName = 0x10; //updated
	} information;

	struct
	{
		static constexpr uint64_t angles_0 = 0x1D0;
		static constexpr uint64_t angles_1 = 0x1D8;
		static constexpr uint64_t position = 0x1E0;
	} movementContext;

	struct
	{
		static constexpr uint64_t m_pPlayerProfile = 0x03A0;
		static constexpr uint64_t movementContext = 0x38;
		static constexpr uint64_t proceduralWeaponAnimation = 0x70; //updayed 1/11/2020
		static constexpr uint64_t playerBody = 0xa8;
		static constexpr uint64_t m_pHealthController = 0x3D0; //updated 1/11/2020
		static constexpr uint64_t profile = 0x3A0; //updayed 1/11/2020

	} Player;
};

struct EFTPlayer
{
	uintptr_t	 instance;
	FVector		 headPos;
	FVector		 neckPos;
	FVector		 LupperarmPos;
	FVector		 RupperarmPos;
	FVector		 Lforearm1Pos;
	FVector		 Rforearm1Pos;
	FVector		 Lforearm2Pos;
	FVector		 Rforearm2Pos;
	FVector		 Lforearm3Pos;
	FVector		 Rforearm3Pos;
	FVector		 pelvisPos;
	FVector		 RThigh1Pos;
	FVector		 LThigh1Pos;
	FVector		RPalmPos;
	FVector		LPalmPos;
	FVector		Spine1Pos;
	FVector Spine2Pos;
	FVector Spine3Pos;
	FVector		LCalf;
	FVector		RCalf;
	FVector		Lfoot;
	FVector		Rfoot;
	FVector		 location;
	uint32_t		RegistrationDate;
	//	int		WildSpawnType;
};

struct EFTExtract
{
	uintptr_t	 instance;
	std::string  name;
	FVector		 location;
};
enum Bones : int
{
	HumanBase = 0,
	HumanPelvis = 14,
	HumanLThigh1 = 15,
	HumanLThigh2 = 16,
	HumanLCalf = 17,
	HumanLFoot = 18,
	HumanLToe = 19,
	HumanRThigh1 = 20,
	HumanRThigh2 = 21,
	HumanRCalf = 22,
	HumanRFoot = 23,
	HumanRToe = 24,
	HumanSpine1 = 29,
	HumanSpine2 = 36,
	HumanSpine3 = 37,
	HumanLCollarbone = 89,
	HumanLUpperarm = 90,
	HumanLForearm1 = 91,
	HumanLForearm2 = 92,
	HumanLForearm3 = 93,
	HumanLPalm = 94,
	HumanRCollarbone = 110,
	HumanRUpperarm = 111,
	HumanRForearm1 = 112,
	HumanRForearm2 = 113,
	HumanRForearm3 = 114,
	HumanRPalm = 115,
	HumanNeck = 132,
	HumanHead = 133
};
