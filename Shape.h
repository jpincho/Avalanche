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
	void Update(float dt);
	int Draw(STriangle *tri);
	int GetType() const { return m_Type; }
	float GetSize() const { return m_Size; }

	void CheckCollision(const uint16_t Index);
	CPoint2d GetPosition() const { return m_Position; }
	const CPoint2d &GetPositionReference() const { return m_Position; }
	static CShape Shapes[32700];
	static uint16_t ShapeArrayLength;
	static uint8_t UpdateMask;
protected:
	CPoint2d m_Position, m_Direction, m_Target;
	float m_Size;
	float m_MinDistance;
	uint8_t m_Type;
	uint16_t m_ID;
};

#endif
