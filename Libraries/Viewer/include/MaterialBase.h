// @otlicense

#pragma once
#include <osg/Material>
#include <osgDB/ReadFile>
#include <osg/Texture2D>

class MaterialBase
{
public:	
	MaterialBase() {};
	bool hasTexture{ false };
	virtual ~MaterialBase(){}
	virtual void setMaterialType(osg::ref_ptr<osg::Material>& mat, float r, float g, float b, float t) = 0;
	virtual void loadTexture(osg::ref_ptr<osg::Texture2D>& texture2D) {};
};

