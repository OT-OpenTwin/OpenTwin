// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/JSON/JSON.h"
#include "OTGui/Properties/PropertyGridCfg.h"
#include "OTModelEntities/ModelEntitiesAPIExport.h"

// std header
#include <string>

#pragma warning(disable : 4251)

class EntityBase;
class EntityProperties;

namespace ot
{
	class Property;
	class PropertyGridCfg;
}

class OT_MODELENTITIES_API_EXPORT EntityPropertiesBase
{
public:
	EntityPropertiesBase();
	virtual ~EntityPropertiesBase() {};

	EntityPropertiesBase(const EntityPropertiesBase& other);

	virtual EntityPropertiesBase* createCopy() const = 0;

	void setContainer(EntityProperties* c) { m_container = c; };

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName() const { return m_name; };

	void setGroup(const std::string& _group) { m_group = _group; };
	const std::string& getGroup() const { return m_group; };

	enum eType { DOUBLE, INTEGER, BOOLEAN, STRING, SELECTION, COLOR, ENTITYLIST, PROJECTLIST, GUIPAINTER, EXTENDEDENTITYLIST };
	virtual eType getType() const = 0;
	virtual std::string getTypeString() const = 0;

	void resetNeedsUpdate() { m_needsUpdateFlag = false; };
	bool needsUpdate() const;
	void setNeedsUpdate();

	void setHasMultipleValues(bool b) { m_multipleValues = b; };
	bool hasMultipleValues() const { return m_multipleValues; };

	virtual bool isCompatible(EntityPropertiesBase* other) const { return true; };
	virtual bool hasSameValue(EntityPropertiesBase* other) const = 0;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) = 0;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root);

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* root);
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root);

	void setToolTip(const std::string& _toolTip) { m_toolTip = _toolTip; };
	const std::string& getToolTip() const { return m_toolTip; };

	void setReadOnly(bool _flag) { m_readOnly = _flag; };
	bool getReadOnly() const { return m_readOnly; };

	void setProtected(bool _flag) { m_protectedProperty = _flag; };
	bool getProtected() const { return m_protectedProperty; };

	void setVisible(bool _flag) { m_visible = _flag; };
	bool getVisible() const { return m_visible; };

	void setErrorState(bool _flag) { m_errorState = _flag; };
	bool getErrorState() const { return m_errorState; };

	void setGroupChanges(bool _flag) { m_groupChanges = _flag; };
	bool getGroupChanges() const { return m_groupChanges; };

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

	virtual ot::PropertyBase::ValueHandlingType getCurrentValueHandlingType() const { return ot::PropertyBase::ValueHandlingType::Value; };

	EntityPropertiesBase& operator=(const EntityPropertiesBase& other);

protected:
	void setupPropertyData(ot::PropertyGridCfg& _configuration, ot::Property* _property) const;

private:
	void setMultipleValues() { m_multipleValues = true; }

	EntityProperties* m_container;
	bool m_needsUpdateFlag;
	std::string m_name;
	std::string m_group;
	std::string m_toolTip;
	bool m_multipleValues;
	bool m_readOnly;
	bool m_protectedProperty;
	bool m_visible;
	bool m_errorState;
	bool m_groupChanges;
};
