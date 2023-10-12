#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "Types.h"

#include <list>

class __declspec(dllexport) EntityMeshCartesianNodes : public EntityBase
{
public:
	EntityMeshCartesianNodes(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityMeshCartesianNodes();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	double getNodeCoordX(size_t node) { assert(node < nodeCoordsX.size()); return nodeCoordsX[node]; };
	double getNodeCoordY(size_t node) { assert(node < nodeCoordsY.size()); return nodeCoordsY[node]; };
	double getNodeCoordZ(size_t node) { assert(node < nodeCoordsZ.size()); return nodeCoordsZ[node]; };
	void   setNodeCoordX(size_t node, double x) { assert(node < nodeCoordsX.size()); nodeCoordsX[node] = x; setModified(); };
	void   setNodeCoordY(size_t node, double y) { assert(node < nodeCoordsY.size()); nodeCoordsY[node] = y; setModified(); };
	void   setNodeCoordZ(size_t node, double z) { assert(node < nodeCoordsZ.size()); nodeCoordsZ[node] = z; setModified(); };

	void setNumberOfNodes(size_t n);
	size_t getNumberOfNodes(void) { return nodeCoordsX.size(); }

	virtual std::string getClassName(void) { return "EntityMeshCartesianNodes"; };

	virtual entityType getEntityType(void) override { return DATA; };

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::vector<double> nodeCoordsX;
	std::vector<double> nodeCoordsY;
	std::vector<double> nodeCoordsZ;
};



