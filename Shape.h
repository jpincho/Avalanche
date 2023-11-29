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
#include <list>

class CPoint2d
{
public:
	CPoint2d() : m_X(0), m_Y(0) {}
	CPoint2d(float x, float y) : m_X(x), m_Y(y) {}
	
	float GetX() const {return m_X;}
	float GetY() const {return m_Y;}

private:
	float m_X, m_Y;
};

class CShape
{
public:
	CShape(float x, float y);
	virtual ~CShape(void);

	virtual void Update(float dt);
	virtual int Draw(STriangle* tri) = 0;
	virtual bool Test(CShape* shape) = 0;
	virtual bool IsWithin(float x, float y) = 0;
	virtual int GetType() const = 0;

	void FindTarget(CShape* shape);
	void CheckCollision(CShape* shape);

	float GetX() const {return m_PosX;}
	float GetY() const {return m_PosY;}

	static std::list<CShape*> Shapes;

protected:
	float m_PosX, m_PosY;
	float m_DirX, m_DirY;
	float m_TargetX, m_TargetY;
	float m_MinDistance;
};

class CTriangle : public CShape
{
public:
	CTriangle(float x, float y, float size);
	virtual ~CTriangle(void) {}

	virtual void Update(float dt);
	virtual int Draw(STriangle* tri);
	virtual bool Test(CShape* shape);
	virtual bool IsWithin(float x, float y);
	virtual int GetType() const {return 0;}

private:
	float m_Size;
};

class CRectangle : public CShape
{
public:
	CRectangle(float x, float y, float size);
	virtual ~CRectangle(void) {}

	virtual void Update(float dt);
	virtual int Draw(STriangle* tri);
	virtual bool Test(CShape* shape);
	virtual bool IsWithin(float x, float y);
	virtual int GetType() const {return 1;}

private:
	float m_Size;
};

class CHexagon : public CShape
{
public:
	CHexagon(float x, float y, float radius);
	virtual ~CHexagon(void) {}

	virtual void Update(float dt);
	virtual int Draw(STriangle* tri);
	virtual bool Test(CShape* shape);
	virtual bool IsWithin(float x, float y);
	virtual int GetType() const {return 2;}

private:
	float m_Radius;
	CPoint2d m_Points[6];
};

class COctagon : public CShape
{
public:
	COctagon(float x, float y, float radius);
	virtual ~COctagon(void) {}

	virtual void Update(float dt);
	virtual int Draw(STriangle* tri);
	virtual bool Test(CShape* shape);
	virtual bool IsWithin(float x, float y);
	virtual int GetType() const {return 3;}

private:
	float m_Radius;
	CPoint2d m_Points[8];
};

#endif
