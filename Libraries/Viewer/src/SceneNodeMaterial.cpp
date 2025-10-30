// @otlicense
// File: SceneNodeMaterial.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
