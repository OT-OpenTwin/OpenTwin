// @otlicense

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

ot::ModelServiceStateRAII::ModelServiceStateRAII(ModelServiceStateRAII&& _other)
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
