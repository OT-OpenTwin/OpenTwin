#pragma once
#include "MaterialBase.h"

class MaterialMetal : public MaterialBase
{
public:
	virtual void setMaterialType(osg::ref_ptr<osg::Material>& mat, float r, float g, float b, float t) override;
};

