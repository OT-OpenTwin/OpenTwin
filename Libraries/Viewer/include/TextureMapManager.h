// @otlicense

#pragma once

#include <osg/Texture2D>

#include <string>
#include <map>

class TextureMapManager
{
public:
	TextureMapManager();
	~TextureMapManager();

	static osg::ref_ptr<osg::Texture2D> getTexture(std::string textureName);


private:
	static std::map<std::string, osg::ref_ptr<osg::Texture2D>> textureImages;
};
