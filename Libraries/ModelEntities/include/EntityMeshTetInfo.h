#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "Types.h"

#include <list>

class __declspec(dllexport) EntityMeshTetInfo : public EntityBase
{
public:
	EntityMeshTetInfo(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);
	virtual ~EntityMeshTetInfo();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual std::string getClassName(void) { return "EntityMeshTetInfo"; };

	virtual entityType getEntityType(void) override { return DATA; };

	void addGeneralData(ot::UID dataID, ot::UID dataVersion) { generalData.push_back(std::pair<ot::UID, ot::UID>(dataID, dataVersion)); };
	void addFacesData(ot::UID dataID, ot::UID dataVersion) { facesData.push_back(std::pair<ot::UID, ot::UID>(dataID, dataVersion)); };
	void addVolumeData(ot::UID dataID, ot::UID dataVersion) { volumeData.push_back(std::pair<ot::UID, ot::UID>(dataID, dataVersion)); };

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::list<std::pair<ot::UID, ot::UID>> generalData;
	std::list<std::pair<ot::UID, ot::UID>> facesData;
	std::list<std::pair<ot::UID, ot::UID>> volumeData;
};



