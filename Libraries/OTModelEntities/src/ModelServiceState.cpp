// @otlicense
// File: ModelServiceState.cpp
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
#include "OTModelEntities/ModelServiceState.h"

ot::ModelServiceState::ModelServiceState()
{

}

void ot::ModelServiceState::addToJsonObject(ot::JsonObject& _jsonObject, ot::JsonAllocator& _allocator)
{
	std::string stateString;
	switch (m_state)
	{
		case State::Uninitialized:
			stateString = "Uninitialized";
			break;
		case State::OpeningProject:
			stateString = "OpeningProject";
			break;
		case State::Running:
			stateString = "Running";
			break;
		default:
			stateString = "< Unknown >";
			break;
	}

	_jsonObject.AddMember("State", JsonString(stateString, _allocator), _allocator);
	_jsonObject.AddMember("ProjectIsOpen", m_projectIsOpen, _allocator);
	_jsonObject.AddMember("IsShuttingDown", m_isShuttingDown, _allocator);
}

ot::ModelServiceStateRAII ot::ModelServiceState::setTemporaryState(State _state)
{
	return ModelServiceStateRAII(*this, _state);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::ModelServiceStateRAII::ModelServiceStateRAII(ModelServiceState& _modelServiceState, ModelServiceState::State _stateToSet)
	: m_modelServiceState(&_modelServiceState)
{
	m_previousState = m_modelServiceState->getState();
	m_modelServiceState->setState(_stateToSet);
}

ot::ModelServiceStateRAII::ModelServiceStateRAII(ModelServiceStateRAII&& _other) noexcept
	: m_modelServiceState(_other.m_modelServiceState), m_previousState(_other.m_previousState)
{
	_other.m_modelServiceState = nullptr;
}

ot::ModelServiceStateRAII::~ModelServiceStateRAII()
{
	if (m_modelServiceState)
	{
		m_modelServiceState->setState(m_previousState);
	}
}
