#pragma once
#include "GameVariables.h"
#include <xmmintrin.h>  
#include <emmintrin.h>
#include <d3dx9.h>
using namespace std;

namespace EFTStructs
{
	struct BaseObject
	{
		uint64_t previousObjectLink; //0x0000
		uint64_t nextObjectLink; //0x0008
		uint64_t object; //0x0010
	};

	struct GameObjectManager {
		uint64_t lastTaggedNode; // 0x0
		uint64_t taggedNodes; // 0x8
		uint64_t lastMainCameraTaggedNode; // 0x10
		uint64_t mainCameraTaggedNodes; // 0x18
		uint64_t lastActiveNode; // 0x20
		uint64_t activeNodes; // 0x28
	};
	class ListInternal
	{
	public:
		char pad_0x0000[0x20]; //0x0000
		uintptr_t* firstEntry; //0x0020 
	}; //Size=0x0028

	class List
	{
	public:
		char pad_0x0000[0x10]; //0x0000
		ListInternal* listBase; //0x0010 
		__int32 itemCount; //0x0018 
	}; //Size=0x001C
}
class EFTData
{
private:
	uint64_t matrix_list_base = 0;
	uint64_t dependency_index_table_base = 0;
public:
	EFT_Offsets offsets;
	uint64_t Unity_Player_DllBase = 0;
	int          playercount;
	EFTPlayer localPlayer;
	std::vector<EFTPlayer> players;
	D3DXMATRIX viewMatrix;

