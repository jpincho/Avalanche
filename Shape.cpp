////////////////////////////////////////////////////////////////
//
// Confidential, must not be distributed. 
// Copyright (C) 2010 Avalanche Studios. All rights reserved.
// May be altered only for the purpose of this test.
//
////////////////////////////////////////////////////////////////
#include "Shape.h"
#include <math.h>
#include <array>

std::vector<CShape> CShape::Shapes;
const float pi = 3.141592f;
const float double_pi = 2.0f * pi;


static std::array <CPoint2d, 3> TriangleVertices = { CPoint2d(0.0f, 0.5f),
												CPoint2d(-0.5f, -0.5f),
												CPoint2d(0.5f, -0.5f) };
static std::array <CPoint2d, 4> RectangleVertices = { CPoint2d(-0.5f, -0.5f),
													CPoint2d(0.5f, -0.5f),
													CPoint2d(0.5f,  0.5f),
													CPoint2d(-0.5f,  0.5f) };
static std::array <CPoint2d, 6> HexagonVertices = { CPoint2d(cosf(double_pi / 6.0f * 0.0f), -sinf(double_pi / 6.0f * 0.0f)),
												  CPoint2d(cosf(double_pi / 6.0f * 1.0f), -sinf(double_pi / 6.0f * 1.0f)),
												  CPoint2d(cosf(double_pi / 6.0f * 2.0f), -sinf(double_pi / 6.0f * 2.0f)),
												  CPoint2d(cosf(double_pi / 6.0f * 3.0f), -sinf(double_pi / 6.0f * 3.0f)),
												  CPoint2d(cosf(double_pi / 6.0f * 4.0f), -sinf(double_pi / 6.0f * 4.0f)),
												  CPoint2d(cosf(double_pi / 6.0f * 5.0f), -sinf(double_pi / 6.0f * 5.0f)) };
static std::array <CPoint2d, 8> OctagonVertices = { CPoint2d(cosf(double_pi / 8.0f * 0.0f), -sinf(double_pi / 8.0f * 0.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 1.0f), -sinf(double_pi / 8.0f * 1.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 2.0f), -sinf(double_pi / 8.0f * 2.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 3.0f), -sinf(double_pi / 8.0f * 3.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 4.0f), -sinf(double_pi / 8.0f * 4.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 5.0f), -sinf(double_pi / 8.0f * 5.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 6.0f), -sinf(double_pi / 8.0f * 6.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 7.0f), -sinf(double_pi / 8.0f * 7.0f)) };

bool EdgeTest(float p0x, float p0y, float p1x, float p1y, float x, float y)
{
	float nx = -(p1y - p0y);
	float ny = p1x - p0x;

	float dot = nx * (x - p0x) + ny * (y - p0y);

	return dot < 0;
}

CShape::CShape(float x, float y, unsigned type, float size)
	: m_Position(x,y), m_Direction(1.0f,0.1f), m_Target(0,0), m_MinDistance(MaxSearchRange), m_Type(type), m_Size(size)
{
}

CShape::~CShape(void)
{
}

void CShape::FindTarget(CShape *shape)
{
	if (shape->m_Type == AttractorType[m_Type])
	{
		float delta_x = shape->m_Position.X - m_Position.X;
		float delta_y = shape->m_Position.Y - m_Position.Y;
		float distance = sqrtf(delta_x * delta_x + delta_y * delta_y);

		if (distance < m_MinDistance)
		{
			m_MinDistance = distance;
			m_Target.X = delta_x / distance;
			m_Target.Y = delta_y / distance;
		}
	}
}

void CShape::CheckCollision(CShape *shape)
{
	if (Test(shape) || shape->Test(this))
	{
		float delta_x = shape->m_Position.X - m_Position.X;
		float delta_y = shape->m_Position.Y - m_Position.Y;

		float length = sqrtf(delta_x * delta_x + delta_y * delta_y);
		m_Direction.X = -delta_x / length;
		m_Direction.Y = -delta_y / length;
	}
}

