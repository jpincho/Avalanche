////////////////////////////////////////////////////////////////
//
// Confidential, must not be distributed. 
// Copyright (C) 2010 Avalanche Studios. All rights reserved.
// May be altered only for the purpose of this test.
//
////////////////////////////////////////////////////////////////
#include "Shape.h"
#include <math.h>

const float pi = 3.141592f;
const float double_pi = 2.0f * pi;

CShape *CShape::Shapes = nullptr;
std::vector<uint16_t> *CShape::Grid = nullptr;
uint16_t CShape::ShapeArrayLength = 0;

static CPoint2d TriangleVertices[3] = { CPoint2d(0.0f, 0.5f),
												CPoint2d(-0.5f, -0.5f),
												CPoint2d(0.5f, -0.5f) };
static CPoint2d RectangleVertices[4] = { CPoint2d(-0.5f, -0.5f),
													CPoint2d(0.5f, -0.5f),
													CPoint2d(0.5f,  0.5f),
													CPoint2d(-0.5f,  0.5f) };
static CPoint2d HexagonVertices[6] = { CPoint2d(cosf(double_pi / 6.0f * 0.0f), -sinf(double_pi / 6.0f * 0.0f)),
												  CPoint2d(cosf(double_pi / 6.0f * 1.0f), -sinf(double_pi / 6.0f * 1.0f)),
												  CPoint2d(cosf(double_pi / 6.0f * 2.0f), -sinf(double_pi / 6.0f * 2.0f)),
												  CPoint2d(cosf(double_pi / 6.0f * 3.0f), -sinf(double_pi / 6.0f * 3.0f)),
												  CPoint2d(cosf(double_pi / 6.0f * 4.0f), -sinf(double_pi / 6.0f * 4.0f)),
												  CPoint2d(cosf(double_pi / 6.0f * 5.0f), -sinf(double_pi / 6.0f * 5.0f)) };
static CPoint2d OctagonVertices[8] = { CPoint2d(cosf(double_pi / 8.0f * 0.0f), -sinf(double_pi / 8.0f * 0.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 1.0f), -sinf(double_pi / 8.0f * 1.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 2.0f), -sinf(double_pi / 8.0f * 2.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 3.0f), -sinf(double_pi / 8.0f * 3.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 4.0f), -sinf(double_pi / 8.0f * 4.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 5.0f), -sinf(double_pi / 8.0f * 5.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 6.0f), -sinf(double_pi / 8.0f * 6.0f)),
												  CPoint2d(cosf(double_pi / 8.0f * 7.0f), -sinf(double_pi / 8.0f * 7.0f)) };

inline void GetGridPosition(const float ShapeX, const float ShapeY, int *GridX, int *GridY)
{
	// because shapes that are positioned exactly at 1.0 at either coordinate would be put somewhere outside the world, I'm just making sure it still uses the last row and col
	*GridX = (int)(((ShapeX - WorldMinX) / (WorldMaxX - WorldMinX)) * GridSideCellCount);
	*GridY = (int)(((ShapeY - WorldMinY) / (WorldMaxY - WorldMinY)) * GridSideCellCount);

	if (*GridX >= GridSideCellCount)
		*GridX = GridSideCellCount - 1;
	if (*GridY >= GridSideCellCount)
		*GridY = GridSideCellCount - 1;
}

inline unsigned GetGridOffset(const CPoint2di &GridPosition)
{
	return GridPosition.X + GridPosition.Y * GridSideCellCount;
}

void CShape::Create(float x, float y, uint16_t id, uint8_t type, float size)
{
	m_Position.X = x;
	m_Position.Y = y;
	m_ID = id;
	m_Direction.X = 1.0f;
	m_Direction.Y = 0.1f;
	m_Target.X = 0.0f;
	m_Target.Y = 0.0f;
	m_Type = type;
	m_Size = size;
	CPoint2di GridPosition;
	GetGridPosition(m_Position.X, m_Position.Y, &GridPosition.X, &GridPosition.Y);
	unsigned Offset = GetGridOffset(GridPosition);
	Grid[Offset].push_back(m_ID);
}

void CShape::Destroy(void) const
{
	CPoint2di GridPosition;
	GetGridPosition(m_Position.X, m_Position.Y, &GridPosition.X, &GridPosition.Y);
	unsigned Offset = GetGridOffset(GridPosition);
	Grid[Offset].erase(std::find(Grid[Offset].begin(), Grid[Offset].end(), m_ID));
}