	uint64_t getbone_matrix(uint64_t instance)
	{
		static std::vector<uint64_t> temp{ offsets.Player.playerBody, 0x28, 0x28, 0x10 };

		return ReadChain(instance, temp);
	}
	uint64_t GetObjectFromList(uint64_t listPtr, uint64_t lastObjectPtr, const char* objectName)
	{
		using EFTStructs::BaseObject;
		char name[256];
		uint64_t classNamePtr = 0x0;
		uint32_t BaseObjectSize = sizeof(uint64_t) * 3;
		BaseObject activeObject, lastObject;
		readmem(listPtr, &activeObject, BaseObjectSize);
		readmem(lastObjectPtr, &lastObject, BaseObjectSize);

		if (activeObject.object != 0x0)
		{
			readmem(activeObject.nextObjectLink, &activeObject, BaseObjectSize);
			while (activeObject.object != 0 && activeObject.object != lastObject.object)
			{

				readmem(activeObject.object + 0x60, &(classNamePtr), sizeof(uint64_t));
				readmem(classNamePtr + 0x0, &name, sizeof(name));

				if (strcmp(name, objectName) == 0)
				{
					return activeObject.object;
				}
				readmem(activeObject.nextObjectLink, &activeObject, BaseObjectSize);
			}
		}
		if (lastObject.object != 0x0)
		{
			readmem(lastObject.object + 0x60, &classNamePtr, sizeof(uint64_t));
			readmem(classNamePtr + 0x0, &name, 256);

			if (strcmp(name, objectName) == 0)
			{
				return lastObject.object;
			}
		}

		return uint64_t();
	}
	FVector GetPosition(uint64_t transform)
	{
		uint64_t transform_internal = 0, matrices = 0;
		int index = 0;

		readmem(transform + 0x10, &transform_internal, sizeof(uint64_t));
		readmem(transform_internal + 0x38, &matrices, sizeof(uint64_t));
		readmem(transform_internal + 0x40, &index, sizeof(int));

		readmem(matrices + 0x18, &matrix_list_base, sizeof(matrix_list_base));

		readmem((matrices + 0x20), &dependency_index_table_base, sizeof(dependency_index_table_base));

		static auto get_dependency_index = [this](uint64_t base, int32_t index)
		{
			readmem((base + index * 4), &index, sizeof(index));
			return index;
		};

		static auto get_matrix_blob = [this](uint64_t base, uint64_t offs, float* blob, uint32_t size) {
			readmem((base + offs), blob, size);
		};

		int32_t index_relation = get_dependency_index(dependency_index_table_base, index);

		FVector ret_value;
		{
			float* base_matrix3x4 = (float*)malloc(64),
				* matrix3x4_buffer0 = (float*)((uint64_t)base_matrix3x4 + 16),
				* matrix3x4_buffer1 = (float*)((uint64_t)base_matrix3x4 + 32),
				* matrix3x4_buffer2 = (float*)((uint64_t)base_matrix3x4 + 48);

			get_matrix_blob(matrix_list_base, index * 48, base_matrix3x4, 16);

			__m128 xmmword_1410D1340 = { -2.f, 2.f, -2.f, 0.f };
			__m128 xmmword_1410D1350 = { 2.f, -2.f, -2.f, 0.f };
			__m128 xmmword_1410D1360 = { -2.f, -2.f, 2.f, 0.f };

			while (index_relation >= 0)
			{
				uint32_t matrix_relation_index = 6 * index_relation;

				// paziuret kur tik 3 nureadina, ten translationas, kur 4 = quatas ir yra rotationas.
				get_matrix_blob(matrix_list_base, 8 * matrix_relation_index, matrix3x4_buffer2, 16);
				__m128 v_0 = *(__m128*)matrix3x4_buffer2;

				get_matrix_blob(matrix_list_base, 8 * matrix_relation_index + 32, matrix3x4_buffer0, 16);
				__m128 v_1 = *(__m128*)matrix3x4_buffer0;

				get_matrix_blob(matrix_list_base, 8 * matrix_relation_index + 16, matrix3x4_buffer1, 16);
				__m128i v9 = *(__m128i*)matrix3x4_buffer1;

				__m128* v3 = (__m128*)base_matrix3x4; // r10@1
				__m128 v10; // xmm9@2
				__m128 v11; // xmm3@2
				__m128 v12; // xmm8@2
				__m128 v13; // xmm4@2
				__m128 v14; // xmm2@2
				__m128 v15; // xmm5@2
				__m128 v16; // xmm6@2
				__m128 v17; // xmm7@2

				v10 = _mm_mul_ps(v_1, *v3);
				v11 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, 0));
				v12 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, 85));
				v13 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, -114));
				v14 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, -37));
				v15 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, -86));
				v16 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, 113));

				v17 = _mm_add_ps(
					_mm_add_ps(
						_mm_add_ps(
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(v11, (__m128)xmmword_1410D1350), v13),
									_mm_mul_ps(_mm_mul_ps(v12, (__m128)xmmword_1410D1360), v14)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), -86))),
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(v15, (__m128)xmmword_1410D1360), v14),
									_mm_mul_ps(_mm_mul_ps(v11, (__m128)xmmword_1410D1340), v16)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 85)))),
						_mm_add_ps(
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(v12, (__m128)xmmword_1410D1340), v16),
									_mm_mul_ps(_mm_mul_ps(v15, (__m128)xmmword_1410D1350), v13)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 0))),
							v10)),
					v_0);

				*v3 = v17;

				index_relation = get_dependency_index(dependency_index_table_base, index_relation);
			}

			ret_value = *(FVector*)base_matrix3x4;
			delete[] base_matrix3x4;
		}

		return ret_value;
	}
	bool InitOffsets()
	{

		//SockGetUnityPlayerRequest((uint64_t*)&Unity_Player_DllBase);
		//GetUnityPlayer(hDriver, GAME_PROCESS_ID, (uint64_t)&Unity_Player_DllBase);
		if (!Unity_Player_DllBase)
			return false;
		cout << "reading gameobject manager: ";
		cout << hex << Unity_Player_DllBase + GAME_OBJECT_MANAGER_OFF << endl;
		readmem(Unity_Player_DllBase + GAME_OBJECT_MANAGER_OFF, &(offsets.gameObjectManager), sizeof(DWORD64));
		
		if (!offsets.gameObjectManager)
		{ 
			cout << "objectmanager fail";
			return false;
		}
		uint64_t LastActiveNode = 0, ActiveNodes = 0;
		uint64_t LastMainCameraTaggedNode = 0, MainCameraTaggedNodes = 0;
		uint64_t LastTaggedNode = 0, TaggedNodes = 0;

		readmem(offsets.gameObjectManager + offsetof(EFTStructs::GameObjectManager, lastActiveNode), &LastActiveNode, sizeof(uint64_t));
		readmem(offsets.gameObjectManager + offsetof(EFTStructs::GameObjectManager, activeNodes), &ActiveNodes, sizeof(uint64_t));

		readmem(offsets.gameObjectManager + offsetof(EFTStructs::GameObjectManager, lastMainCameraTaggedNode), &LastMainCameraTaggedNode, sizeof(uint64_t));
		readmem(offsets.gameObjectManager + offsetof(EFTStructs::GameObjectManager, mainCameraTaggedNodes), &MainCameraTaggedNodes, sizeof(uint64_t));

		//readmem(offsets.gameObjectManager + offsetof(EFTStructs::GameObjectManager, lastTaggedNode), &LastTaggedNode, sizeof(uint64_t));
		//readmem(offsets.gameObjectManager + offsetof(EFTStructs::GameObjectManager, taggedNodes), &TaggedNodes, sizeof(uint64_t));

		if (!LastActiveNode || !ActiveNodes || !LastMainCameraTaggedNode || !MainCameraTaggedNodes)
			return false;
		if (!(offsets.gameWorld = GetObjectFromList(ActiveNodes, LastActiveNode, "GameWorld")))
			return false;
		if (!(offsets.fpsCamera = GetObjectFromList(MainCameraTaggedNodes, LastMainCameraTaggedNode, "FPS Camera")))
			return false;
		if (!(offsets.localGameWorld = ReadChain(offsets.gameWorld, { 0x30, 0x18, 0x28 })))
			return false;

		//const auto cameracom = ReadChain(offsets.fpsCamera, { 0x30, 0x18 });

		//G::MATRIX = Mem::read<geo::mat4x4_t>(cameracom + 0xDC);

		return true;
	}
	bool ReadLoop()
	{
		players.clear();
		if (!(offsets.localGameWorld = ReadChain(offsets.gameWorld, { 0x30, 0x18, 0x28 })))
			return false;
		uint64_t onlineusers = 0, list_base = 0;
		int player_count = 0;
		readmem(offsets.localGameWorld + 0x80, &onlineusers, sizeof(uint64_t));
		if (!onlineusers)
			return false;
		readmem(onlineusers + offsetof(EFTStructs::List, listBase), &list_base, sizeof(uint64_t));
		readmem(onlineusers + 0x18, &player_count, sizeof(int)); //  + 0x18
		if (player_count <= 0 || !list_base)
			return false;

		constexpr auto BUFFER_SIZE = 128;

		uint64_t player_buffer[BUFFER_SIZE];
		readmem(list_base + offsetof(EFTStructs::ListInternal, firstEntry), player_buffer, sizeof(uint64_t) * player_count);
		if (!player_buffer)
			return false;
		EFTPlayer player;

		for (int i = 0; i < player_count; i++)
		{
			player.instance = player_buffer[i];
			playercount = player_count;


			uint64_t bone_matrix = getbone_matrix(player.instance);

			if (bone_matrix)
			{
				uint64_t bone = ReadChain(bone_matrix, { 0x20, 0x10, 0x38 });
				if (!bone)
					return false;
				readmem(bone + 0x90, &(player.location), sizeof(FVector));
				uint64_t boneheadpos = 0, boneneckpos = 0, boneRupperarmpos = 0, boneLupperarmpos = 0, boneRforearm1pos = 0, boneLforearm1pos = 0, bonepelvispos = 0, boneLCalfpos = 0, boneRCalfpos = 0, boneLFootpos = 0, boneRFootpos = 0, boneLforearm2pos = 0, boneRforearm2pos = 0, boneLforearm3pos = 0, boneRforearm3pos = 0;
				uint64_t boneRPalmpos = 0, boneLPalmpos = 0, boneSpine1pos = 0, boneSpine2pos = 0, boneSpine3pos = 0;
				readmem(bone_matrix + 0x20 + (int)Bones::HumanHead * 8, &boneheadpos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanNeck * 8, &boneneckpos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanRUpperarm * 8, &boneRupperarmpos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanLUpperarm * 8, &boneLupperarmpos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanRForearm1 * 8, &boneRforearm1pos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanLForearm1 * 8, &boneLforearm1pos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanLForearm2 * 8, &boneLforearm2pos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanRForearm2 * 8, &boneRforearm2pos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanLForearm3 * 8, &boneLforearm3pos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanRForearm3 * 8, &boneRforearm3pos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanRPalm * 8, &boneRPalmpos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanLPalm * 8, &boneLPalmpos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanSpine1 * 8, &boneSpine1pos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanSpine2 * 8, &boneSpine2pos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanSpine3 * 8, &boneSpine3pos, sizeof(uint64_t));
				
				readmem(bone_matrix + 0x20 + (int)Bones::HumanPelvis * 8, &bonepelvispos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanLCalf * 8, &boneLCalfpos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanRCalf * 8, &boneRCalfpos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanLFoot * 8, &boneLFootpos, sizeof(uint64_t));
				readmem(bone_matrix + 0x20 + (int)Bones::HumanRFoot * 8, &boneRFootpos, sizeof(uint64_t)); 
				

				player.headPos = GetPosition(boneheadpos);
				player.neckPos = GetPosition(boneneckpos);
				player.RupperarmPos = GetPosition(boneRupperarmpos);
				player.LupperarmPos = GetPosition(boneLupperarmpos);
				player.Rforearm1Pos = GetPosition(boneRforearm1pos);
				player.Lforearm1Pos = GetPosition(boneLforearm1pos);
				player.Rforearm2Pos = GetPosition(boneRforearm2pos);
				player.Lforearm2Pos = GetPosition(boneLforearm2pos);
				player.Rforearm3Pos = GetPosition(boneRforearm3pos);
				player.Lforearm3Pos = GetPosition(boneLforearm3pos);
				player.pelvisPos = GetPosition(bonepelvispos);
				player.LCalf = GetPosition(boneLCalfpos);
				player.RCalf = GetPosition(boneRCalfpos);
				player.Lfoot = GetPosition(boneLFootpos);
				player.Rfoot = GetPosition(boneRFootpos);
				player.RPalmPos = GetPosition(boneRPalmpos);
				player.LPalmPos = GetPosition(boneLPalmpos);
				player.Spine1Pos = GetPosition(boneSpine1pos);
				player.Spine2Pos = GetPosition(boneSpine2pos);
				player.Spine3Pos = GetPosition(boneSpine3pos);
			}
			else 
				return false;
			 
			int boolplayer = 0;
			
			uint64_t PlayerInfo = ReadChain(player.instance, { 0x4B8, 0x28 });
			if (PlayerInfo)
			{
				readmem(PlayerInfo + 0x5C, &player.RegistrationDate, sizeof(uint32_t));
				//player.WildSpawnType = ReadChain(PlayerInfo, { 0x38,0x10 });
			}
			else
				return false;
			readmem(player.instance + 0x18, &boolplayer, sizeof(int));
			if (boolplayer)
			{
				localPlayer = player;
				localPlayer.location = player.location;
			}

			players.emplace_back(player);

		}
		//VieMatrix
		uint64_t temp = offsets.fpsCamera;
		readmem(temp + 0x30, &temp, sizeof(uint64_t));
		if (!temp)
			return false;
		readmem(temp + 0x18, &temp, sizeof(uint64_t));
		if (!temp)
			return false;

		D3DXMATRIX temp_matrix;
		readmem(temp + 0xDC, &temp_matrix, sizeof(temp_matrix));
		D3DXMatrixTranspose(&viewMatrix, &temp_matrix);
		return true;
	}

};