void CShape::Update(float dt)
{
	// Blend in target shape position
	m_Direction.X = m_Direction.X * (1.0f - TargetBlend) + m_Target.X * TargetBlend;
	m_Direction.Y = m_Direction.Y * (1.0f - TargetBlend) + m_Target.Y * TargetBlend;

	// Reset target
	m_MinDistance = MaxSearchRange;
	m_Target.X = m_Direction.X;
	m_Target.Y = m_Direction.Y;

	// Normalize direction
	float length = sqrtf(m_Direction.X * m_Direction.X + m_Direction.Y * m_Direction.Y);
	m_Direction.X /= length;
	m_Direction.Y /= length;

	// Move
	m_Position.X += dt * m_Direction.X;
	m_Position.Y += dt * m_Direction.Y;

	// Wrap around window frame
	if (m_Position.X > WorldMaxX)
		m_Position.X -= (WorldMaxX - WorldMinX);
	if (m_Position.X < WorldMinX)
		m_Position.X += (WorldMaxX - WorldMinX);
	if (m_Position.Y > WorldMaxY)
		m_Position.Y -= (WorldMaxY - WorldMinY);
	if (m_Position.Y < WorldMinY)
		m_Position.Y += (WorldMaxY - WorldMinY);

	// Check collision against other shapes
	for (unsigned ShapeIndex = 0; ShapeIndex < Shapes.size(); ++ShapeIndex)
	{
		if (&Shapes[ShapeIndex] == this)
			continue;
		FindTarget(&Shapes[ShapeIndex]);
		CheckCollision(&Shapes[ShapeIndex]);
	}
}

int CShape::Draw(STriangle *tri)
{
	switch (m_Type)
	{
	case 0:
	{
		for (unsigned VertexIndex = 0; VertexIndex < TriangleVertices.size(); ++VertexIndex)
		{
			tri->SetColor(VertexIndex, 0, 255, 255);
			tri->SetPosition(VertexIndex, m_Position.X + TriangleVertices[VertexIndex].X * m_Size, m_Position.Y + TriangleVertices[VertexIndex].Y * m_Size);
		}
		return 1;
	}
	case 1:
	{

		tri->SetColor(0, 255, 0, 0);
		tri->SetPosition(0, m_Position.X + RectangleVertices[0].X * m_Size, m_Position.Y + RectangleVertices[0].Y * m_Size);
		tri->SetColor(1, 255, 0, 0);
		tri->SetPosition(1, m_Position.X + RectangleVertices[1].X * m_Size, m_Position.Y + RectangleVertices[1].Y * m_Size);
		tri->SetColor(2, 255, 0, 0);
		tri->SetPosition(2, m_Position.X + RectangleVertices[2].X * m_Size, m_Position.Y + RectangleVertices[2].Y * m_Size);

		++tri;

		tri->SetColor(0, 255, 0, 0);
		tri->SetPosition(0, m_Position.X + RectangleVertices[0].X * m_Size, m_Position.Y + RectangleVertices[0].Y * m_Size);
		tri->SetColor(1, 255, 0, 0);
		tri->SetPosition(1, m_Position.X + RectangleVertices[2].X * m_Size, m_Position.Y + RectangleVertices[2].Y * m_Size);
		tri->SetColor(2, 255, 0, 0);
		tri->SetPosition(2, m_Position.X + RectangleVertices[3].X * m_Size, m_Position.Y + RectangleVertices[3].Y * m_Size);

		return 2;
	}
	case 2:
	{
		for (int a = 0; a < 6; a++)
		{
			tri->SetColor(0, 255, 0, 255);
			tri->SetPosition(0, m_Position.X, m_Position.Y);
			tri->SetColor(1, 255, 0, 255);
			tri->SetPosition(1, m_Position.X + HexagonVertices[a].X * m_Size, m_Position.Y + HexagonVertices[a].Y * m_Size);
			tri->SetColor(2, 255, 0, 255);
			tri->SetPosition(2, m_Position.X + HexagonVertices[(a + 1) % 6].X * m_Size, m_Position.Y + HexagonVertices[(a + 1) % 6].Y * m_Size);
			tri++;
		}

		return 6;
	}
	case 3:
	{
		for (int a = 0; a < 8; a++)
		{
			tri->SetColor(0, 255, 255, 0);
			tri->SetPosition(0, m_Position.X, m_Position.Y);
			tri->SetColor(1, 255, 255, 0);
			tri->SetPosition(1, m_Position.X + OctagonVertices[a].X * m_Size, m_Position.Y + OctagonVertices[a].Y * m_Size);
			tri->SetColor(2, 255, 255, 0);
			tri->SetPosition(2, m_Position.X + OctagonVertices[(a + 1) % 8].X * m_Size, m_Position.Y + OctagonVertices[(a + 1) % 8].Y * m_Size);
			tri++;
		}

		return 8;
	}
	}
	return 0;
}

