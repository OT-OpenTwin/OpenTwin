#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "Types.h"
#include <vector>
#include <array>

class __declspec(dllexport) EntityParameter : public EntityBase
{
public:
	EntityParameter(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityParameter();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setValue(const std::string &value);
	std::string getValue(void);
	
	double getNumericValue(void);

	void createProperties(void);

	virtual void addVisualizationNodes(void) override;
	
	virtual std::string getClassName(void) { return "EntityParameter"; };

	void addVisualizationItem(bool isHidden);

	virtual entityType getEntityType(void) override { return TOPOLOGY; };

	virtual bool updateFromProperties(void) override;

	void addDependency(ot::UID entityID, const std::string &propertyName, const std::string& propertyGroup);
	void addDependencyByIndex(ot::UID entityID, const std::string& propertyIndex);
	void removeDependency(ot::UID entityID, const std::string &propertyName, const std::string& propertyGroup);
	void removeDependency(ot::UID entityID);
	bool hasDependency(ot::UID entityID);
	void removeAllDependencies(void);

	std::map<ot::UID, std::map<std::string, bool>> getDependencies(void) { return dependencyMap; }

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::map<ot::UID, std::map<std::string, bool>> dependencyMap;
};



