#pragma once

class Vec2
{
public:
	float x = 0;
	float y = 0;

	Vec2();
	Vec2(float xin, float yin);

	bool operator == (const Vec2& rhs) const;
	bool operator != (const Vec2& rhs) const;

	Vec2 operator + (const Vec2& rhs) const;
	Vec2 operator - (const Vec2& rhs) const;
	Vec2 operator / (const float val) const;
	Vec2 operator * (const float val) const;
	
	void operator += (const Vec2& rhs);
	void operator -= (const Vec2& rhs);
	void operator *= (const float val);
	void operator /= (const float val);
	
	// calculates the distance between destination (dest) and origin (this*)
	float dist(const Vec2& dest) const;

	// calculates the angle of this* using its x,y components
	// this* must be a difference vector between two positions (Vec2 dest - Vec2 origin)
	// returns angle in degrees
	float angle() const;

	// calcutates the velocity vector
	// angle must be in degrees
	static Vec2 velocity(float speed, float angle);
};