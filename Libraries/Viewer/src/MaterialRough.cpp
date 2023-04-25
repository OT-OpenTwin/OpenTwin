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
