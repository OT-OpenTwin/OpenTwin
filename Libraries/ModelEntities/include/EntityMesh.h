#pragma once
#pragma warning(disable : 4251)

#include "EntityContainer.h"

#include <list>
#include <map>

class __declspec(dllexport) EntityMesh : public EntityContainer
{
public:
	EntityMesh(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner)
	: EntityContainer(ID, parent, obs, ms, factory, owner){};
	virtual ~EntityMesh() {};
};
