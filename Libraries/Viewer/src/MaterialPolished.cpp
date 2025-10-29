// @otlicense

#include "stdafx.h"
#include "MaterialPolished.h"


void MaterialPolished::setMaterialType(osg::ref_ptr<osg::Material>& mat, float r, float g, float b, float t) 
{
	mat->setAmbient(osg::Material::FRONT_AND_BACK,  osg::Vec4(0.3f * r, 0.3f * g, 0.3f * b, 1.0f));
	mat->setDiffuse(osg::Material::FRONT_AND_BACK, 	osg::Vec4(0.9f * r, 0.9f * g, 0.9f * b, 1.0f));
	mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.7f * r, 0.7f * g, 0.7f * b, 1.0f));

	mat->setShininess(osg::Material::FRONT_AND_BACK, 10.0f);

	mat->setColorMode(osg::Material::OFF);

	mat->setAlpha(osg::Material::FRONT_AND_BACK, t);
}
