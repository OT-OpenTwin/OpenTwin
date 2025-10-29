// @otlicense

#pragma once
#include "MaterialBase.h"

class SceneNodeMaterialFactory
{
public:
	static MaterialBase* createMaterial(const std::string& materialType);
};

