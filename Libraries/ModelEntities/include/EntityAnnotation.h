#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "OldTreeIcon.h"
#include <vector>
#include <array>

class EntityAnnotationData;

class __declspec(dllexport) EntityAnnotation : public EntityBase {
public:
	EntityAnnotation(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityAnnotation();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void addPoint(double x, double y, double z, double r, double g, double b);
	void addTriangle(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double r, double g, double b);

	const std::vector<std::array<double, 3>> &getPoints(void);
	const std::vector<std::array<double, 3>> &getPointsRGB(void);

	const std::vector<std::array<double, 3>> &getTriangleP1(void);
	const std::vector<std::array<double, 3>> &getTriangleP2(void);
	const std::vector<std::array<double, 3>> &getTriangleP3(void);
	const std::vector<std::array<double, 3>> &getTriangleRGB(void);

	const double* getEdgeColorRGB();

	virtual void StoreToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	
	virtual std::string getClassName(void) { return "EntityAnnotation"; };

	void addVisualizationItem(bool isHidden);

	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual void removeChild(EntityBase *child) override;

	EntityAnnotationData *getAnnotationData(void) { EnsureAnnotationDataIsLoaded(); return annotationData; }

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void EnsureAnnotationDataIsLoaded(void);
	void storeAnnotationData(void);
	void releaseAnnotationData(void);

	EntityAnnotationData *annotationData;
	long long annotationDataStorageId;
};



