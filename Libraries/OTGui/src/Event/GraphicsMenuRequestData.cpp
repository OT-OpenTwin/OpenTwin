// @otlicense

// OpenTwin header
#include "OTGui/Event/GraphicsMenuRequestData.h"

static ot::MenuRequestData::Registrar<ot::GraphicsMenuRequestData> registrar(ot::GraphicsMenuRequestData::className());

ot::GraphicsMenuRequestData::GraphicsMenuRequestData()
	: MenuRequestData(WidgetViewBase::ViewGraphics)
{

}

ot::GraphicsMenuRequestData::GraphicsMenuRequestData(const BasicEntityInformation& _entityInfo)
	: MenuRequestData(_entityInfo, WidgetViewBase::ViewGraphics)
{

}

ot::GraphicsMenuRequestData::GraphicsMenuRequestData(BasicEntityInformation&& _entityInfo) noexcept
	: MenuRequestData(std::move(_entityInfo), WidgetViewBase::ViewGraphics)
{

}

ot::GraphicsMenuRequestData::~GraphicsMenuRequestData()
{
}

void ot::GraphicsMenuRequestData::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	MenuRequestData::addToJsonObject(_object, _allocator);
}

void ot::GraphicsMenuRequestData::setFromJsonObject(const ConstJsonObject& _object)
{
	MenuRequestData::setFromJsonObject(_object);
}
