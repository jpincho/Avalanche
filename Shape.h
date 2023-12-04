////////////////////////////////////////////////////////////////
//
// Confidential, must not be distributed. 
// Copyright (C) 2010 Avalanche Studios. All rights reserved.
// May be altered only for the purpose of this test.
//
////////////////////////////////////////////////////////////////
#ifndef SHAPE_INCLUDE
#define SHAPE_INCLUDE

#include "Framework.h"
#include "Point2d.h"

class CShape
{
public:
	void Create(float x, float y, uint16_t id, uint8_t type, float size);
	void Destroy(void) const;
	void Update(float dt, const bool UpdateCollisionAttraction, bool CheckNeighbourCells);
	int Draw(STriangle *tri);

	static CShape Shapes[32700];
	static uint16_t ShapeArrayLength;
protected:
	CPoint2d m_Position, m_Direction, m_Target;
	float m_Size;
	uint8_t m_Type;
	uint16_t m_ID;
};

#endif
