#pragma once
#include <string>
#include "MaterialBase.h"
#include <osg/Material>

class SceneNodeMaterial
{
public:
	virtual ~SceneNodeMaterial(){}
	MaterialBase* setMaterial(osg::ref_ptr<osg::Material>& mat, const std::string& materialType, float r, float g, float b, float t);
	bool materialHasTexture(MaterialBase* mat);
};

