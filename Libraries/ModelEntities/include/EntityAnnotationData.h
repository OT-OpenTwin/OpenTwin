#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "OldTreeIcon.h"
#include <vector>
#include <array>

class __declspec(dllexport) EntityAnnotationData : public EntityBase
{
public:
	EntityAnnotationData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityAnnotationData();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void addPoint(double x, double y, double z, double r, double g, double b);
	void addTriangle(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double r, double g, double b);

	const std::vector<std::array<double, 3>> &getPoints(void) { return points; };
	const std::vector<std::array<double, 3>> &getPointsRGB(void) { return points_rgb; };

	const std::vector<std::array<double, 3>> &getTriangleP1(void) { return triangle_p1; };
	const std::vector<std::array<double, 3>> &getTriangleP2(void) { return triangle_p2; };
	const std::vector<std::array<double, 3>> &getTriangleP3(void) { return triangle_p3; };
	const std::vector<std::array<double, 3>> &getTriangleRGB(void) { return triangle_rgb; };

	const double* getEdgeColorRGB() { return edgeColorRGB; };

	virtual std::string getClassName(void) { return "EntityAnnotationData"; };

	virtual entityType getEntityType(void) override { return DATA; };

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	double edgeColorRGB[3];

	std::vector<std::array<double, 3>> points;	
	std::vector<std::array<double, 3>> points_rgb;

	std::vector<std::array<double, 3>> triangle_p1;
	std::vector<std::array<double, 3>> triangle_p2;
	std::vector<std::array<double, 3>> triangle_p3;
	std::vector<std::array<double, 3>> triangle_rgb;
};



