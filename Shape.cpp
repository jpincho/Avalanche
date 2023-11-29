////////////////////////////////////////////////////////////////
//
// Confidential, must not be distributed. 
// Copyright (C) 2010 Avalanche Studios. All rights reserved.
// May be altered only for the purpose of this test.
//
////////////////////////////////////////////////////////////////
#include "Shape.h"
#include <math.h>

std::list<CShape*> CShape::Shapes;

CShape::CShape(float x, float y)
: m_PosX(x), m_PosY(y), m_DirX(1.0f), m_DirY(0.1f), m_TargetX(0), m_TargetY(0), m_MinDistance(MaxSearchRange)
{
}

CShape::~CShape(void)
{
}

void CShape::FindTarget(CShape* shape)
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

void CShape::CheckCollision(CShape* shape)
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
		m_PosX -= (WorldMaxX-WorldMinX);
	if (m_PosX < WorldMinX)
		m_PosX += (WorldMaxX-WorldMinX);
	if (m_PosY > WorldMaxY)
		m_PosY -= (WorldMaxY-WorldMinY);
	if (m_PosY < WorldMinY)
		m_PosY += (WorldMaxY-WorldMinY);

	// Check collision against other shapes
	std::list<CShape*>::iterator i = Shapes.begin();
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

//////////////////////////////////////////////////////////////////
//
//
// Triangle shape
//
//
//////////////////////////////////////////////////////////////////

CTriangle::CTriangle(float x, float y, float size)
: CShape(x, y), m_Size(size)
{

}

bool CTriangle::Test(CShape* shape)
{
	return (shape->IsWithin(m_PosX, m_PosY + m_Size * 0.5f) || shape->IsWithin(m_PosX - m_Size * 0.5f, m_PosY - m_Size * 0.5f) || shape->IsWithin(m_PosX + m_Size * 0.5f, m_PosY - m_Size * 0.5f));
}

bool EdgeTest(float p0x, float p0y, float p1x, float p1y, float x, float y)
{
	float nx = -(p1y - p0y);
	float ny = p1x - p0x;

	float dot = nx * (x-p0x) + ny * (y-p0y);

	return dot < 0;
}

bool CTriangle::IsWithin(float x, float y)
{
	float p0x = m_PosX, p0y = m_PosY + m_Size * 0.5f;
	float p1x = m_PosX + m_Size * 0.5f, p2y = m_PosY - m_Size * 0.5f;
	float p2x = m_PosX - m_Size * 0.5f, p1y = m_PosY - m_Size * 0.5f;

	return EdgeTest(p0x, p0y, p1x, p1y, x, y) && EdgeTest(p1x, p1y, p2x, p2y, x, y) && EdgeTest(p2x, p2y, p0x, p0y, x, y);
}

void CTriangle::Update(float dt)
{
	CShape::Update(dt);
}

int CTriangle::Draw(STriangle* tri)
{
	tri->SetColor(0, 0, 255, 255);
	tri->SetPosition(0, m_PosX, m_PosY + m_Size * 0.5f);

	tri->SetColor(1, 0, 255, 255);
	tri->SetPosition(1, m_PosX - m_Size * 0.5f, m_PosY - m_Size * 0.5f);

	tri->SetColor(2, 0, 255, 255);
	tri->SetPosition(2, m_PosX + m_Size * 0.5f, m_PosY - m_Size * 0.5f);

	return 1;
}

//////////////////////////////////////////////////////////////////
//
//
// Rectangle shape
//
//
//////////////////////////////////////////////////////////////////

CRectangle::CRectangle(float x, float y, float size)
: CShape(x, y), m_Size(size)
{

}

bool CRectangle::Test(CShape* shape)
{
	return (shape->IsWithin(m_PosX - m_Size * 0.5f, m_PosY - m_Size * 0.5f) || shape->IsWithin(m_PosX + m_Size * 0.5f, m_PosY - m_Size * 0.5f) || shape->IsWithin(m_PosX - m_Size * 0.5f, m_PosY + m_Size * 0.5f) || shape->IsWithin(m_PosX + m_Size * 0.5f, m_PosY + m_Size * 0.5f));
}

bool CRectangle::IsWithin(float x, float y)
{
	return (x >= m_PosX - m_Size * 0.5f && x <= m_PosX + m_Size * 0.5f && y >= m_PosY - m_Size * 0.5f && y <= m_PosY + m_Size * 0.5f);
}

void CRectangle::Update(float dt)
{
	CShape::Update(dt);
}

