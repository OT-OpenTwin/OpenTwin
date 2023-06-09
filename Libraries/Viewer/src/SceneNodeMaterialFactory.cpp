#include "stdafx.h"
#include "SceneNodeMaterialFactory.h"

#include "MaterialRough.h"
#include "MaterialPlastic.h"
#include "MaterialPolished.h"
#include "MaterialMetal.h"

#include <cassert>

MaterialBase* SceneNodeMaterialFactory::createMaterial(const std::string& matType)
{
	if (matType == "Rough")
	{
		return new MaterialRough;
	}
	else if (matType == "Plastic")
	{
		return new MaterialPlastic;
	}
	else if (matType == "Polished")
	{
		return new MaterialPolished;
	}
	else if (matType == "Metal")
	{
		return new MaterialMetal;
	}

	assert(0); // Unknown material type
	return nullptr;
}
