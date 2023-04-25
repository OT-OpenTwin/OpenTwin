#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "Types.h"

class __declspec(dllexport) EntityMaterial : public EntityBase
{
public:
	EntityMaterial(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, ClassFactory *factory, const std::string &owner);
	virtual ~EntityMaterial();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual void StoreToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	
	virtual std::string getClassName(void) { return "EntityMaterial"; };

	void addVisualizationItem(bool isHidden);

	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual void removeChild(EntityBase *child) override;

	void createProperties(void);

	virtual bool updateFromProperties(void) override;

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};



