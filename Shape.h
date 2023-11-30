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

struct CPoint2d
{
	CPoint2d() = default;
	CPoint2d(float x, float y) : X(x), Y(y) {}

	CPoint2d &operator += (const CPoint2d &Other)
	{
		X += Other.X;
		Y += Other.Y;
		return *this;
	}
	CPoint2d &operator -= (const CPoint2d &Other)
	{
		X -= Other.X;
		Y -= Other.Y;
		return *this;
	}
	CPoint2d &operator /= (const float Scalar)
	{
		X /= Scalar;
		Y /= Scalar;
		return *this;
	}
	float X, Y;
};

class CShape
{
public:
	CShape(float x, float y, unsigned type, float size);
	~CShape(void);

	void Update(float dt);
	int Draw(STriangle *tri);
	bool Test(CShape *shape);
	bool IsWithin(float x, float y);
	int GetType() const { return m_Type; }
	float GetSize() const { return m_Size; }

	void FindTarget(CShape *shape);
	void CheckCollision(CShape *shape);

	float GetX() const { return m_Position.X; }
	float GetY() const { return m_Position.Y; }
	CPoint2d GetPosition() const { return m_Position; }
	const CPoint2d &GetPositionReference() const { return m_Position; }
	static std::vector<CShape> Shapes;

protected:
	CPoint2d m_Position, m_Direction, m_Target;
	float m_Size;
	float m_MinDistance;
	unsigned m_Type;
};

#endif
