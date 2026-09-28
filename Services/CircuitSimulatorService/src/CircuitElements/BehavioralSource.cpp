// @otlicense
// File: BehavioralSource.cpp
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

// Service Header
#include "CircuitElements/BehavioralSource.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitBehavioralSource.h"

static CircuitElement::Registrar<BehavioralSource> registrar("EntityBlockCircuitBehavioralSource");

BehavioralSource::BehavioralSource(std::string sourceType, std::string expression, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName)
	: m_sourceType(sourceType), m_expression(expression), CircuitElement(itemName, editorName, Uid, netlistName)
{
}

void BehavioralSource::initFromEntity(const std::shared_ptr<ot::EntityBlock>& _entity, const std::string& _editorName)
{
	auto* myElement = dynamic_cast<EntityBlockCircuitBehavioralSource*>(_entity.get());
	if (!myElement) return;

	m_sourceType = myElement->getSourceType();
	m_expression = myElement->getExpression();
	m_itemName = myElement->getBlockTitle();
	m_editorName = _editorName;
	m_Uid = myElement->getEntityID();
}

std::string BehavioralSource::getNetlistValue() const
{
	std::string expr = m_expression;
	size_t start = expr.find_first_not_of(" \t");
	if (start != std::string::npos) {
		std::string trimmed = expr.substr(start);
		if (trimmed.rfind("V=", 0) == 0 || trimmed.rfind("V =", 0) == 0 ||
			trimmed.rfind("I=", 0) == 0 || trimmed.rfind("I =", 0) == 0 ||
			trimmed.rfind("v=", 0) == 0 || trimmed.rfind("v =", 0) == 0 ||
			trimmed.rfind("i=", 0) == 0 || trimmed.rfind("i =", 0) == 0)
		{
			return trimmed;
		}
	}

	return m_sourceType + " = " + m_expression;
}
