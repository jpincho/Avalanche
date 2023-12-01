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
	if (CShape::Shapes.empty())
		return;
	for (int i = 0; i < num; i++)
	{
		CShape::Shapes.pop_back();
	}
}

void CApplication::SpawnTriangle(float x, float y, float size)
{
	CShape::Shapes.emplace_back(CShape(x, y, 0, size));
}

void CApplication::SpawnRectangle(float x, float y, float size)
{
	CShape::Shapes.emplace_back(CShape(x, y, 1, size));
}

void CApplication::SpawnHexagon(float x, float y, float radius)
{
	CShape::Shapes.emplace_back(CShape(x, y, 2, radius));
}

void CApplication::SpawnOctagon(float x, float y, float radius)
{
	CShape::Shapes.emplace_back(CShape(x, y, 3, radius));
}

void CApplication::Resize(float scale)
{

}

int CApplication::Update(float dt, STriangle *tri)
{
	static std::vector <uint16_t> AllIndices;
	if (AllIndices.size() != CShape::Shapes.size())
	{
		AllIndices.clear();
		AllIndices.resize(CShape::Shapes.size());
		for (unsigned Index = 0; Index < CShape::Shapes.size(); ++Index)
		{
			AllIndices[Index] = Index;
		}
	}
	int tri_count = 0;
	for (unsigned ShapeIndex = 0; ShapeIndex < CShape::Shapes.size(); ++ShapeIndex)
	{
		CShape::Shapes[ShapeIndex].Update(dt);
		tri_count += CShape::Shapes[ShapeIndex].Draw(&tri[tri_count]);
	}

	for (unsigned ShapeIndex = 0; ShapeIndex < CShape::Shapes.size(); ++ShapeIndex)
	{
		CShape::Shapes[ShapeIndex].CheckCollisions(AllIndices);
	}
	return tri_count;
}
