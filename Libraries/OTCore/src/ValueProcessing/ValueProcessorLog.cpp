// @otlicense
// File: ValueProcessorLog.cpp
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
#include "OTCore/Variable/VariableHelper.h"
#include "OTCore/ValueProcessing/ValueProcessorLog.h"
#include "OTCore/ValueProcessing/ValueProcessorPow.h"

static ot::ValueProcessor::Registrar<ot::ValueProcessorLog> registrar(ot::ValueProcessorLog::className());

ot::ValueProcessorLog::ValueProcessorLog(double _multiplier, LogBase _base)
	: m_multiplier(_multiplier), m_base(_base)
{
}

ot::Variable ot::ValueProcessorLog::execute(const ot::Variable& _input)
{
	ot::Variable value;
	switch (m_base)
	{
	case ot::ValueProcessorLog::LogBase::Base10: value = ot::VariableHelper::log10(_input); break;
	case ot::ValueProcessorLog::LogBase::Natural: value = ot::VariableHelper::ln(_input); break;
	default:
		OT_LOG_E("Unknown LogBase (" + std::to_string(static_cast<int32_t>(m_base)) + ")");
		throw Exception::InvalidArgument("Unknown LogBase (" + std::to_string(static_cast<int32_t>(m_base)) + ")");
	}
	
	value = value * m_multiplier;
	return value;
}

ot::ValueProcessor* ot::ValueProcessorLog::inverse() const
{	
	switch (m_base)
	{
	case ot::ValueProcessorLog::LogBase::Base10: return new ValueProcessorPow(m_multiplier, 10);
	case ot::ValueProcessorLog::LogBase::Natural: return new ValueProcessorPow(m_multiplier, Math::e());
	default:
		OT_LOG_E("Unknown LogBase (" + std::to_string(static_cast<int32_t>(m_base)) + ")");
		throw Exception::InvalidArgument("Unknown LogBase (" + std::to_string(static_cast<int32_t>(m_base)) + ")");
	}
}

void ot::ValueProcessorLog::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	ValueProcessor::addToJsonObject(_jsonObject, _allocator);
	_jsonObject.AddMember("Multiplier", m_multiplier, _allocator);
	_jsonObject.AddMember("LogBase", static_cast<int32_t>(m_base), _allocator);
}

void ot::ValueProcessorLog::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	ValueProcessor::setFromJsonObject(_jsonObject);
	m_multiplier = json::getDouble(_jsonObject, "Multiplier");
	m_base = static_cast<LogBase>(json::getInt(_jsonObject, "LogBase"));
}