// @otlicense

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

