#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "OldTreeIcon.h"

#include <list>
#include <vector>

class __declspec(dllexport) EntityBinaryData : public EntityBase
{
public:
	EntityBinaryData() : EntityBinaryData(0, nullptr, nullptr, nullptr, "") {};
	EntityBinaryData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityBinaryData();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	static std::string className() { return "EntityBinaryData"; };
	virtual std::string getClassName(void) override { return EntityBinaryData::className(); };

	virtual entityType getEntityType(void) const override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

	void setData(const char *dat, size_t length);

	void clearData(void);

	const std::vector<char>& getData(void) const;

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	
	std::vector<char> data;
};



