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

IApplication* Factory(int index)
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
	for (int i = 0; i < num; i++)
	{
		if (CShape::Shapes.empty())
			return;
		CShape::Shapes.pop_back();
	}
}

void CApplication::SpawnTriangle(float x, float y, float size)
{
	CShape::Shapes.push_back(new CTriangle(x, y, size));
}

void CApplication::SpawnRectangle(float x, float y, float size)
{
	CShape::Shapes.push_back(new CRectangle(x, y, size));
}

void CApplication::SpawnHexagon(float x, float y, float radius)
{
	CShape::Shapes.push_back(new CHexagon(x, y, radius));
}

void CApplication::SpawnOctagon(float x, float y, float radius)
{
	CShape::Shapes.push_back(new COctagon(x, y, radius));
}

void CApplication::Resize(float scale)
{
}

int CApplication::Update(float dt, STriangle* tri)
{
	int tri_count = 0;
	std::list<CShape*>::iterator i = CShape::Shapes.begin();
	while (i != CShape::Shapes.end())
	{
		(*i)->Update(dt);
		tri_count += (*i)->Draw(&tri[tri_count]);
		i++;
	}

	return tri_count;
}