int CRectangle::Draw(STriangle* tri)
{
	tri->SetColor(0, 255, 0, 0);
	tri->SetPosition(0, m_PosX - m_Size * 0.5f, m_PosY - m_Size * 0.5f);
	tri->SetColor(1, 255, 0, 0);
	tri->SetPosition(1, m_PosX + m_Size * 0.5f, m_PosY - m_Size * 0.5f);
	tri->SetColor(2, 255, 0, 0);
	tri->SetPosition(2, m_PosX + m_Size * 0.5f, m_PosY + m_Size * 0.5f);
	
	tri++;

	tri->SetColor(0, 255, 0, 0);
	tri->SetPosition(0, m_PosX - m_Size * 0.5f, m_PosY - m_Size * 0.5f);
	tri->SetColor(1, 255, 0, 0);
	tri->SetPosition(1, m_PosX + m_Size * 0.5f, m_PosY + m_Size * 0.5f);
	tri->SetColor(2, 255, 0, 0);
	tri->SetPosition(2, m_PosX - m_Size * 0.5f, m_PosY + m_Size * 0.5f);

	return 2;
}


//////////////////////////////////////////////////////////////////
//
//
// Hexagon shape
//
//
//////////////////////////////////////////////////////////////////

CHexagon::CHexagon(float x, float y, float radius)
: CShape(x, y), m_Radius(radius)
{
	float radians = 0.0f;
	for (int a = 0;  a < 6; a++, radians += 3.141592f / 6.0f * 2.0f)
	{
		m_Points[a] = CPoint2d(cosf(radians) * m_Radius, -sinf(radians) * m_Radius);
	}
}

bool CHexagon::Test(CShape* shape)
{
	for (int a = 0;  a < 6; a++)
		if (shape->IsWithin(m_PosX + m_Points[a%6].GetX(), m_PosY + m_Points[a%6].GetY()))
			return true;

	return false;
}

bool CHexagon::IsWithin(float x, float y)
{
	int sum = 0;

	for (int a = 0;  a < 6; a++)
		sum += EdgeTest(m_PosX + m_Points[a].GetX(), m_PosY + m_Points[a].GetY(), m_PosX + m_Points[(a+1)%6].GetX(), m_PosY + m_Points[(a+1)%6].GetY(), x, y);

	return sum == 6;
}

void CHexagon::Update(float dt)
{
	CShape::Update(dt);
}

int CHexagon::Draw(STriangle* tri)
{
	for (int a = 0; a < 6; a++)
	{
		tri->SetColor(0, 255, 0, 255);
		tri->SetPosition(0, m_PosX, m_PosY);
		tri->SetColor(1, 255, 0, 255);
		tri->SetPosition(1, m_PosX + m_Points[a].GetX(), m_PosY + m_Points[a].GetY());
		tri->SetColor(2, 255, 0, 255);
		tri->SetPosition(2, m_PosX + m_Points[(a+1)%6].GetX(), m_PosY + m_Points[(a+1)%6].GetY());
		tri++;
	}

	return 6;
}

//////////////////////////////////////////////////////////////////
//
//
// Octagon shape
//
//
//////////////////////////////////////////////////////////////////

COctagon::COctagon(float x, float y, float radius)
: CShape(x, y), m_Radius(radius)
{
	float radians = 0.0f;
	for (int a = 0;  a < 8; a++, radians += 3.141592f / 8.0f * 2.0f)
	{
		m_Points[a] = CPoint2d(cosf(radians) * m_Radius, -sinf(radians) * m_Radius);
	}
}

bool COctagon::Test(CShape* shape)
{
	for (int a = 0;  a < 8; a++)
		if (shape->IsWithin(m_PosX + m_Points[a%8].GetX(), m_PosY + m_Points[a%8].GetY()))
			return true;

	return false;
}

bool COctagon::IsWithin(float x, float y)
{
	int sum = 0;

	for (int a = 0;  a < 8; a++)
		sum += EdgeTest(m_PosX + m_Points[a].GetX(), m_PosY + m_Points[a].GetY(), m_PosX + m_Points[(a+1)%8].GetX(), m_PosY + m_Points[(a+1)%8].GetY(), x, y);

	return sum == 8;
}

void COctagon::Update(float dt)
{
	CShape::Update(dt);
}

int COctagon::Draw(STriangle* tri)
{
	for (int a = 0; a < 8; a++)
	{
		tri->SetColor(0, 255, 255, 0);
		tri->SetPosition(0, m_PosX, m_PosY);
		tri->SetColor(1, 255, 255, 0);
		tri->SetPosition(1, m_PosX + m_Points[a].GetX(), m_PosY + m_Points[a].GetY());
		tri->SetColor(2, 255, 255, 0);
		tri->SetPosition(2, m_PosX + m_Points[(a+1)%8].GetX(), m_PosY + m_Points[(a+1)%8].GetY());
		tri++;
	}

	return 8;
}

