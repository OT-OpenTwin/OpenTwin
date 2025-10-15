#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "OldTreeIcon.h"

class __declspec(dllexport) EntityMaterial : public EntityBase
{
public:
	EntityMaterial() : EntityMaterial(0, nullptr, nullptr, nullptr, "") {};
	EntityMaterial(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, const std::string &owner);
	virtual ~EntityMaterial();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual void storeToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	
	static std::string className() { return "EntityMaterial"; };
	virtual std::string getClassName(void) override { return EntityMaterial::className(); };

	void addVisualizationItem(bool isHidden);

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void removeChild(EntityBase *child) override;

	void createProperties(void);

	virtual bool updateFromProperties(void) override;

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};



