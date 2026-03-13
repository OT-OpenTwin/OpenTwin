// @otlicense

// OpenTwin header
#include "OTGui/Event/TextEditorMenuRequestData.h"

static ot::MenuRequestData::Registrar<ot::TextEditorMenuRequestData> registrar(ot::TextEditorMenuRequestData::className());

ot::TextEditorMenuRequestData::TextEditorMenuRequestData()
	: MenuRequestData(WidgetViewBase::ViewText)
{

}

ot::TextEditorMenuRequestData::~TextEditorMenuRequestData()
{
}

void ot::TextEditorMenuRequestData::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	MenuRequestData::addToJsonObject(_object, _allocator);
}

void ot::TextEditorMenuRequestData::setFromJsonObject(const ConstJsonObject& _object)
{
	MenuRequestData::setFromJsonObject(_object);
}
