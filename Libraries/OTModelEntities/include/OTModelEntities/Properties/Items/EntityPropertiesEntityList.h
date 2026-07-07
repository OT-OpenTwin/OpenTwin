// @otlicense

#pragma once

// OpenTwin header
#include "OTModelEntities/Properties/Items/EntityPropertiesBase.h"

class EntityContainer;

class OT_MODELENTITIES_API_EXPORT EntityPropertiesEntityList : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "entitylist"; };

	EntityPropertiesEntityList() : m_entityContainerID(0), m_valueID(0), m_includeRoot(false), m_recursive(false) {};

	EntityPropertiesEntityList(const EntityPropertiesEntityList& other) : EntityPropertiesBase(other) { m_entityContainerName = other.getEntityContainerName(); m_entityContainerID = other.getEntityContainerID(); m_valueName = other.getValueName(); m_valueID = other.getValueID(); m_filter = other.getFilter(); m_includeRoot = other.getIncludeRoot(); m_recursive = other.getRecursive(); };
	EntityPropertiesEntityList(const std::string& n, const std::string& contName, ot::UID contID, const std::string& valName, ot::UID valID) : m_entityContainerName(contName), m_entityContainerID(contID), m_valueName(valName), m_valueID(valID), m_includeRoot(false), m_recursive(false) { setName(n); };

	virtual ~EntityPropertiesEntityList() {};

	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesEntityList(*this); };

	EntityPropertiesEntityList& operator=(const EntityPropertiesEntityList& other);

	virtual eType getType() const override { return ENTITYLIST; };
	virtual std::string getTypeString() const override { return EntityPropertiesEntityList::typeString(); };

	virtual bool hasSameValue(EntityPropertiesBase* other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

	void setEntityContainerName(const std::string& containerName) { if (m_entityContainerName != containerName) setNeedsUpdate(); m_entityContainerName = containerName; };
	void setEntityContainerID(ot::UID containerID) { if (m_entityContainerID != containerID) setNeedsUpdate(); m_entityContainerID = containerID; };

	void setValueName(const std::string& vname) { if (m_valueName != vname) setNeedsUpdate(); m_valueName = vname; };
	void setValueID(ot::UID vid) { if (m_valueID != vid) setNeedsUpdate(); m_valueID = vid; };

	std::string getEntityContainerName() const { return m_entityContainerName; };
	ot::UID getEntityContainerID() const { return m_entityContainerID; };

	std::string getValueName() const { return m_valueName; };
	ot::UID getValueID() const { return m_valueID; };

	void setFilter(const std::string& _filter) { m_filter = _filter; }
	std::string getFilter() const { return m_filter; }

	void setIncludeRoot(bool flag) { m_includeRoot = flag; }
	bool getIncludeRoot() const { return m_includeRoot; }

	void setRecursive(bool flag) { m_recursive = flag; }
	bool getRecursive() const { return m_recursive; }

	static EntityPropertiesEntityList* createProperty(const std::string& group, const std::string& name, const std::string& contName, ot::UID contID, const std::string& valName, ot::UID valID, const std::string& defaultCategory, EntityProperties& properties);

protected:

	//! \brief Will update the ValueName, ValueID, ContainerName and ContainerID.
	//! This method will check if the value and container IDs are correct and set the names respectivly.
	//! If the IDs are incorrect the IDs will be determined by the name.
	void updateValueAndContainer(EntityBase* _root, std::list<std::string>& _containerOptions);

	EntityContainer* findContainerFromID(EntityBase* root, ot::UID entityID);
	EntityContainer* findContainerFromName(EntityBase* root, const std::string& entityName);
	EntityBase* findEntityFromName(EntityBase* root, const std::string& entityName);
	EntityBase* findEntityFromID(EntityBase* root, ot::UID entityID);

	void appendItemToList(EntityBase* item, const std::string& filter, bool recursive, std::list<std::string>& _containerOptions);

private:
	std::string m_entityContainerName;
	ot::UID m_entityContainerID;

	std::string m_valueName;
	ot::UID m_valueID;
	std::string m_filter;
	bool m_includeRoot;
	bool m_recursive;
};
