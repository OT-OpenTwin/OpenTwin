#pragma once
#include "OTCore/JSON.h"
#include <string>

class ActionAndFunctionHandler
{
public:
	void setNextHandler(ActionAndFunctionHandler* _nextHandler) { m_nextHandler = _nextHandler; }
	
	bool tryToHandleAction(const std::string& _action, ot::JsonDocument& _doc)
	{
		bool successfullyHandledByThisHandler = handleAction(_action, _doc);
		if (!successfullyHandledByThisHandler)
		{
			if (m_nextHandler != nullptr)
			{
				return m_nextHandler->tryToHandleAction(_action, _doc);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
	virtual ~ActionAndFunctionHandler()
	{
		if (m_nextHandler != nullptr)
		{
			delete m_nextHandler;
			m_nextHandler = nullptr;
		}
	}
protected:
	virtual bool handleAction(const std::string& _action, ot::JsonDocument& _doc) { return false; }
private:
	ActionAndFunctionHandler* getNextHandler() { return m_nextHandler; }
	ActionAndFunctionHandler* m_nextHandler = nullptr;
};
