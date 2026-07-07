// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTGui/Painter/FillPainter2D.h"
#include "OTGui/Painter/Painter2DFactory.h"
#include "OTGui/Properties/PropertyPainter2D.h"
#include "OTModelEntities/TemplateDefaultManager.h"
#include "OTModelEntities/Properties/EntityProperties.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesGuiPainter.h"

EntityPropertiesGuiPainter::EntityPropertiesGuiPainter()
{
	m_painter = new ot::FillPainter2D;
}

EntityPropertiesGuiPainter::~EntityPropertiesGuiPainter()
{
	delete m_painter;
	m_painter = nullptr;
}

EntityPropertiesGuiPainter::EntityPropertiesGuiPainter(const EntityPropertiesGuiPainter& other)
	: EntityPropertiesBase(other), m_painter(nullptr)
{
	m_painter = other.getValue()->createCopy();
	m_filter = other.m_filter;
}

EntityPropertiesGuiPainter& EntityPropertiesGuiPainter::operator=(const EntityPropertiesGuiPainter& other)
{
	if (&other != this)
	{
		EntityPropertiesBase::operator=(other);
		setValue(other.getValue()->createCopy());
		m_filter = other.m_filter;
	}

	return *this;
}

void EntityPropertiesGuiPainter::setValue(const ot::Painter2D* _painter)
{
	if (m_painter == _painter)
	{
		return;
	}
	OTAssertNullptr(_painter);
	OTAssertNullptr(m_painter);
	if (m_painter->isEqualTo(_painter))
	{
		return;
	}

	delete m_painter;
	m_painter = _painter->createCopy();
	this->setNeedsUpdate();
}

EntityPropertiesGuiPainter* EntityPropertiesGuiPainter::createProperty(const std::string& group, const std::string& name, ot::Painter2D* _defaultPainter, const std::string& defaultCategory, EntityProperties& properties)
{
	// Load the template defaults if any
	TemplateDefaultManager::getTemplateDefaultManager()->loadDefaults(defaultCategory);

	// Now load the default value if available. Otherwise take the provided default
	const ot::Painter2D* newPainter = TemplateDefaultManager::getTemplateDefaultManager()->getDefaultGuiPainter(defaultCategory, name);

	if (newPainter)
	{
		if (_defaultPainter) delete _defaultPainter;
		_defaultPainter = newPainter->createCopy();
	}

	// Finally create the new property
	EntityPropertiesGuiPainter* newProperty = new EntityPropertiesGuiPainter(name, _defaultPainter);
	properties.createProperty(newProperty, group);
	return newProperty;
}

void EntityPropertiesGuiPainter::addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root)
{
	OTAssertNullptr(m_painter);
	ot::PropertyPainter2D* newProp = new ot::PropertyPainter2D(this->getName(), m_painter->createCopy());
	newProp->setFilter(m_filter);
	this->setupPropertyData(_configuration, newProp);
}

void EntityPropertiesGuiPainter::setFromConfiguration(const ot::Property* _property, EntityBase* _root)
{
	EntityPropertiesBase::setFromConfiguration(_property, _root);
	const ot::PropertyPainter2D* actualProperty = dynamic_cast<const ot::PropertyPainter2D*>(_property);
	if (!actualProperty)
	{
		OT_LOG_E("Property cast failed");
		return;
	}

	m_filter = actualProperty->getFilter();
	this->setValue(actualProperty->getPainter()->createCopy());
}

void EntityPropertiesGuiPainter::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root)
{
	EntityPropertiesBase::addToJsonObject(_jsonObject, _allocator, _root);

	_jsonObject.AddMember("Value", ot::JsonObject(m_painter, _allocator), _allocator);
	_jsonObject.AddMember("Filter", ot::JsonObject(m_filter, _allocator), _allocator);
}

void EntityPropertiesGuiPainter::readFromJsonObject(const ot::ConstJsonObject& _object, EntityBase* _root)
{
	EntityPropertiesBase::readFromJsonObject(_object, _root);

	ot::Painter2D* newPainter = ot::Painter2DFactory::create(ot::json::getObject(_object, "Value"));
	if (!newPainter)
	{
		return;
	}

	if (_object.HasMember("Filter"))
	{
		m_filter.setFromJsonObject(ot::json::getObject(_object, "Filter"));
	}

	setValue(newPainter);
}

void EntityPropertiesGuiPainter::copySettings(EntityPropertiesBase* other, EntityBase* root)
{
	EntityPropertiesBase::copySettings(other, root);

	EntityPropertiesGuiPainter* entity = dynamic_cast<EntityPropertiesGuiPainter*>(other);
	assert(entity != nullptr);

	if (entity != nullptr)
	{
		setValue(entity->getValue()->createCopy());
		m_filter = entity->m_filter;
	}
}

bool EntityPropertiesGuiPainter::hasSameValue(EntityPropertiesBase* other) const
{
	EntityPropertiesGuiPainter* entity = dynamic_cast<EntityPropertiesGuiPainter*>(other);

	if (entity == nullptr) return false;

	return (getValue()->isEqualTo(entity->getValue()));
}
