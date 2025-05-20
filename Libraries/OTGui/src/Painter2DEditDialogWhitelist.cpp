//! @file Painter2DEditDialogWhitelist.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2DEditDialogWhitelist.h"

ot::Painter2DEditDialogWhitelist ot::Painter2DEditDialogWhitelist::createDefault(void) {
	Painter2DEditDialogWhitelist result;

	result.setAllowedPainters(AllowedPainter::AllPainters);
	result.allowAllColorStyleValueEntries();

	return result;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor / Destructor

ot::Painter2DEditDialogWhitelist::Painter2DEditDialogWhitelist() :
	m_allowedPainters(AllowedPainter::None) {
}

ot::Painter2DEditDialogWhitelist::Painter2DEditDialogWhitelist(const Painter2DEditDialogWhitelist& _other) :
	m_allowedPainters(_other.m_allowedPainters), m_allowedColorStyleValueEntries(_other.m_allowedColorStyleValueEntries) {
}

ot::Painter2DEditDialogWhitelist::Painter2DEditDialogWhitelist(Painter2DEditDialogWhitelist&& _other) noexcept :
	m_allowedPainters(std::move(_other.m_allowedPainters)), m_allowedColorStyleValueEntries(std::move(_other.m_allowedColorStyleValueEntries)) {
	_other.m_allowedPainters = AllowedPainter::None;
	_other.m_allowedColorStyleValueEntries.clear();
}

ot::Painter2DEditDialogWhitelist::~Painter2DEditDialogWhitelist() {}

ot::Painter2DEditDialogWhitelist& ot::Painter2DEditDialogWhitelist::operator=(const Painter2DEditDialogWhitelist& _other) {
	if (this != &_other) {
		m_allowedPainters = _other.m_allowedPainters;
		m_allowedColorStyleValueEntries = _other.m_allowedColorStyleValueEntries;
	}

	return *this;
}

ot::Painter2DEditDialogWhitelist& ot::Painter2DEditDialogWhitelist::operator=(Painter2DEditDialogWhitelist&& _other) noexcept {
	if (this != &_other) {
		m_allowedPainters = std::move(_other.m_allowedPainters);
		m_allowedColorStyleValueEntries = std::move(_other.m_allowedColorStyleValueEntries);

		_other.m_allowedPainters = AllowedPainter::None;
		_other.m_allowedColorStyleValueEntries.clear();
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual functions

void ot::Painter2DEditDialogWhitelist::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	JsonArray allowedArr;
	if (m_allowedPainters & Fill) { allowedArr.PushBack(JsonString("Fill", _allocator), _allocator); }
	if (m_allowedPainters & LinearGradient) { allowedArr.PushBack(JsonString("Linear", _allocator), _allocator); }
	if (m_allowedPainters & RadialGradient) { allowedArr.PushBack(JsonString("Radial", _allocator), _allocator); }
	if (m_allowedPainters & StyleReference) { allowedArr.PushBack(JsonString("Style", _allocator), _allocator); }
	_object.AddMember("Painters", allowedArr, _allocator);

	std::list<std::string> styleValues;
	for (ColorStyleValueEntry entry : m_allowedColorStyleValueEntries) {
		styleValues.push_back(toString(entry));
	}
	_object.AddMember("StyleValues", JsonArray(styleValues, _allocator), _allocator);
}

void ot::Painter2DEditDialogWhitelist::setFromJsonObject(const ConstJsonObject& _object) {
	m_allowedPainters = AllowedPainter::None;
	m_allowedColorStyleValueEntries.clear();
	
	for (const std::string& painter : json::getStringList(_object, "Painters")) {
		if (painter == "Fill") { m_allowedPainters |= Fill; }
		else if (painter == "Linear") { m_allowedPainters |= LinearGradient; }
		else if (painter == "Radial") { m_allowedPainters |= RadialGradient; }
		else if (painter == "Style") { m_allowedPainters |= StyleReference; }
		else {
			OT_LOG_E("Unknown painter type: " + painter);
		}
	}

	for (const std::string& styleValue : json::getStringList(_object, "StyleValues")) {
		m_allowedColorStyleValueEntries.push_back(stringToColorStyleValueEntry(styleValue));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::Painter2DEditDialogWhitelist::allowAllColorStyleValueEntries(void) {
	m_allowedColorStyleValueEntries.clear();
	for (int i = (int)ColorStyleValueEntry::Transparent; i < (int)ColorStyleValueEntry::ColorStyleValueEntry_End; i++) {
		m_allowedColorStyleValueEntries.push_back((ColorStyleValueEntry)i);
	}
}
