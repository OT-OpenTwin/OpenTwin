#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "Geometry.h"
#include "BoundingBox.h"
#include "Types.h"

class EntityMeshCartesianData;

#include <list>

//! The Entity class is the base class for model entities and provides basic access properties to model entities. 
class __declspec(dllexport) EntityMeshCartesianItem : public EntityBase
{
public:
	EntityMeshCartesianItem(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityMeshCartesianItem();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setMesh(EntityMeshCartesianData *m) { mesh = m; };

	void setNumberFaces(size_t nT) { faces.resize(nT); setModified(); };

	int getFaceId(size_t nF) { return faces[nF]; };
	void setFace(size_t nF, int faceId) { faces[nF] = faceId; setModified(); };

	bool getFaceOrientation(size_t nF) { return getFaceId(nF) > 0; };
 
	//EntityMeshTetFace *getFace(size_t nF);

	virtual void StoreToDataBase(void) override;

	virtual void addVisualizationNodes(void) override;

	void addVisualizationItem(bool isHidden);

	void setColor(double r, double g, double b) { colorRGB[0] = r; colorRGB[1] = g; colorRGB[2] = b; };

	virtual std::string getClassName(void) override { return "EntityMeshCartesianItem"; };

	virtual entityType getEntityType(void) override { return TOPOLOGY; };

	BoundingBox &getBoundingBox(void) { return boundingBox; }

private:

	virtual int getSchemaVersion(void) override { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	EntityMeshCartesianData *mesh;

	double colorRGB[3];

	std::vector<int> faces;

	BoundingBox boundingBox;
};



