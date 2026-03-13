// @otlicense

// OpenTwin header
#include "OTGui/Event/PlotMenuRequestData.h"

static ot::MenuRequestData::Registrar<ot::PlotMenuRequestData> registrar(ot::PlotMenuRequestData::className());

ot::PlotMenuRequestData::PlotMenuRequestData()
	: MenuRequestData(WidgetViewBase::View1D)
{

}

ot::PlotMenuRequestData::PlotMenuRequestData(const BasicEntityInformation& _entityInfo)
	: MenuRequestData(_entityInfo, WidgetViewBase::View1D)
{

}

ot::PlotMenuRequestData::PlotMenuRequestData(BasicEntityInformation&& _entityInfo) noexcept
	: MenuRequestData(std::move(_entityInfo), WidgetViewBase::View1D)
{

}

ot::PlotMenuRequestData::~PlotMenuRequestData()
{
}

void ot::PlotMenuRequestData::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	MenuRequestData::addToJsonObject(_object, _allocator);
}

void ot::PlotMenuRequestData::setFromJsonObject(const ConstJsonObject& _object)
{
	MenuRequestData::setFromJsonObject(_object);
}
