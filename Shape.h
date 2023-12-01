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
#include <vector>
#include "Point2d.h"

class CShape
{
public:
	CShape(float x, float y, unsigned type, float size);
	~CShape(void);

	void Update(float dt);
	int Draw(STriangle *tri);
	bool Test(const CShape *shape) const;
	bool IsWithin(float x, float y) const;
	int GetType() const { return m_Type; }
	float GetSize() const { return m_Size; }

	void CheckCollision(const uint16_t Index);
	void CheckCollisions(const std::vector <uint16_t> &Indices);
	CPoint2d GetPosition() const { return m_Position; }
	const CPoint2d &GetPositionReference() const { return m_Position; }
	static std::vector<CShape> Shapes;

protected:
	CPoint2d m_Position, m_Direction, m_Target;
	float m_Size;
	float m_MinDistance;
	uint8_t m_Type;
};

#endif
