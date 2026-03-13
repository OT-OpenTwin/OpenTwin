// @otlicense

// OpenTwin header
#include "OTGui/Event/NavigationMenuRequestData.h"

static ot::MenuRequestData::Registrar<ot::NavigationMenuRequestData> registrar(ot::NavigationMenuRequestData::className());

ot::NavigationMenuRequestData::NavigationMenuRequestData()
	: MenuRequestData(WidgetViewBase::ViewNavigation)
{}

ot::NavigationMenuRequestData::NavigationMenuRequestData(const BasicEntityInformation & _basicEntityInformation)
	: MenuRequestData(_basicEntityInformation, WidgetViewBase::ViewNavigation)
{}

ot::NavigationMenuRequestData::NavigationMenuRequestData(BasicEntityInformation&& _basicEntityInformation) noexcept
	: MenuRequestData(std::move(_basicEntityInformation), WidgetViewBase::ViewNavigation)
{}

void ot::NavigationMenuRequestData::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	MenuRequestData::addToJsonObject(_object, _allocator);
}

void ot::NavigationMenuRequestData::setFromJsonObject(const ConstJsonObject& _object)
{
	MenuRequestData::setFromJsonObject(_object);
}
