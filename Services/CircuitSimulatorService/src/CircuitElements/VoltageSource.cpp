// @otlicense
// File: VoltageSource.cpp
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
#include "CircuitElements/VoltageSource.h"

// OpenTwin Header
#include "OTBlockEntities/Circuit/EntityBlockCircuitVoltageSource.h"

static CircuitElement::Registrar<VoltageSource> registrar("EntityBlockCircuitVoltageSource");

VoltageSource::VoltageSource(std::string value,std::string function,std::string type, std::string amplitude,
	const std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName)
	:m_value(value),m_function(function),m_type(type),m_Amplitude(amplitude),CircuitElement(itemName,editorName,Uid,netlistName) {

}

VoltageSource::~VoltageSource(){

}

void VoltageSource::initFromEntity(const std::shared_ptr<ot::EntityBlock>& _entity, const std::string& _editorName)
{
    auto* myElement = dynamic_cast<EntityBlockCircuitVoltageSource*>(_entity.get());
    if (!myElement) return;

    m_value = myElement->getVoltage();
    m_function = myElement->getFunction();
    m_Amplitude = myElement->getAmplitude();
    m_itemName = myElement->getBlockTitle();
    m_editorName = _editorName;
    m_Uid = myElement->getEntityID();
    // VoltageSource-spezifisch: Function-String bauen
    if (m_function == "PULSE" || m_function == "SIN" || m_function == "EXP")
    {
        std::string functionStr = m_function + "(";
        std::vector<std::string> parameters;
        if (m_function == "PULSE") { parameters = myElement->getPulseParameters(); }
        else if (m_function == "SIN") { parameters = myElement->getSinParameters(); }
        else { parameters = myElement->getExpParameters(); }
        for (const auto& param : parameters) { functionStr += param + " "; }
        functionStr += ")";
        m_function = functionStr;
    }
}

