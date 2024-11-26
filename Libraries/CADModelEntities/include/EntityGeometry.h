#pragma once
#pragma warning(disable : 4251)

#include "EntityContainer.h"
#include "EntityBrep.h"
#include "EntityFacetData.h"
#include "OldTreeIcon.h"

#include <string>

#include "TopoDS_Shape.hxx"

//! The Entity class is the base class for model entities and provides basic access properties to model entities. 
class __declspec(dllexport) EntityGeometry : public EntityContainer
{
public:
	EntityGeometry(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityGeometry();

	void		 setBrep(TopoDS_Shape shape);
	TopoDS_Shape &getBrep(void);

	EntityFacetData* getFacets(void);
	EntityBrep* getBrepEntity(void);

	long long getBrepStorageObjectID(void) { assert(brepStorageID != -1);  return brepStorageID; }
	long long getFacetsStorageObjectID(void) { assert(facetsStorageID != -1);  return facetsStorageID; }

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual bool updateFromProperties(void) override;

	virtual void StoreToDataBase(void) override;

	virtual void addVisualizationNodes(void) override;

	void releaseBrep(void);
	void releaseFacets(void);

	void resetBrep(void);
	void resetFacets(void);

	virtual std::string getClassName(void) override  { return "EntityGeometry"; };

	void addPrefetchingRequirementsForBrep(std::list<ot::UID> &prefetchIds);
	void addPrefetchingRequirementsForFacets(std::list<ot::UID> &prefetchIds);

	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual void removeChild(EntityBase *child) override;

	long long getBrepPrefetchID(void);
	long long getFacetsPrefetchID(void);

	void createProperties(int colorR, int colorG, int colorB, const std::string materialsFolder, ot::UID materialsFolderID);
	void createMaterialPropertiesOnly(int colorR, int colorG, int colorB, const std::string materialsFolder, ot::UID materialsFolderID);
	void createNonMaterialProperties(void);

	void facetEntity(bool isHidden);

	void setData(void *data) { tmpData = data; }
	void *getData(void) { return tmpData; }

	void replaceBrepStorageID(ot::UID id) { assert(brep == nullptr);  brepStorageID = id; setModified(); }
	void replaceFacetsStorageID(ot::UID id) { assert(facets == nullptr);  facetsStorageID = id; setModified(); }

	double evaluateExpressionDouble(const std::string &expr);

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	void setOldTreeIcons(const std::string &visible, const std::string &hidden) { treeIcons.visibleIcon = visible; treeIcons.hiddenIcon = hidden; }
	void getOldTreeIcons(std::string &visible, std::string &hidden) { visible = treeIcons.visibleIcon; hidden = treeIcons.hiddenIcon; }

	bool isTextureReflective(const std::string textureType);

	void storeBrep(void);
	void detachBrep(void);

	void storeFacets(void);

	void setShowWhenSelected(bool flag) { showWhenSelected = flag; }
	bool getShowWhenSelected(void) { return showWhenSelected; }

	void resetTransformation(void);
	void getTransformation(std::vector<double> &transformation);
	void setTransformation(const std::vector<double> &transformation);

	void setFacetEntity(EntityFacetData *entity) { assert(facets == nullptr); facets = entity; }
	void setBrepEntity(EntityBrep *entity) { assert(brep == nullptr); brep = entity; }

private:
	virtual int getSchemaVersion(void) override  { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void EnsureBrepIsLoaded(void);
	void EnsureFacetsAreLoaded(void);
	bool updatePropertyVisibilities(void);
	void updateDimensionProperties(void);
	bool isTransformProperty(const std::string &propName);
	void createTransformProperty(const std::string &propName, double value);

	// Persistent data
	EntityBrep *brep;
	EntityFacetData *facets;

	long long brepStorageID;
	long long facetsStorageID;

	OldTreeIcon treeIcons;

	bool showWhenSelected;

	// Temporary raw pointer which can be used for some algorithms temporarily (we do not take care of this data at all)
	void *tmpData;
};



