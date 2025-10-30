// @otlicense
// File: MaterialRough.cpp
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
#include "MaterialRough.h"

void MaterialRough::setMaterialType(osg::ref_ptr<osg::Material>& mat, float r, float g, float b, float t) 
{
	mat->setAmbient(osg::Material::FRONT_AND_BACK,  osg::Vec4(0.3f * r, 0.3f * g, 0.3f * b, 1.0f));
	mat->setDiffuse(osg::Material::FRONT_AND_BACK,  osg::Vec4(0.8f * r, 0.8f * g, 0.8f * b, 1.0f));
	mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f    , 0.0f    , 0.0f    , 1.0f));

	mat->setColorMode(osg::Material::OFF);

	mat->setAlpha(osg::Material::FRONT_AND_BACK, t);

	mat->setShininess(osg::Material::FRONT_AND_BACK, 0);
}
