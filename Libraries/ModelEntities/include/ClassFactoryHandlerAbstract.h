#pragma once
#include "ClassFactoryHandler.h"

class  _declspec(dllexport) ClassFactoryHandlerAbstract : public ClassFactoryHandler
{
public:
	virtual ClassFactoryHandler* setNextHandler(ClassFactoryHandler* _nextHandler) override
	{
		m_nextHandler = _nextHandler;
		return m_nextHandler;
	}

	virtual EntityBase* createEntity(const std::string& _className) override
	{
		if (m_nextHandler != nullptr)
		{
			return m_nextHandler->createEntity(_className);
		}
		return nullptr;
	}

private:
	ClassFactoryHandler* m_nextHandler = nullptr;
};
