////////////////////////////////////////////////////////////////
//
// Confidential, must not be distributed. 
// Copyright (C) 2010 Avalanche Studios. All rights reserved.
// May be altered only for the purpose of this test.
//
////////////////////////////////////////////////////////////////
#ifndef APPLICATION_INCLUDE
#define APPLICATION_INCLUDE

#include "Framework.h"

class CApplication : public IApplication
{
private:
	uint16_t UpdateMask, InitialUpdateMask, UpdateMaskShift, CheckNeighbourCells;
public:
	CApplication(void);
	virtual ~CApplication(void);

	virtual int Update(float dt, STriangle* tri);

	virtual void DestroyShapes(int num = MaxNumOfShapes);
	virtual void SpawnTriangle(float x, float y, float size);
	virtual void SpawnRectangle(float x, float y, float size);
	virtual void SpawnHexagon(float x, float y, float radius);
	virtual void SpawnOctagon(float x, float y, float radius);
	void SpawnNewShape(const float x, const float y, const float size, const uint8_t type);
	virtual void Resize(float scale);
};

#endif