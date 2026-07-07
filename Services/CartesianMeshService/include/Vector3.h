// @otlicense
// File: Vector3.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

#include <cmath>

class vector3
{
public:
	vector3() : x(0.0f), y(0.0f), z(0.0f) {}
	vector3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}

	void setX(float v) { x = v; }
	void setY(float v) { y = v; }
	void setZ(float v) { z = v; }

	float getX() const { return x; }
	float getY() const { return y; }
	float getZ() const { return z; }

	float dot(const vector3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	vector3 cross(const vector3& other) const
	{
		return vector3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x);
	}

	float length() const
	{
		return std::sqrt(dot(*this));
	}

	void normalize()
	{
		float l = length();
		if (l > 0.0f)
		{
			x /= l;
			y /= l;
			z /= l;
		}
	}

	vector3 normalized() const
	{
		vector3 v(*this);
		v.normalize();
		return v;
	}

	vector3 operator+(const vector3& other) const
	{
		return vector3(x + other.x,
			y + other.y,
			z + other.z);
	}

	vector3 operator-(const vector3& other) const
	{
		return vector3(x - other.x,
			y - other.y,
			z - other.z);
	}

	vector3 operator*(float s) const
	{
		return vector3(x * s,
			y * s,
			z * s);
	}

	vector3 operator/(float s) const
	{
		return vector3(x / s,
			y / s,
			z / s);
	}

	vector3& operator+=(const vector3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

private:
	float x;
	float y;
	float z;
};

vector3 operator*(float lhs, const vector3& rhs);
