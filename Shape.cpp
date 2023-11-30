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

std::list<CShape *> CShape::Shapes;
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
	: m_PosX(x), m_PosY(y), m_DirX(1.0f), m_DirY(0.1f), m_TargetX(0), m_TargetY(0), m_MinDistance(MaxSearchRange), m_Type(type), m_Size(size)
{
}

CShape::~CShape(void)
{
}

void CShape::FindTarget(CShape *shape)
{
	float delta_x = shape->m_PosX - m_PosX;
	float delta_y = shape->m_PosY - m_PosY;
	float distance = sqrtf(delta_x * delta_x + delta_y * delta_y);

	if (distance < m_MinDistance && shape->GetType() == AttractorType[GetType()])
	{
		m_MinDistance = distance;
		m_TargetX = delta_x / distance;
		m_TargetY = delta_y / distance;
	}
}

void CShape::CheckCollision(CShape *shape)
{
	if (Test(shape) || shape->Test(this))
	{
		float delta_x = shape->GetX() - m_PosX;
		float delta_y = shape->GetY() - m_PosY;

		float length = sqrtf(delta_x * delta_x + delta_y * delta_y);
		m_DirX = -delta_x / length;
		m_DirY = -delta_y / length;
	}
}

void CShape::Update(float dt)
{
	// Blend in target shape position
	m_DirX = m_DirX * (1.0f - TargetBlend) + m_TargetX * TargetBlend;
	m_DirY = m_DirY * (1.0f - TargetBlend) + m_TargetY * TargetBlend;

	// Reset target
	m_MinDistance = MaxSearchRange;
	m_TargetX = m_DirX;
	m_TargetY = m_DirY;

	// Normalize direction
	float length = sqrtf(m_DirX * m_DirX + m_DirY * m_DirY);
	m_DirX /= length;
	m_DirY /= length;

	// Move
	m_PosX += dt * m_DirX;
	m_PosY += dt * m_DirY;

	// Wrap around window frame
	if (m_PosX > WorldMaxX)
		m_PosX -= (WorldMaxX - WorldMinX);
	if (m_PosX < WorldMinX)
		m_PosX += (WorldMaxX - WorldMinX);
	if (m_PosY > WorldMaxY)
		m_PosY -= (WorldMaxY - WorldMinY);
	if (m_PosY < WorldMinY)
		m_PosY += (WorldMaxY - WorldMinY);

	// Check collision against other shapes
	std::list<CShape *>::iterator i = Shapes.begin();
	while (i != Shapes.end())
	{
		if (*i != this)
		{
			FindTarget(*i);
			CheckCollision(*i);
		}

		i++;
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
			tri->SetPosition(VertexIndex, m_PosX + TriangleVertices[VertexIndex].GetX() * m_Size, m_PosY + TriangleVertices[VertexIndex].GetY() * m_Size);
		}
		return 1;
	}
	case 1:
	{

		tri->SetColor(0, 255, 0, 0);
		tri->SetPosition(0, m_PosX + RectangleVertices[0].GetX() * m_Size, m_PosY + RectangleVertices[0].GetY() * m_Size);
		tri->SetColor(1, 255, 0, 0);
		tri->SetPosition(1, m_PosX + RectangleVertices[1].GetX() * m_Size, m_PosY + RectangleVertices[1].GetY() * m_Size);
		tri->SetColor(2, 255, 0, 0);
		tri->SetPosition(2, m_PosX + RectangleVertices[2].GetX() * m_Size, m_PosY + RectangleVertices[2].GetY() * m_Size);

		++tri;

		tri->SetColor(0, 255, 0, 0);
		tri->SetPosition(0, m_PosX + RectangleVertices[0].GetX() * m_Size, m_PosY + RectangleVertices[0].GetY() * m_Size);
		tri->SetColor(1, 255, 0, 0);
		tri->SetPosition(1, m_PosX + RectangleVertices[2].GetX() * m_Size, m_PosY + RectangleVertices[2].GetY() * m_Size);
		tri->SetColor(2, 255, 0, 0);
		tri->SetPosition(2, m_PosX + RectangleVertices[3].GetX() * m_Size, m_PosY + RectangleVertices[3].GetY() * m_Size);

		return 2;
	}
	case 2:
	{
		for (int a = 0; a < 6; a++)
		{
			tri->SetColor(0, 255, 0, 255);
			tri->SetPosition(0, m_PosX, m_PosY);
			tri->SetColor(1, 255, 0, 255);
			tri->SetPosition(1, m_PosX + HexagonVertices[a].GetX() * m_Size, m_PosY + HexagonVertices[a].GetY() * m_Size);
			tri->SetColor(2, 255, 0, 255);
			tri->SetPosition(2, m_PosX + HexagonVertices[(a + 1) % 6].GetX() * m_Size, m_PosY + HexagonVertices[(a + 1) % 6].GetY() * m_Size);
			tri++;
		}

		return 6;
	}
	case 3:
	{
		for (int a = 0; a < 8; a++)
		{
			tri->SetColor(0, 255, 255, 0);
			tri->SetPosition(0, m_PosX, m_PosY);
			tri->SetColor(1, 255, 255, 0);
			tri->SetPosition(1, m_PosX + OctagonVertices[a].GetX() * m_Size, m_PosY + OctagonVertices[a].GetY() * m_Size);
			tri->SetColor(2, 255, 255, 0);
			tri->SetPosition(2, m_PosX + OctagonVertices[(a + 1) % 8].GetX() * m_Size, m_PosY + OctagonVertices[(a + 1) % 8].GetY() * m_Size);
			tri++;
		}

		return 8;
	}
	}
	return 0;
}

