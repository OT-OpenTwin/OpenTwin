// @otlicense
// File: SceneNodeVTK.h
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

namespace osg
{
	class Node;
	class Switch;
}

#include "Geometry.h"
#include "Viewer.h"
#include "SceneNodeBase.h"

#include <string>
#include <ctime>
#include <list>

#include <osg/Array>
#include <osg/Material>

class Model;
class Viewer;

class SceneNodeVTK : public SceneNodeBase
{
public:
	SceneNodeVTK();
	virtual ~SceneNodeVTK();

	virtual void setTransparent(bool t) override;
	virtual void setWireframe(bool w) override;
	virtual void setVisible(bool v) override;
	virtual void setHighlighted(bool h) override;
	
	virtual bool isItem3D(void) const override { return true; };

	void updateVTKNode(const std::string &projName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion);

private:
	void deleteShapeNode(void);
	osg::Node *createOSGNodeFromVTK();
	void reportTime(const std::string &message, std::time_t &timer);
	std::string loadDataItem(unsigned long long entityID, unsigned long long entityVersion);

	bool initialized;
	std::string projectName;
	unsigned long long dataID;
	unsigned long long dataVersion;
};