bool CShape::Test(CShape *shape)
{
	// Get square distance to other object, for an early exit
	float delta_x = shape->m_Position.X - m_Position.X;
	delta_x *= delta_x;
	float delta_y = shape->m_Position.Y - m_Position.Y;
	delta_y *= delta_y;
	if (delta_x + delta_y > shape->m_Size * shape->m_Size + m_Size * m_Size)
		return false;

	switch (m_Type)
	{
	case 0:
	{
		for (unsigned VertexIndex = 0; VertexIndex < TriangleVertices.size(); ++VertexIndex)
		{
			if (shape->IsWithin(m_Position.X + TriangleVertices[VertexIndex].X * m_Size, m_Position.Y + TriangleVertices[VertexIndex].Y * m_Size))
				return true;
		}
		return false;
	}
	case 1:
	{
		for (unsigned VertexIndex = 0; VertexIndex < RectangleVertices.size(); ++VertexIndex)
		{
			if (shape->IsWithin(m_Position.X + RectangleVertices[VertexIndex].X * m_Size, m_Position.Y + RectangleVertices[VertexIndex].Y * m_Size))
				return true;
		}
		return false;
	}
	case 2:
	{
		for (unsigned VertexIndex = 0; VertexIndex < HexagonVertices.size(); ++VertexIndex)
		{
			if (shape->IsWithin(m_Position.X + HexagonVertices[VertexIndex].X * m_Size, m_Position.Y + HexagonVertices[VertexIndex].Y * m_Size))
				return true;
		}
		return false;
	}
	case 3:
	{
		for (unsigned VertexIndex = 0; VertexIndex < OctagonVertices.size(); ++VertexIndex)
		{
			if (shape->IsWithin(m_Position.X + OctagonVertices[VertexIndex].X * m_Size, m_Position.Y + OctagonVertices[VertexIndex].Y * m_Size))
				return true;
		}
		return false;
	}
	}
	return false;
}

bool CShape::IsWithin(float x, float y)
{
	switch (m_Type)
	{
	case 0:
	{
		float p0x = m_Position.X, p0y = m_Position.Y + m_Size * 0.5f;
		float p1x = m_Position.X + m_Size * 0.5f, p2y = m_Position.Y - m_Size * 0.5f;
		float p2x = m_Position.X - m_Size * 0.5f, p1y = m_Position.Y - m_Size * 0.5f;

		return EdgeTest(p0x, p0y, p1x, p1y, x, y) && EdgeTest(p1x, p1y, p2x, p2y, x, y) && EdgeTest(p2x, p2y, p0x, p0y, x, y);
	}
	case 1:
	{
		return (x >= m_Position.X - m_Size * 0.5f && x <= m_Position.X + m_Size * 0.5f && y >= m_Position.Y - m_Size * 0.5f && y <= m_Position.Y + m_Size * 0.5f);
	}
	case 2:
	{
		for (unsigned VertexIndex = 0; VertexIndex < HexagonVertices.size(); ++VertexIndex)
		{
			if (EdgeTest(m_Position.X + HexagonVertices[VertexIndex].X * m_Size, m_Position.Y + HexagonVertices[VertexIndex].Y * m_Size, m_Position.X + HexagonVertices[(VertexIndex + 1) % HexagonVertices.size()].X * m_Size, m_Position.Y + HexagonVertices[(VertexIndex + 1) % HexagonVertices.size()].Y * m_Size, x, y) == false)
				return false;
		}
		return true;
	}
	case 3:
	{
		for (unsigned VertexIndex = 0; VertexIndex < OctagonVertices.size(); ++VertexIndex)
		{
			if (EdgeTest(m_Position.X + OctagonVertices[VertexIndex].X * m_Size, m_Position.Y + OctagonVertices[VertexIndex].Y * m_Size, m_Position.X + OctagonVertices[(VertexIndex + 1) % OctagonVertices.size()].X * m_Size, m_Position.Y + OctagonVertices[(VertexIndex + 1) % OctagonVertices.size()].Y * m_Size, x, y) == false)
				return false;
		}
		return true;
	}
	}
	return false;
}
