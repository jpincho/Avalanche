#pragma once

template <typename Type> struct TPoint2d
{
	TPoint2d() = default;
	TPoint2d(Type x, Type y) : X(x), Y(y) {}

	TPoint2d<Type> &operator += (const TPoint2d<Type> &Other)
	{
		X += Other.X;
		Y += Other.Y;
		return *this;
	}
	TPoint2d<Type> &operator -= (const TPoint2d<Type> &Other)
	{
		X -= Other.X;
		Y -= Other.Y;
		return *this;
	}
	TPoint2d<Type> &operator /= (const float Scalar)
	{
		X /= Scalar;
		Y /= Scalar;
		return *this;
	}
	bool operator == (const TPoint2d<Type> &Other) const
	{
		return ((X == Other.X) && (Y == Other.Y));
	}
	bool operator != (const TPoint2d<Type> &Other) const
	{
		return !operator ==(Other);
	}
	Type X, Y;
};

typedef TPoint2d <float> CPoint2d;
typedef TPoint2d <int> CPoint2di;