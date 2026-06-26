// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Properties/Property.h"
#include "OTGui/Properties/PropertyGroup.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesBase.h"

EntityPropertiesBase::EntityPropertiesBase()
	: m_container(nullptr), m_needsUpdateFlag(false), m_multipleValues(false), m_readOnly(false), m_protectedProperty(true),
	m_visible(true), m_errorState(false), m_groupChanges(false)
{}

EntityPropertiesBase::EntityPropertiesBase(const EntityPropertiesBase& other)
{
	m_name = other.m_name;
	m_group = other.m_group;

	m_multipleValues = other.m_multipleValues;
	m_container = other.m_container;
	m_needsUpdateFlag = other.m_needsUpdateFlag;
	m_readOnly = other.m_readOnly;
	m_protectedProperty = other.m_protectedProperty;
	m_visible = other.m_visible;
	m_errorState = other.m_errorState;
	m_toolTip = other.m_toolTip;
	m_groupChanges = other.m_groupChanges;
}

void EntityPropertiesBase::setNeedsUpdate()
{
	if (m_container != nullptr) m_container->setNeedsUpdate();

	m_needsUpdateFlag = true;
}

void EntityPropertiesBase::setFromConfiguration(const ot::Property* _property, EntityBase* root)
{
	this->setName(_property->getPropertyName());
	this->setHasMultipleValues(_property->getPropertyFlags().has(ot::Property::HasMultipleValues));
	this->setReadOnly(_property->getPropertyFlags().has(ot::Property::IsReadOnly));
	this->setProtected(!_property->getPropertyFlags().has(ot::Property::IsDeletable));
	this->setVisible(!_property->getPropertyFlags().has(ot::Property::IsHidden));
	this->setErrorState(_property->getPropertyFlags().has(ot::Property::HasInputError));
	this->setToolTip(_property->getPropertyTip());
	this->setGroupChanges(_property->getPropertyFlags().has(ot::Property::GroupChanges));
}

void EntityPropertiesBase::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* root)
{
	if (getCurrentValueHandlingType() != ot::PropertyBase::ValueHandlingType::Value)
	{
		OTAssert(0, "Invalid property value handling while storing property to database");
		OT_LOG_ES("Invalid property value handling while storing property to database { \"PropertyName\": \"" << getName() << "\" }");
	}

	_jsonObject.AddMember("Type", ot::JsonString(this->getTypeString(), _allocator), _allocator);
	_jsonObject.AddMember("MultipleValues", this->hasMultipleValues(), _allocator);
	_jsonObject.AddMember("ReadOnly", this->getReadOnly(), _allocator);
	_jsonObject.AddMember("Protected", this->getProtected(), _allocator);
	_jsonObject.AddMember("Visible", this->getVisible(), _allocator);
	_jsonObject.AddMember("ErrorState", this->getErrorState(), _allocator);
	_jsonObject.AddMember("Group", ot::JsonString(this->getGroup(), _allocator), _allocator);
	_jsonObject.AddMember("ToolTip", ot::JsonString(this->getToolTip(), _allocator), _allocator);
	_jsonObject.AddMember("GroupChanges", this->getGroupChanges(), _allocator);
}

void EntityPropertiesBase::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	if (_object.HasMember("MultipleValues"))
	{
		this->setHasMultipleValues(ot::json::getBool(_object, "MultipleValues"));
	}
	if (_object.HasMember("ReadOnly"))
	{
		this->setReadOnly(ot::json::getBool(_object, "ReadOnly"));
	}
	if (_object.HasMember("Protected"))
	{
		this->setProtected(ot::json::getBool(_object, "Protected"));
	}
	if (_object.HasMember("Visible"))
	{
		this->setVisible(ot::json::getBool(_object, "Visible"));
	}
	if (_object.HasMember("ErrorState"))
	{
		this->setErrorState(ot::json::getBool(_object, "ErrorState"));
	}
	if (_object.HasMember("ToolTip"))
	{
		this->setToolTip(ot::json::getString(_object, "ToolTip"));
	}
	if (_object.HasMember("GroupChanges"))
	{
		this->setGroupChanges(ot::json::getBool(_object, "GroupChanges"));
	}
}

void EntityPropertiesBase::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	assert(m_name == other->getName());

	// We keep the container unchanged, since we don't want to overwrite the ownership of this item
	// We also dont assign the group, since the group assignment of a property can not be changed
	m_multipleValues = other->m_multipleValues;
	m_needsUpdateFlag = other->m_needsUpdateFlag;
	m_readOnly = other->m_readOnly;
	m_protectedProperty = other->m_protectedProperty;
	m_visible = other->m_visible;
	m_errorState = other->m_errorState;
	m_toolTip = other->m_toolTip;
	m_groupChanges = other->m_groupChanges;
}

EntityPropertiesBase& EntityPropertiesBase::operator=(const EntityPropertiesBase& other)
{
	if (&other != this)
	{
		// We keep the container unchanged, since we don't want to overwrite the ownership of this item
		m_name = other.m_name;
		m_group = other.m_group;
		m_multipleValues = other.m_multipleValues;
		m_needsUpdateFlag = other.m_needsUpdateFlag;
		m_readOnly = other.m_readOnly;
		m_protectedProperty = other.m_protectedProperty;
		m_visible = other.m_visible;
		m_errorState = other.m_errorState;
		m_toolTip = other.m_toolTip;
		m_groupChanges = other.m_groupChanges;
	}

	return *this;
}

void EntityPropertiesBase::setupPropertyData(ot::PropertyGridCfg& _configuration, ot::Property* _property) const
{
	if (this->hasMultipleValues()) _property->setPropertyFlag(ot::Property::HasMultipleValues);
	if (this->getReadOnly()) _property->setPropertyFlag(ot::Property::IsReadOnly);
	if (!this->getProtected()) _property->setPropertyFlag(ot::Property::IsDeletable);
	if (!this->getVisible()) _property->setPropertyFlag(ot::Property::IsHidden);
	if (this->getErrorState()) _property->setPropertyFlag(ot::Property::HasInputError);
	if (this->getGroupChanges()) _property->setPropertyFlag(ot::Property::GroupChanges);

	_property->setPropertyTip(m_toolTip);

	ot::PropertyGroup* group = _configuration.findOrCreateGroup(this->getGroup());
	OTAssertNullptr(group);
	group->addProperty(_property);
}

bool EntityPropertiesBase::needsUpdate() const
{
	return m_needsUpdateFlag;
}
