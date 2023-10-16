#pragma once
#pragma warning(disable : 4251)
#include <string>
#include "EntityBase.h"
class _declspec(dllexport) ClassFactoryHandler
{
public:
	virtual ClassFactoryHandler* SetNextHandler(ClassFactoryHandler* nextHandler) = 0;
	virtual EntityBase* CreateEntity(const std::string& className) = 0;
	void SetChainRoot(ClassFactoryHandler* root) { _chainRoot = root; };
	ClassFactoryHandler* GetChainRoot() { return _chainRoot; }

protected:
	ClassFactoryHandler* _chainRoot = nullptr;
};