bool CShape::Test(CShape *shape)
{
	switch (m_Type)
	{
	case 0:
	{
		return (shape->IsWithin(m_PosX, m_PosY + m_Size * 0.5f) || shape->IsWithin(m_PosX - m_Size * 0.5f, m_PosY - m_Size * 0.5f) || shape->IsWithin(m_PosX + m_Size * 0.5f, m_PosY - m_Size * 0.5f));
	}
	case 1:
	{
		return (shape->IsWithin(m_PosX - m_Size * 0.5f, m_PosY - m_Size * 0.5f) || shape->IsWithin(m_PosX + m_Size * 0.5f, m_PosY - m_Size * 0.5f) || shape->IsWithin(m_PosX - m_Size * 0.5f, m_PosY + m_Size * 0.5f) || shape->IsWithin(m_PosX + m_Size * 0.5f, m_PosY + m_Size * 0.5f));
	}
	case 2:
	{
		for (unsigned VertexIndex = 0; VertexIndex < HexagonVertices.size(); ++VertexIndex)
		{
			if (shape->IsWithin(m_PosX + HexagonVertices[VertexIndex].GetX() * m_Size, m_PosY + HexagonVertices[VertexIndex].GetY() * m_Size))
				return true;
		}

		return false;
	}
	case 3:
	{
		for (unsigned VertexIndex = 0; VertexIndex < OctagonVertices.size(); ++VertexIndex)
		{
			if (shape->IsWithin(m_PosX + OctagonVertices[VertexIndex].GetX() * m_Size, m_PosY + OctagonVertices[VertexIndex].GetY() * m_Size))
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
		float p0x = m_PosX, p0y = m_PosY + m_Size * 0.5f;
		float p1x = m_PosX + m_Size * 0.5f, p2y = m_PosY - m_Size * 0.5f;
		float p2x = m_PosX - m_Size * 0.5f, p1y = m_PosY - m_Size * 0.5f;

		return EdgeTest(p0x, p0y, p1x, p1y, x, y) && EdgeTest(p1x, p1y, p2x, p2y, x, y) && EdgeTest(p2x, p2y, p0x, p0y, x, y);
	}
	case 1:
	{
		return (x >= m_PosX - m_Size * 0.5f && x <= m_PosX + m_Size * 0.5f && y >= m_PosY - m_Size * 0.5f && y <= m_PosY + m_Size * 0.5f);
	}
	case 2:
	{
		int sum = 0;

		for (int a = 0; a < 6; a++)
			sum += EdgeTest(m_PosX + HexagonVertices[a].GetX() * m_Size, m_PosY + HexagonVertices[a].GetY() * m_Size, m_PosX + HexagonVertices[(a + 1) % 6].GetX() * m_Size, m_PosY + HexagonVertices[(a + 1) % 6].GetY() * m_Size, x, y);

		return sum == 6;
	}
	case 3:
	{
		int sum = 0;

		for (int a = 0; a < 8; a++)
			sum += EdgeTest(m_PosX + OctagonVertices[a].GetX() * m_Size, m_PosY + OctagonVertices[a].GetY() * m_Size, m_PosX + OctagonVertices[(a + 1) % 8].GetX() * m_Size, m_PosY + OctagonVertices[(a + 1) % 8].GetY() * m_Size, x, y);

		return sum == 8;
	}
	}
	return false;
}
