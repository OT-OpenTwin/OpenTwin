//! @file Painter2DDialogFilter.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2DDialogFilter.h"

ot::Painter2DDialogFilter::Painter2DDialogFilter() :
	m_allowedPainters(Painter2DDialogFilter::All), m_allowedReferences(ot::getAllColorStyleValueEntries())
{

}

void ot::Painter2DDialogFilter::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonArray painterArr;
	if (m_allowedPainters & Painter2DDialogFilter::Fill) { painterArr.PushBack(JsonString("Fill", _allocator), _allocator); }
	if (m_allowedPainters & Painter2DDialogFilter::StyleRef) { painterArr.PushBack(JsonString("StyleRef", _allocator), _allocator); }
	if (m_allowedPainters & Painter2DDialogFilter::LinearGradient) { painterArr.PushBack(JsonString("LinearGradient", _allocator), _allocator); }
	if (m_allowedPainters & Painter2DDialogFilter::RadialGradient) { painterArr.PushBack(JsonString("RadialGradient", _allocator), _allocator); }
	_object.AddMember("Painters", painterArr, _allocator);

	JsonArray referenceArr;
	for (ColorStyleValueEntry ref : m_allowedReferences) {
		referenceArr.PushBack(JsonString(ot::toString(ref), _allocator), _allocator);
	}
	_object.AddMember("References", referenceArr, _allocator);

}

void ot::Painter2DDialogFilter::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_allowedPainters = None;
	m_allowedReferences.clear();

	for (const std::string& painterType : json::getStringList(_object, "Painters")) {
		if (painterType == "Fill") { m_allowedPainters |= Painter2DDialogFilter::Fill; }
		else if (painterType == "StyleRef") { m_allowedPainters |= Painter2DDialogFilter::StyleRef; }
		else if (painterType == "LinearGradient") { m_allowedPainters |= Painter2DDialogFilter::LinearGradient; }
		else if (painterType == "RadialGradient") { m_allowedPainters |= Painter2DDialogFilter::RadialGradient; }
		else {
			OT_LOG_EAS("Unknown painter type \"" + painterType + "\"");
		}
	}

	for (const std::string& reference : json::getStringList(_object, "References")) {
		m_allowedReferences.push_back(ot::stringToColorStyleValueEntry(reference));
	}
}
