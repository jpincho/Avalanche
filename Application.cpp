////////////////////////////////////////////////////////////////
//
// Confidential, must not be distributed. 
// Copyright (C) 2010 Avalanche Studios. All rights reserved.
// May be altered only for the purpose of this test.
//
////////////////////////////////////////////////////////////////
#include "Application.h"
#include "Shape.h"


////////////////////////////////////////////
//
// Application factory - add your implementation creation here as new switch cases. 
// Up to eight implementations can be created and will be mapped to the function keys f1-f8 for run-time switching and comparison.
//
////////////////////////////////////////////

IApplication *Factory(int index)
{
	switch (index)
	{
	case 1:
		return new CApplication();
		/*
	case 2 to 8:
		// Here you can add additional implementations for convenient comparisons
	*/
	}

	return 0;
}

////////////////////////////////////////////
//
// CApplication
//
////////////////////////////////////////////

CApplication::CApplication(void)
{
	UpdateMask = InitialUpdateMask = 0xFFFF;
	UpdateMaskShift = 0;
	CheckNeighbourCells = true;
}

CApplication::~CApplication(void)
{
	DestroyShapes();
}

void CApplication::DestroyShapes(int num)
{
	if (num >= CShape::ShapeArrayLength)
	{
		CShape::ShapeArrayLength = 0;
		return;
	}
	for (int i = 0; i < num; i++)
	{
		CShape::Shapes[CShape::ShapeArrayLength-1].Destroy();
		--CShape::ShapeArrayLength;
	}
}

void CApplication::SpawnTriangle(float x, float y, float size)
{
	SpawnNewShape(x, y, size, 0);
}

void CApplication::SpawnRectangle(float x, float y, float size)
{
	SpawnNewShape(x, y, size, 1);
}

void CApplication::SpawnHexagon(float x, float y, float radius)
{
	SpawnNewShape(x, y, radius, 2);
}

void CApplication::SpawnOctagon(float x, float y, float radius)
{
	SpawnNewShape(x, y, radius, 3);
}

void CApplication::SpawnNewShape(const float x, const float y, const float size, const uint8_t type)
{
	if (CShape::ShapeArrayLength == 32700)
		return;
	CShape::Shapes[CShape::ShapeArrayLength].Create(x, y, CShape::ShapeArrayLength, type, size);
	++CShape::ShapeArrayLength;
	if (CShape::ShapeArrayLength > 16000)
	{
		UpdateMask = InitialUpdateMask = 0x03;
		UpdateMaskShift = 2;
	}
	else if (CShape::ShapeArrayLength > 8000)
	{
		UpdateMask = InitialUpdateMask = 0x0F;
		UpdateMaskShift = 4;
	}
	else if (CShape::ShapeArrayLength > 4000)
	{
		UpdateMask = InitialUpdateMask = 0xFF;
		UpdateMaskShift = 8;
	}
	else
	{
		UpdateMask = InitialUpdateMask = 0xFFFF;
		UpdateMaskShift = 0;
		CheckNeighbourCells = true;
	}
}

void CApplication::Resize(float scale)
{

}

int CApplication::Update(float dt, STriangle *tri)
{
	UpdateMask <<= UpdateMaskShift;
	if (UpdateMask == 0)
		UpdateMask = InitialUpdateMask;
	int tri_count = 0;
	bool UpdateCollision;
	for (unsigned ShapeIndex = 0; ShapeIndex < CShape::ShapeArrayLength; ++ShapeIndex)
	{
		UpdateCollision = ((ShapeIndex +1)& UpdateMask) != 0;
		CShape::Shapes[ShapeIndex].Update(dt, UpdateCollision, CheckNeighbourCells );
		tri_count += CShape::Shapes[ShapeIndex].Draw(&tri[tri_count]);
	}
	return tri_count;
}
