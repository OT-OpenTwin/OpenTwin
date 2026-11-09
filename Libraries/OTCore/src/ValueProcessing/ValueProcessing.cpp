// @otlicense
// File: ValueProcessing.cpp
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
#include "OTCore/ValueProcessing/ValueProcessing.h"

ot::ValueProcessing::ValueProcessing(const ConstJsonObject& _jsonObject)
	: ValueProcessing()
{
	setFromJsonObject(_jsonObject);
}

ot::ValueProcessing::ValueProcessing(const ValueProcessing& _other)
{
	for (const ValueProcessor* processor : _other.m_processors)
	{
		m_processors.push_back(processor->createCopy());
	}
}

ot::ValueProcessing::ValueProcessing(ValueProcessing&& _other) noexcept
	: m_processors(std::move(_other.m_processors))
{
	_other.m_processors.clear();
}

ot::ValueProcessing::~ValueProcessing()
{
	for (ValueProcessor* processor : m_processors)
	{
		OTAssertNullptr(processor);
		delete processor;
	}
}

ot::ValueProcessing& ot::ValueProcessing::operator=(const ValueProcessing& _other)
{
	if (this != &_other)
	{
		for (const ValueProcessor* processor : _other.m_processors)
		{
			m_processors.push_back(processor->createCopy());
		}
	}
	return *this;
}

ot::ValueProcessing& ot::ValueProcessing::operator=(ValueProcessing&& _other) noexcept
{
	if (this != &_other)
	{
		m_processors = std::move(_other.m_processors);
		_other.m_processors.clear();
	}
	return *this;
}

void ot::ValueProcessing::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	JsonArray procArr;
	for (const ValueProcessor* proc : m_processors)
	{
		procArr.PushBack(JsonObject(proc, _allocator), _allocator);
	}
	_jsonObject.AddMember("Processors", procArr, _allocator);
}

void ot::ValueProcessing::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	clear();
	for (const ConstJsonObject& procObj : json::getObjectList(_jsonObject, "Processors"))
	{
		m_processors.push_back(ValueProcessor::fromJson(procObj));
	}
}

ot::Variable ot::ValueProcessing::execute(const ot::Variable& _input) const
{
	ot::Variable currentValue = _input;
	for (ValueProcessor* currentProcessor : m_processors)
	{
		currentValue = currentProcessor->execute(currentValue);
	}
	return currentValue;
}

ot::ValueProcessing ot::ValueProcessing::inverse() const
{
	ValueProcessing inv;

	for (auto it = m_processors.rbegin(); it != m_processors.rend(); ++it)
	{
		inv.m_processors.push_back((*it)->inverse());
	}
	
	return inv;
}

void ot::ValueProcessing::clear()
{
	for (ValueProcessor* processor : m_processors)
	{
		OTAssertNullptr(processor);
		delete processor;
	}
	m_processors.clear();
}

