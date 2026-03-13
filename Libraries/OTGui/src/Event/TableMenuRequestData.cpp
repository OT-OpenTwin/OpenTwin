// @otlicense

// OpenTwin header
#include "OTGui/Event/TableMenuRequestData.h"

static ot::MenuRequestData::Registrar<ot::TableMenuRequestData> registrar(ot::TableMenuRequestData::className());

ot::TableMenuRequestData::TableMenuRequestData()
	: MenuRequestData(WidgetViewBase::ViewTable)
{

}

ot::TableMenuRequestData::TableMenuRequestData(const BasicEntityInformation& _entityInfo)
	: MenuRequestData(_entityInfo, WidgetViewBase::ViewTable)
{

}

ot::TableMenuRequestData::TableMenuRequestData(BasicEntityInformation&& _entityInfo) noexcept
	: MenuRequestData(std::move(_entityInfo), WidgetViewBase::ViewTable)
{

}

ot::TableMenuRequestData::~TableMenuRequestData()
{
}

void ot::TableMenuRequestData::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	MenuRequestData::addToJsonObject(_object, _allocator);
}

void ot::TableMenuRequestData::setFromJsonObject(const ConstJsonObject& _object)
{
	MenuRequestData::setFromJsonObject(_object);
}
