#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <Dwmapi.h> 
#include <list>

#include "math.hpp"


	void Render();
	bool WorldToScreenv2(const FVector& point3D, D2D1_POINT_2F& point2D);

