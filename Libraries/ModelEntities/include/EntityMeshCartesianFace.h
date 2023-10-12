#pragma once
#pragma warning(disable : 4251)

#include "EntityContainer.h"
#include "BoundingBox.h"
#include "EntityBase.h"
#include "Types.h"

#include <list>

class __declspec(dllexport) EntityMeshCartesianFace : public EntityBase
{
public:
	enum eType {INDEX_BASED = 0, POINT_BASED = 1};

	EntityMeshCartesianFace(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityMeshCartesianFace() {};

	void setType(eType t) { type = t; }

	// Access the index based data
	void setNumberCellFaces(int direction, size_t nT);
	size_t getNumberCellFaces(int direction);
	long long getCellFace(int direction, size_t n) { assert(direction >= 0 && direction < 3); assert(n < cellFaces[direction].size()); return cellFaces[direction][n]; };
	void setCellFace(int direction, size_t n, long long face) { assert(direction >= 0 && direction < 3); assert(n < cellFaces[direction].size()); cellFaces[direction][n] = face; };

	// Access the point based data
	void setNumberCellFaces(size_t nT);
	size_t getNumberCellFaces(void);
	void setCellFace(size_t n, long long p0, long long p1, long long p2, long long p3) { assert(n < pointIndices[0].size()); pointIndices[0][n] = p0; pointIndices[1][n] = p1; pointIndices[2][n] = p2; pointIndices[3][n] = p3; };

	BoundingBox &getBoundingBox(void) { return boundingBox; };
	void setBoundingBox(const BoundingBox &box) { boundingBox = box; setModified(); };

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax);

	void setSurfaceId(int id) { surfaceId = id; setModified(); };
	int  getSurfaceId(void) { return surfaceId; };
	
	virtual std::string getClassName(void) { return "EntityMeshCartesianFace"; };

	virtual entityType getEntityType(void) override { return DATA; };

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void readCellFaces(bsoncxx::document::view &doc_view, int direction, const std::string &itemName);
	void readPoints(bsoncxx::document::view &doc_view, int index, const std::string &itemName);

	eType type;
	int surfaceId;
	std::vector<long long> cellFaces[3];
	std::vector<long long> pointIndices[4];
	BoundingBox boundingBox;
};
