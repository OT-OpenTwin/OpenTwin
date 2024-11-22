// OpenTwin header
#include "stdafx.h"
#include "ActionAndFunctionHandler.h"

ActionAndFunctionHandler::ActionAndFunctionHandler() 
	: m_nextHandler(nullptr), m_dontDeleteHandler(false)
{

}

ActionAndFunctionHandler::~ActionAndFunctionHandler() {
	if (m_nextHandler != nullptr) {
		if (!m_nextHandler->m_dontDeleteHandler) {
			delete m_nextHandler;
			m_nextHandler = nullptr;
		}
	}
}

bool ActionAndFunctionHandler::tryToHandleAction(const std::string& _action, ot::JsonDocument& _doc) {
	if (this->handleAction(_action, _doc)) {
		return true;
	}
	else if (m_nextHandler != nullptr) {
		return m_nextHandler->tryToHandleAction(_action, _doc);
	}
	else {
		return false;
	}
}