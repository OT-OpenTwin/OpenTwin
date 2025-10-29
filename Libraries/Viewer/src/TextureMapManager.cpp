// @otlicense

#include "stdafx.h"

#include "TextureMapManager.h"

#include <osgDB/ReadFile>

#include <qcoreapplication.h>	// QCoreApplication
#include <qfile.h>				// QFile

std::map<std::string, osg::ref_ptr<osg::Texture2D>> TextureMapManager::textureImages;

TextureMapManager::TextureMapManager()
{
	

}

TextureMapManager::~TextureMapManager()
{
	textureImages.clear();
}

osg::ref_ptr<osg::Texture2D> TextureMapManager::getTexture(std::string textureName)
{
	if (textureImages.count(textureName) != 0)
	{
		// We have loaded this texture already
		return textureImages[textureName];
	}

	// We need to load this texture
	// Determine the full file path first

	std::string fontPath = QCoreApplication::applicationDirPath().toStdString();
	fontPath.append(std::string("/Icons/Textures/" + textureName + ".jpg").c_str());
	if (!QFile::exists(fontPath.c_str()))
	{
		fontPath = std::string(qgetenv("OPENTWIN_DEV_ROOT")) + "/Assets/Icons/Textures/" + textureName + ".jpg";

		if (!QFile::exists(fontPath.c_str())) {
			assert(0); // Environment path does not exist aswell
			return nullptr;
		}
	}

	// Now create the new texture and load the image

	osg::ref_ptr<osg::Texture2D> texture2D = new osg::Texture2D;

	texture2D->setImage(osgDB::readRefImageFile(fontPath));

	/*
	texture2D->setDataVariance(osg::Object::DYNAMIC);

	texture2D->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	texture2D->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);

	texture2D->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
	texture2D->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	*/

	textureImages[textureName] = texture2D;

	return texture2D;
}

