#pragma once
#pragma warning(disable : 4251)
#include <string>
#include "EntityBase.h"
class _declspec(dllexport) ClassFactoryHandler
{
public:
	virtual ClassFactoryHandler* setNextHandler(ClassFactoryHandler* nextHandler) = 0;
	virtual EntityBase* createEntity(const std::string& className) = 0;
	void setChainRoot(ClassFactoryHandler* root) { m_chainRoot = root; };
	ClassFactoryHandler* getChainRoot() { return m_chainRoot; }

protected:
	ClassFactoryHandler* m_chainRoot = nullptr;
};
