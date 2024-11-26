#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "OldTreeIcon.h"

#include <list>
#include <vector>

class __declspec(dllexport) EntityResultTextData : public EntityBase
{
public:
	EntityResultTextData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityResultTextData();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual void StoreToDataBase(void) override;

	virtual std::string getClassName(void) { return "EntityResultTextData"; };

	virtual entityType getEntityType(void) override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

	void setText(const std::string &text);
	std::string &getText(void);

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	
	std::string textData;
};



