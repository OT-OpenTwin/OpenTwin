#include "stdafx.h"
#include "SceneNodeMaterial.h"
#include "SceneNodeMaterialFactory.h"

MaterialBase* SceneNodeMaterial::setMaterial(osg::ref_ptr<osg::Material>& mat, const std::string& materialType, float r, float g, float b, float t)
{
	MaterialBase* material = SceneNodeMaterialFactory::createMaterial(materialType);

	if (material) 
	{
		material->setMaterialType(mat, r, g, b, t);
	}

	return material;
}

bool SceneNodeMaterial::materialHasTexture(MaterialBase* mat) {
	if (mat) {
		return mat->hasTexture;
	}
	return false;
}
