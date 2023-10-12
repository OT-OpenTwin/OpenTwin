#pragma once
#include "ClassFactoryHandler.h"

class  _declspec(dllexport) ClassFactoryHandlerAbstract : public ClassFactoryHandler
{
public:
	virtual ClassFactoryHandler* SetNextHandler(ClassFactoryHandler* nextHandler) override
	{
		_nextHandler = nextHandler;
		return _nextHandler;
	}

	virtual EntityBase* CreateEntity(const std::string& className) override
	{
		if (_nextHandler != nullptr)
		{
			return _nextHandler->CreateEntity(className);
		}
		return nullptr;
	}

private:
	ClassFactoryHandler* _nextHandler = nullptr;
};
