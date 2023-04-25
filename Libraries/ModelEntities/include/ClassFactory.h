#pragma once
#pragma warning(disable : 4251)

#include <string>
class EntityBase;

class _declspec(dllexport) ClassFactory
{
public:
	ClassFactory() {};
	virtual ~ClassFactory() {};

	virtual EntityBase *CreateEntity(const std::string &entityType);
};

