// @otlicense
// File: ValueProcessorMultiply.cpp
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
#include "OTCore/ValueProcessing/ValueProcessorMultiply.h"

static ot::ValueProcessor::Registrar<ot::ValueProcessorMultiply> registrar(ot::ValueProcessorMultiply::className());

ot::ValueProcessorMultiply::ValueProcessorMultiply(double _factor)
    : m_factor(_factor)
{}

ot::Variable ot::ValueProcessorMultiply::execute(const ot::Variable& _input)
{
    return _input * m_factor;
}

ot::ValueProcessor* ot::ValueProcessorMultiply::inverse() const
{
    return new ValueProcessorMultiply(1. / m_factor);
}

void ot::ValueProcessorMultiply::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
    ValueProcessor::addToJsonObject(_jsonObject, _allocator);
    _jsonObject.AddMember("Factor", m_factor, _allocator);
}

void ot::ValueProcessorMultiply::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
    ValueProcessor::setFromJsonObject(_jsonObject);
    m_factor = json::getDouble(_jsonObject, "Factor");
}
