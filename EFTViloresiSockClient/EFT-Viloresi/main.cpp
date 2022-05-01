#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <Windows.h>
#include <iostream>
#include <vector>
#include<cmath>
using namespace std;

#include "ConstantsAndOffsets.h"
#include "DirectOverlay.h"
#include "MemOperations.h"
float playersposarray[MAX_PLAYERS_SAVED][TOTAL_SAVED_BONES][3];
wchar_t itemName[2000][25];
float itemCoord[2000][3] = { 0 };
#include "GameVariables.h"
#include "EFTData.h"
EFTData Data;
#include "EFTSockClient.h"
#include "visuals.h"
#include "utils.h"




int width = 0;
int height = 0;
int ScreenCenterX = 0;
int ScreenCenterY = 0;
int sentplayerrequests = 0;
void ReadLoop()
{
	while (1)
	{


		SockGetPlayersAndMatrix();
		sentplayerrequests++;
		if (sentplayerrequests == 100)
		{
			SockGetItems();
			sentplayerrequests = 0;
		}
	}
}
void drawLoop(int width, int height) {

	Render();
}

int main()
{
	InitSockClient();
	SockGetPlayersAndMatrix();
	SockGetItems();
	DirectOverlaySetOption(D2DOV_DRAW_FPS | D2DOV_FONT_IMPACT);

	HWND windowhwnd = FindWindowA("TESTWINDOW", NULL);

	DirectOverlaySetup(drawLoop, windowhwnd);
	RECT rect;
	if (GetWindowRect(windowhwnd, &rect))
	{
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
	}
	CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ReadLoop, 0, 0, nullptr);

	getchar();
	return 1;
}

bool WorldToScreenv2(float* point3D, D2D1_POINT_2F& point2D)
{

	D3DXVECTOR3 _point3D = D3DXVECTOR3(point3D[0], point3D[1], point3D[2]);

	auto& matrix = viewMatrix;



	D3DXVECTOR3 translationVector = D3DXVECTOR3(matrix._41, matrix._42, matrix._43);
	D3DXVECTOR3 up = D3DXVECTOR3(matrix._21, matrix._22, matrix._23);
	D3DXVECTOR3 right = D3DXVECTOR3(matrix._11, matrix._12, matrix._13);

	float w = D3DXVec3Dot(&translationVector, &_point3D) + matrix._44;

	if (w < 0.098f)
		return false;

	float y = D3DXVec3Dot(&up, &_point3D) + matrix._24;
	float x = D3DXVec3Dot(&right, &_point3D) + matrix._14;
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	int horizontal = desktop.right;
	int vertical = desktop.bottom;
	point2D.x = (horizontal / 2) * (1.f + x / w);
	point2D.y = (vertical / 2) * (1.f - y / w);
	ScreenCenterX = (horizontal / 2);
	ScreenCenterY = (vertical / 2);
	//point2D.x = (1920 / 2) * (1.f + x / w);
	//point2D.y = (1080 / 2) * (1.f - y / w);
	return true;
}



