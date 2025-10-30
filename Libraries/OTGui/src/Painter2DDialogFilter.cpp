// @otlicense
// File: Painter2DDialogFilter.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
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
