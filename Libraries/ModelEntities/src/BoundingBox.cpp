// @otlicense
// File: BoundingBox.cpp
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


#include "BoundingBox.h"

#include <algorithm>

BoundingBox::BoundingBox()
	: xmin(0.0), xmax(0.0), ymin(0.0), ymax(0.0), zmin(0.0), zmax(0.0),
	  empty(true)
{
}


BoundingBox::~BoundingBox()
{
}

void BoundingBox::reset(void)
{
	xmin = xmax = ymin = ymax = zmin = zmax = 0.0;
	empty = true;
}

void BoundingBox::extend(double x, double y, double z)
{
	if (empty)
	{
		xmin = xmax = x;
		ymin = ymax = y;
		zmin = zmax = z;

		empty = false;
	}
	else
	{
		xmin = std::min(x, xmin);
		ymin = std::min(y, ymin);
		zmin = std::min(z, zmin);

		xmax = std::max(x, xmax);
		ymax = std::max(y, ymax);
		zmax = std::max(z, zmax);
	}
}

void BoundingBox::extend(const BoundingBox &box)
{
	if (box.getEmpty()) return;

	extend(box.getXmin(), box.getXmax(), box.getYmin(), box.getYmax(), box.getZmin(), box.getZmax());
}

void BoundingBox::extend(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
	extend(xmin, ymin, zmin);
	extend(xmax, ymax, zmax);
}

bool BoundingBox::testPointInside(double x, double y, double z, double tolerance) const
{
	if (getEmpty()) return false;

	if (   x < xmin - tolerance || x > xmax + tolerance
		|| y < ymin - tolerance || y > ymax + tolerance
		|| z < zmin - tolerance || z > zmax + tolerance)
	{
		return false;
	}

	return true;
}

bool BoundingBox::getCenterPoint(double &xc, double &yc, double &zc) const
{
	if (empty) return false;

	xc = 0.5 * (xmin + xmax);
	yc = 0.5 * (ymin + ymax);
	zc = 0.5 * (zmin + zmax);

	return true;
}

bsoncxx::document::value BoundingBox::getBSON(void)
{
	auto doc = bsoncxx::builder::basic::document{};

	doc.append(
		bsoncxx::builder::basic::kvp("Empty", getEmpty()),
		bsoncxx::builder::basic::kvp("Xmin", xmin),
		bsoncxx::builder::basic::kvp("Xmax", xmax),
		bsoncxx::builder::basic::kvp("Ymin", ymin),
		bsoncxx::builder::basic::kvp("Ymax", ymax),
		bsoncxx::builder::basic::kvp("Zmin", zmin),
		bsoncxx::builder::basic::kvp("Zmax", zmax)
	);

	return doc.extract();
}

void BoundingBox::setFromBSON(bsoncxx::document::view view)
{
	empty = view["Empty"].get_bool();
	xmin  = view["Xmin"].get_double();
	xmax  = view["Xmax"].get_double();
	ymin  = view["Ymin"].get_double();
	ymax  = view["Ymax"].get_double();
	zmin  = view["Zmin"].get_double();
	zmax  = view["Zmax"].get_double();
}
