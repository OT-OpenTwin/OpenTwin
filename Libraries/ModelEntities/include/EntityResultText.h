#pragma once
#pragma warning(disable : 4251)

#include "EntityContainer.h"
#include "Types.h"

class EntityResultTextData;

class __declspec(dllexport) EntityResultText : public EntityContainer
{
public:
	EntityResultText(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityResultText();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual void StoreToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	void addVisualizationItem(bool isHidden);
	
	virtual std::string getClassName(void) { return "EntityResultText"; };

	virtual entityType getEntityType(void) override { return TOPOLOGY; };
	virtual void removeChild(EntityBase *child) override;

	void createProperties(void);

	virtual bool updateFromProperties(void) override;

	void deleteTextData(void);

	void storeTextData(void);
	void releaseTextData(void);

	long long getTextDataStorageId(void) { return textDataStorageId; }
	long long getTextDataStorageVersion(void) { return textDataStorageVersion; }

	void setText(const std::string &text);
	std::string &getText(void);

private:
	void EnsureTextDataLoaded(void);
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	EntityResultTextData *getTextData(void);

	EntityResultTextData *textData;
	long long textDataStorageId;
	long long textDataStorageVersion;
};



