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
}

CApplication::~CApplication(void)
{
	DestroyShapes();
}

void CApplication::DestroyShapes(int num)
{
	if (CShape::ShapeArrayLength==0)
		return;
	for (int i = 0; i < num; i++)
	{
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
	CShape::Shapes[CShape::ShapeArrayLength].Create(x, y, type, size);
	++CShape::ShapeArrayLength;
}

void CApplication::Resize(float scale)
{

}

int CApplication::Update(float dt, STriangle *tri)
{
	static std::vector <uint16_t> AllIndices;
	if (AllIndices.size() != CShape::ShapeArrayLength)
	{
		AllIndices.clear();
		AllIndices.resize(CShape::ShapeArrayLength);
		for (unsigned Index = 0; Index < CShape::ShapeArrayLength; ++Index)
		{
			AllIndices[Index] = Index;
		}
	}
	int tri_count = 0;
	for (unsigned ShapeIndex = 0; ShapeIndex < CShape::ShapeArrayLength; ++ShapeIndex)
	{
		CShape::Shapes[ShapeIndex].Update(dt);
		tri_count += CShape::Shapes[ShapeIndex].Draw(&tri[tri_count]);
	}

	for (unsigned ShapeIndex = 0; ShapeIndex < CShape::ShapeArrayLength; ++ShapeIndex)
	{
		CShape::Shapes[ShapeIndex].CheckCollisions(AllIndices);
	}
	return tri_count;
}
