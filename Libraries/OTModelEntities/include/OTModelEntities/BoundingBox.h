// @otlicense
// File: BoundingBox.h
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
#pragma warning(disable : 4251)

#include <bsoncxx/builder/basic/document.hpp>

class __declspec(dllexport) BoundingBox
{
public:
	BoundingBox();
	virtual ~BoundingBox();

	void reset(void);

	void extend(double x, double y, double z);
	void extend(const BoundingBox &box);
	void extend(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);

	bool testPointInside(double x, double y, double z, double tolerance) const;

	double getXmin(void) const { return xmin; };
	double getXmax(void) const { return xmax; };
	double getYmin(void) const { return ymin; };
	double getYmax(void) const { return ymax; };
	double getZmin(void) const { return zmin; };
	double getZmax(void) const { return zmax; };

	bool getEmpty(void) const { return empty; };

	bool getCenterPoint(double &xc, double &yc, double &zc) const;
	double getDiagonal(void) const { return sqrt((xmax - xmin)*(xmax - xmin) + (ymax - ymin)*(ymax - ymin) + (zmax - zmin)*(zmax - zmin)); };

	bsoncxx::document::value getBSON(void);
	void setFromBSON(bsoncxx::document::view view);

private:
	double xmin, xmax, ymin, ymax, zmin, zmax;
	bool empty;
};

