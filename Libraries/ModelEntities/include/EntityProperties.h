#pragma once
#pragma warning(disable : 4251)

#include <string>
#include <map>

#include "EntityPropertiesItems.h"

#include "OTGui/PropertyGridCfg.h"

class EntityBase;

namespace ot { class PropertyGroup; };

class __declspec(dllexport) EntityProperties
{
public:
	EntityProperties() : needsUpdate(false) {};
	EntityProperties(const EntityProperties &other);
	virtual ~EntityProperties();

	void merge(EntityProperties& other);

	bool createProperty(EntityPropertiesBase *property, const std::string &group);
	bool updateProperty(EntityPropertiesBase *property, const std::string &group);
	bool deleteProperty(const std::string &name);
	bool propertyExists(const std::string &name);

	EntityPropertiesBase *getProperty(const std::string &name);

	void setNeedsUpdate(void) { needsUpdate = true; };
	bool anyPropertyNeedsUpdate(void) {return needsUpdate; };
	void checkWhetherUpdateNecessary(void);
	void forceResetUpdateForAllProperties();

	void addToConfiguration(EntityBase *root, bool visibleOnly, ot::PropertyGridCfg& _config);
	void buildFromConfiguration(const ot::PropertyGridCfg& _config);
	void buildFromConfiguration(const ot::PropertyGroup* _groupConfig);
	void checkMatchingProperties(EntityProperties &other);
	void readFromProperties(const EntityProperties &other, EntityBase *root);

	EntityProperties& operator=(const EntityProperties &other);

	void setAllPropertiesReadOnly(void);
	void setAllPropertiesNonProtected(void);

	std::list<EntityPropertiesBase*> getListOfAllProperties(void);
	std::list<EntityPropertiesBase *> getListOfPropertiesWhichNeedUpdate(void);
	std::list<EntityPropertiesDouble *> getListOfNumericalProperties(void);
	size_t getNumberOfProperties(void) { return properties.size(); }

	std::list<std::string> getListOfPropertiesForGroup(const std::string &group);

private:
	void deleteAllProperties(void);

	bool needsUpdate;
	std::map<std::string, EntityPropertiesBase *> properties;
	std::list<EntityPropertiesBase *> propertiesList; // The storage in the list is redundant, but allows to keep the order of the properties
};
