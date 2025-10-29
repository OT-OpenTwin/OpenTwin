// @otlicense

#pragma once
#pragma warning(disable : 4251)

#include "EntityContainer.h"

#include <list>
#include <map>

class __declspec(dllexport) EntityMesh : public EntityContainer
{
public:
	EntityMesh() : EntityMesh(0, nullptr, nullptr, nullptr, "") {};
	EntityMesh(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityMesh() {};
};
