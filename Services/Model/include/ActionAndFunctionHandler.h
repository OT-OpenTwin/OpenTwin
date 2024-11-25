#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <string>

class ActionAndFunctionHandler {
	OT_DECL_NOCOPY(ActionAndFunctionHandler)
public:
	ActionAndFunctionHandler();
	virtual ~ActionAndFunctionHandler();

	void setDontDeleteHandler(bool _dontDelete = true) { m_dontDeleteHandler = _dontDelete; };
	void setNextHandler(ActionAndFunctionHandler* _nextHandler) { m_nextHandler = _nextHandler; };

	bool tryToHandleAction(const std::string& _action, ot::JsonDocument& _doc);

protected:
	virtual bool handleAction(const std::string& _action, ot::JsonDocument& _doc) { return false; };

private:
	ActionAndFunctionHandler* getNextHandler(void) { return m_nextHandler; }
	ActionAndFunctionHandler* m_nextHandler;
	bool m_dontDeleteHandler;
};
