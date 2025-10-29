// @otlicense

#include "stdafx.h"
#include "MaterialMetal.h"

void MaterialMetal::setMaterialType(osg::ref_ptr<osg::Material>& mat, float r, float g, float b, float t) 
{
	mat->setAmbient(osg::Material::FRONT_AND_BACK,  osg::Vec4(0.2f * r, 0.2f * g, 0.2f * b, 1.0f));
	mat->setDiffuse(osg::Material::FRONT_AND_BACK,  osg::Vec4(0.9f * r, 0.9f * g, 0.9f * b, 1.0f));
	mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f    , 1.0f    , 1.0f    , 1.0f));

	mat->setShininess(osg::Material::FRONT_AND_BACK, 20.0f);

	mat->setColorMode(osg::Material::OFF);

	mat->setAlpha(osg::Material::FRONT_AND_BACK, t);
}