void Render()
{

	auto& local_player = Data.localPlayer;

	float distance;
	float distance2;
	float MaxDrawDistance = 300.f;
	D3DCOLOR color = 0;
	D2D1_POINT_2F itemCoord2D;

	for (int i = 0; i < savedplayersnum; i++)
	{

		D2D1_POINT_2F screen_pos, head_pos, neck_pos, Lupperarm_pos, Rupperarm_pos, Lforearm1_pos, Rforearm1_pos, Lforearm2_pos, Rforearm2_pos, Lforearm3_pos, Rforearm3_pos, pelvis_pos, LCalf_pos, RCalf_pos, LFoot_pos, RFoot_pos;
		D2D1_POINT_2F RPalm_pos, LPalm_pos, Spine1_pos, Spine2_pos, Spine3_pos;
		int R = 0, G = 0, B = 0;
		if (playersposarray[i][IS_LOCAL_PLAYER][0] == 1)
			continue;
		if (playersposarray[i][IS_PLAYER][0] == 1)
		{
			R = 256;
			G = 0;
			B = 0;
		}
		else
		{
			R = 0;
			G = 256;
			B = 0;
		}

		WorldToScreenv2(playersposarray[i][headPosN], head_pos);
		WorldToScreenv2(playersposarray[i][neckPosN], neck_pos);
		WorldToScreenv2(playersposarray[i][LupperarmPosN], Lupperarm_pos);
		WorldToScreenv2(playersposarray[i][RupperarmPosN], Rupperarm_pos);
		WorldToScreenv2(playersposarray[i][Lforearm1PosN], Lforearm1_pos);
		WorldToScreenv2(playersposarray[i][Rforearm1PosN], Rforearm1_pos);
		WorldToScreenv2(playersposarray[i][Lforearm2PosN], Lforearm2_pos);
		WorldToScreenv2(playersposarray[i][Rforearm2PosN], Rforearm2_pos);
		WorldToScreenv2(playersposarray[i][Lforearm3PosN], Lforearm3_pos);
		WorldToScreenv2(playersposarray[i][Rforearm3PosN], Rforearm3_pos);
		WorldToScreenv2(playersposarray[i][pelvisPosN], pelvis_pos);
		WorldToScreenv2(playersposarray[i][RCalfN], RCalf_pos);
		WorldToScreenv2(playersposarray[i][LCalfN], LCalf_pos);
		WorldToScreenv2(playersposarray[i][RfootN], RFoot_pos);
		WorldToScreenv2(playersposarray[i][LfootN], LFoot_pos);
		WorldToScreenv2(playersposarray[i][RPalmPosN], RPalm_pos);
		WorldToScreenv2(playersposarray[i][LPalmPosN], LPalm_pos);
		WorldToScreenv2(playersposarray[i][Spine1PosN], Spine1_pos);
		WorldToScreenv2(playersposarray[i][Spine2PosN], Spine2_pos);
		WorldToScreenv2(playersposarray[i][Spine3PosN], Spine3_pos);

		float HeadNeckDistance = sqrtf(pow(neck_pos.y - head_pos.y, 2) + pow(neck_pos.x - head_pos.x, 2));
		DrawCircle((int)head_pos.x, (int)head_pos.y, HeadNeckDistance, 2, R, G, B, 255, false);
		DrawLine((int)neck_pos.x, (int)neck_pos.y, Lupperarm_pos.x, Lupperarm_pos.y, 2, R, G, B);
		DrawLine((int)neck_pos.x, (int)neck_pos.y, Rupperarm_pos.x, Rupperarm_pos.y, 2, R, G, B);
		DrawLine((int)Lupperarm_pos.x, (int)Lupperarm_pos.y, Lforearm1_pos.x, Lforearm1_pos.y, 2, R, G, B);
		DrawLine((int)Lforearm1_pos.x, (int)Lforearm1_pos.y, Lforearm2_pos.x, Lforearm2_pos.y, 2, R, G, B);
		DrawLine((int)Lforearm2_pos.x, (int)Lforearm2_pos.y, Lforearm3_pos.x, Lforearm3_pos.y, 2, R, G, B);
		DrawLine((int)Lforearm3_pos.x, (int)Lforearm3_pos.y, LPalm_pos.x, LPalm_pos.y, 2, R, G, B);
		DrawLine((int)Rupperarm_pos.x, (int)Rupperarm_pos.y, Rforearm1_pos.x, Rforearm1_pos.y, 2, R, G, B);
		DrawLine((int)Rforearm1_pos.x, (int)Rforearm1_pos.y, Rforearm2_pos.x, Rforearm2_pos.y, 2, R, G, B);
		DrawLine((int)Rforearm2_pos.x, (int)Rforearm2_pos.y, Rforearm3_pos.x, Rforearm3_pos.y, 2, R, G, B);
		DrawLine((int)Rforearm3_pos.x, (int)Rforearm3_pos.y, RPalm_pos.x, RPalm_pos.y, 2, R, G, B);
		DrawLine((int)neck_pos.x, (int)neck_pos.y, Spine1_pos.x, Spine1_pos.y, 2, R, G, B);
		DrawLine((int)Spine1_pos.x, (int)Spine1_pos.y, Spine2_pos.x, Spine2_pos.y, 2, R, G, B);
		DrawLine((int)Spine2_pos.x, (int)Spine2_pos.y, Spine3_pos.x, Spine3_pos.y, 2, R, G, B);
		DrawLine((int)Spine3_pos.x, (int)Spine3_pos.y, pelvis_pos.x, pelvis_pos.y, 2, R, G, B);
		DrawLine((int)pelvis_pos.x, (int)pelvis_pos.y, RCalf_pos.x, RCalf_pos.y, 2, R, G, B);
		DrawLine((int)pelvis_pos.x, (int)pelvis_pos.y, LCalf_pos.x, LCalf_pos.y, 2, R, G, B);
		DrawLine((int)LCalf_pos.x, (int)LCalf_pos.y, LFoot_pos.x, LFoot_pos.y, 2, R, G, B);
		DrawLine((int)RCalf_pos.x, (int)RCalf_pos.y, RFoot_pos.x, RFoot_pos.y, 2, R, G, B);
	}
	for (int k = 0; k < saveditemsnum; k++)
	{
		for (int w = 0; w < TOTAL_SAVED_ITEMIDS; w++)
		{
			for (int j = 0; itemName[k][j] == itemIds[w][j] && j < 25; j++)
			{
				if (j == 24)
				{
					WorldToScreenv2(itemCoord[k], itemCoord2D);
					DrawString(itemRealName[w], 10.f, itemCoord2D.x, itemCoord2D.y, 0, 255, 0, 255);
				}
			}
		}
	}
}



