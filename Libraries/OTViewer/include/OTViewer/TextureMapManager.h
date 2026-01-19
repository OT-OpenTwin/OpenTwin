// @otlicense
// File: TextureMapManager.h
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
