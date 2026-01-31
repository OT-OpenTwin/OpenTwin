// @otlicense
// File: SequenceDiaFunctionCfg.cpp
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
#include "OTGui/Diagram/SequenceDiaFunctionCfg.h"
#include "OTGui/Diagram/SequenceDiaCallCfgFactory.h"

ot::SequenceDiaFunctionCfg::SequenceDiaFunctionCfg(const ConstJsonObject& _jsonObject)
	: SequenceDiaFunctionCfg()
{
	this->setFromJsonObject(_jsonObject);
}

ot::SequenceDiaFunctionCfg::SequenceDiaFunctionCfg(SequenceDiaFunctionCfg&& _other) noexcept {
	this->operator=(std::move(_other));
}

ot::SequenceDiaFunctionCfg::~SequenceDiaFunctionCfg() {
	clear();
}

ot::SequenceDiaFunctionCfg& ot::SequenceDiaFunctionCfg::operator=(SequenceDiaFunctionCfg&& _other) noexcept {
	if (this != &_other) {
		clear();

		m_lifeLine = std::move(_other.m_lifeLine);
		m_name = std::move(_other.m_name);
		m_defaultText = std::move(_other.m_defaultText);
		m_filePath = std::move(_other.m_filePath);
		m_lineNr = _other.m_lineNr;
		m_returnValue = std::move(_other.m_returnValue);
		m_calls = std::move(_other.m_calls);
		_other.m_calls.clear();

		_other.clear();
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ot::SequenceDiaFunctionCfg::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("LifeLine", JsonString(m_lifeLine, _allocator), _allocator);
	_jsonObject.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_jsonObject.AddMember("DefaultText", JsonString(m_defaultText, _allocator), _allocator);
	_jsonObject.AddMember("FilePath", JsonString(m_filePath, _allocator), _allocator);
	_jsonObject.AddMember("LineNr", m_lineNr, _allocator);
	_jsonObject.AddMember("ReturnValue", JsonString(m_returnValue, _allocator), _allocator);

	JsonArray callsArray;
	for (const SequenceDiaAbstractCallCfg* call : m_calls) {
		callsArray.PushBack(JsonObject(call, _allocator), _allocator);
	}
	_jsonObject.AddMember("Calls", callsArray, _allocator);
}

void ot::SequenceDiaFunctionCfg::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	clear();

	m_lifeLine = json::getString(_jsonObject, "LifeLine");
	m_name = json::getString(_jsonObject, "Name");
	m_defaultText = json::getString(_jsonObject, "DefaultText");
	m_filePath = json::getString(_jsonObject, "FilePath");
	m_lineNr = json::getUInt64(_jsonObject, "LineNr");
	m_returnValue = json::getString(_jsonObject, "ReturnValue");

	for (const ConstJsonObject& callObject : json::getObjectList(_jsonObject, "Calls")) {
		SequenceDiaAbstractCallCfg* call = SequenceDiaCallCfgFactory::create(callObject);
		if (call != nullptr) {
			m_calls.push_back(call);
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::SequenceDiaFunctionCfg::clear() {
	m_lifeLine.clear();
	m_name.clear();
	m_defaultText.clear();
	m_filePath.clear();
	m_lineNr = 0;
	m_returnValue.clear();

	for (SequenceDiaAbstractCallCfg* call : m_calls) {
		delete call;
	}
	m_calls.clear();
}

bool ot::SequenceDiaFunctionCfg::isValid() const {
	return !m_name.empty() && !m_lifeLine.empty();
}

void ot::SequenceDiaFunctionCfg::addCall(SequenceDiaAbstractCallCfg* _call) {
	OTAssertNullptr(_call);
	m_calls.push_back(_call);
}
