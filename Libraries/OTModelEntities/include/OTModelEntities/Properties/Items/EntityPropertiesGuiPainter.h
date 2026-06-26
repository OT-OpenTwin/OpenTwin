// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Dialog/Painter2DDialogFilter.h"
#include "OTModelEntities/Properties/Items/EntityPropertiesBase.h"

namespace ot
{
	class Painter2D;
}

class OT_MODELENTITIES_API_EXPORT EntityPropertiesGuiPainter : public EntityPropertiesBase
{
public:
	static std::string typeString() { return "guipainter"; };

	EntityPropertiesGuiPainter();
	virtual ~EntityPropertiesGuiPainter();

	//! \brief Constructor.
	//! The entity takes ownership of the painter.
	EntityPropertiesGuiPainter(const std::string& n, ot::Painter2D* _painter) : m_painter(_painter) { setName(n); };

	EntityPropertiesGuiPainter(const EntityPropertiesGuiPainter& other);

	virtual EntityPropertiesBase* createCopy() const override { return new EntityPropertiesGuiPainter(*this); };

	EntityPropertiesGuiPainter& operator=(const EntityPropertiesGuiPainter& other);

	virtual eType getType() const override { return GUIPAINTER; };
	virtual std::string getTypeString() const override { return EntityPropertiesGuiPainter::typeString(); };

	void setValue(const ot::Painter2D* _painter);

	//! \brief Returns the current painter.
	//! The entity keeps ownership of the painter
	const ot::Painter2D* getValue() const { return m_painter; };

	void setFilter(const ot::Painter2DDialogFilter& _filter) { m_filter = _filter; };
	const ot::Painter2DDialogFilter& getFilter() const { return m_filter; };

	virtual bool hasSameValue(EntityPropertiesBase* other) const override;

	virtual void addToConfiguration(ot::PropertyGridCfg& _configuration, EntityBase* root) override;
	virtual void setFromConfiguration(const ot::Property* _property, EntityBase* root) override;

	virtual void addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator, EntityBase* _root) override;
	virtual void readFromJsonObject(const ot::ConstJsonObject& object, EntityBase* root) override;

	virtual void copySettings(EntityPropertiesBase* other, EntityBase* root);

	static EntityPropertiesGuiPainter* createProperty(const std::string& group, const std::string& name, ot::Painter2D* _defaultPainter, const std::string& defaultCategory, EntityProperties& properties);

private:
	ot::Painter2D* m_painter;
	ot::Painter2DDialogFilter m_filter;
};
