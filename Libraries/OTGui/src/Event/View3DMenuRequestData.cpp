// @otlicense

// OpenTwin header
#include "OTGui/Event/View3DMenuRequestData.h"

static ot::MenuRequestData::Registrar<ot::View3DMenuRequestData> registrar(ot::View3DMenuRequestData::className());

ot::View3DMenuRequestData::View3DMenuRequestData()
	: MenuRequestData(WidgetViewBase::View3D)
{

}

ot::View3DMenuRequestData::View3DMenuRequestData(const BasicEntityInformation& _entityInfo)
	: MenuRequestData(_entityInfo, WidgetViewBase::View3D)
{

}

ot::View3DMenuRequestData::View3DMenuRequestData(BasicEntityInformation&& _entityInfo) noexcept
	: MenuRequestData(std::move(_entityInfo), WidgetViewBase::View3D)
{

}

ot::View3DMenuRequestData::~View3DMenuRequestData()
{
}

void ot::View3DMenuRequestData::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	MenuRequestData::addToJsonObject(_object, _allocator);
}

void ot::View3DMenuRequestData::setFromJsonObject(const ConstJsonObject& _object)
{
	MenuRequestData::setFromJsonObject(_object);
}