void CShape::Update(float dt, const bool UpdateCollisionAttraction, bool CheckNeighbourCells)
{
	// Blend in target shape position
	m_Direction.X = m_Direction.X * (1.0f - TargetBlend) + m_Target.X * TargetBlend;
	m_Direction.Y = m_Direction.Y * (1.0f - TargetBlend) + m_Target.Y * TargetBlend;

	// Normalize direction
	float length = sqrtf(m_Direction.X * m_Direction.X + m_Direction.Y * m_Direction.Y);
	m_Direction.X /= length;
	m_Direction.Y /= length;

	// Reset target
	float MaxRangeSquared = MaxSearchRange * MaxSearchRange;
	m_Target.X = m_Direction.X;
	m_Target.Y = m_Direction.Y;

	// Move
	CPoint2di OldGridPosition, NewGridPosition;
	GetGridPosition(m_Position.X, m_Position.Y, &OldGridPosition.X, &OldGridPosition.Y);
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

	// Update grid position if needed
	GetGridPosition(m_Position.X, m_Position.Y, &NewGridPosition.X, &NewGridPosition.Y);
	if (OldGridPosition != NewGridPosition)
	{
		unsigned Offset = GetGridOffset(OldGridPosition);
		Grid[Offset].erase(std::find(Grid[Offset].begin(), Grid[Offset].end(), m_ID));
		Offset = GetGridOffset(NewGridPosition);
		Grid[Offset].push_back(m_ID);
		Grid[Offset].shrink_to_fit();// Boom! long live memory fragmentation!
	}
	if (!UpdateCollisionAttraction)
		return;

	int MinX, MaxX, MinY, MaxY;
	MinX = std::max(NewGridPosition.X - 1, 0);
	MaxX = std::min(NewGridPosition.X + 1, GridSideCellCount - 1);
	MinY = std::max(NewGridPosition.Y - 1, 0);
	MaxY = std::min(NewGridPosition.Y + 1, GridSideCellCount - 1);

	CPoint2di Iterator;

	bool Attracted = false, Collided = false;
	unsigned Offset = NewGridPosition.X + NewGridPosition.Y * GridSideCellCount;
	for (uint16_t Index : Grid[Offset])
	{
		if (Index == m_ID)
			continue;

		// Quick check to see if the shape is outside my bounding box
		if (Shapes[Index].m_Position.X < m_Position.X - MaxSearchRange) continue;
		if (Shapes[Index].m_Position.X > m_Position.X + MaxSearchRange) continue;
		if (Shapes[Index].m_Position.Y < m_Position.Y - MaxSearchRange) continue;
		if (Shapes[Index].m_Position.Y > m_Position.Y + MaxSearchRange) continue;
		float DeltaX = Shapes[Index].m_Position.X - m_Position.X;
		float DeltaY = Shapes[Index].m_Position.Y - m_Position.Y;
		float DistanceSquared = DeltaX * DeltaX + DeltaY * DeltaY;

		Attracted = ((Shapes[Index].m_Type == AttractorType[m_Type]) && (DistanceSquared < MaxRangeSquared));
		Collided = (DistanceSquared < m_Size * m_Size + Shapes[Index].m_Size * Shapes[Index].m_Size);

		if (Attracted || Collided)
		{
			CheckNeighbourCells = false;
			float Distance = sqrtf(DistanceSquared);
			DeltaX /= Distance;
			DeltaY /= Distance;
			if (Attracted)
			{
				MaxRangeSquared = DistanceSquared;
				m_Target.X = DeltaX;
				m_Target.Y = DeltaY;
			}
			if (Collided)
			{
				m_Direction.X = -DeltaX;
				m_Direction.Y = -DeltaY;
			}
		}
	}

	// If I've already been attracted or collided with a shape in my own cell, skip checking against farther neighbours.
	// There's an edge case here where this shape is near the edge of the cell, and there's a closer shape on the other side of the edge, but I'll just ignore it...
	if (CheckNeighbourCells == false)
		return;

	for (Iterator.X = MinX; Iterator.X <= MaxX; ++Iterator.X)
	{
		for (Iterator.Y = MinY; Iterator.Y <= MaxY; ++Iterator.Y)
		{
			if (Iterator == NewGridPosition)
				continue;
			unsigned Offset = Iterator.X + Iterator.Y * GridSideCellCount;
			for (uint16_t Index : Grid[Offset])
			{
				// Quick check to see if the shape is outside my bounding box
				if (Shapes[Index].m_Position.X < m_Position.X - MaxSearchRange) continue;
				if (Shapes[Index].m_Position.X > m_Position.X + MaxSearchRange) continue;
				if (Shapes[Index].m_Position.Y < m_Position.Y - MaxSearchRange) continue;
				if (Shapes[Index].m_Position.Y > m_Position.Y + MaxSearchRange) continue;
				float DeltaX = Shapes[Index].m_Position.X - m_Position.X;
				float DeltaY = Shapes[Index].m_Position.Y - m_Position.Y;
				float DistanceSquared = DeltaX * DeltaX + DeltaY * DeltaY;

				bool Attracted = ((Shapes[Index].m_Type == AttractorType[m_Type]) && (DistanceSquared < MaxRangeSquared));
				bool Collided = (DistanceSquared < m_Size * m_Size + Shapes[Index].m_Size * Shapes[Index].m_Size);

				if (Attracted || Collided)
				{
					float Distance = sqrtf(DistanceSquared);
					DeltaX /= Distance;
					DeltaY /= Distance;
					if (Attracted)
					{
						MaxRangeSquared = DistanceSquared;
						m_Target.X = DeltaX;
						m_Target.Y = DeltaY;
					}
					if (Collided)
					{
						m_Direction.X = -DeltaX;
						m_Direction.Y = -DeltaY;
					}
				}
			}
		}
	}
}

int CShape::Draw(STriangle *tri)
{
	switch (m_Type)
	{
	case 0:
	{
		for (unsigned VertexIndex = 0; VertexIndex < 3; ++VertexIndex)
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
