// @otlicense

// OpenTwin header
#include "OTGui/Event/NavigationContextRequestData.h"

static ot::ContextRequestData::Registrar<ot::NavigationContextRequestData> registrar(ot::NavigationContextRequestData::className());

ot::NavigationContextRequestData::NavigationContextRequestData()
	: ContextRequestData(WidgetViewBase::ViewNavigation)
{}

ot::NavigationContextRequestData::NavigationContextRequestData(const BasicEntityInformation & _basicEntityInformation)
	: ContextRequestData(_basicEntityInformation, WidgetViewBase::ViewNavigation)
{}

ot::NavigationContextRequestData::NavigationContextRequestData(BasicEntityInformation&& _basicEntityInformation) noexcept
	: ContextRequestData(std::move(_basicEntityInformation), WidgetViewBase::ViewNavigation)
{}

void ot::NavigationContextRequestData::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	ContextRequestData::addToJsonObject(_object, _allocator);
}

void ot::NavigationContextRequestData::setFromJsonObject(const ConstJsonObject& _object)
{
	ContextRequestData::setFromJsonObject(_object);
}
