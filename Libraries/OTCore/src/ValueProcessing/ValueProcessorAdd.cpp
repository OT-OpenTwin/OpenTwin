// @otlicense
// File: ValueProcessorAdd.cpp
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
#include "OTCore/ValueProcessing/ValueProcessorAdd.h"

static ot::ValueProcessor::Registrar<ot::ValueProcessorAdd> registrar(ot::ValueProcessorAdd::className());

ot::ValueProcessorAdd::ValueProcessorAdd(double _summand)
	: m_summand(_summand)
{}

ot::Variable ot::ValueProcessorAdd::execute(const ot::Variable & _input)
{
	return _input + m_summand;
}

ot::ValueProcessor* ot::ValueProcessorAdd::inverse() const
{
	return new ValueProcessorAdd(-m_summand);
}

void ot::ValueProcessorAdd::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	ValueProcessor::addToJsonObject(_jsonObject, _allocator);
	_jsonObject.AddMember("Summand", m_summand, _allocator);
}

void ot::ValueProcessorAdd::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	ValueProcessor::setFromJsonObject(_jsonObject);
	m_summand = json::getDouble(_jsonObject, "Summand");
}