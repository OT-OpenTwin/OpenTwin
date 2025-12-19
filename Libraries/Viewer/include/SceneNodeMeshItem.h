// @otlicense
// File: SceneNodeMeshItem.h
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
#include "SceneNodeBase.h"
#include "DataBase.h"

#include <string>
#include <map>

#include <osg/Array>

class Model;
class SceneNodeMesh;

class SceneNodeMeshItem : public SceneNodeBase
{
public:
	SceneNodeMeshItem();
	virtual ~SceneNodeMeshItem();

	virtual void setTransparent(bool t) override;
	virtual void setWireframe(bool w) override;
	virtual void setVisible(bool v) override;
	virtual void setHighlighted(bool h) override;

	void setStorage(const std::string &proj, unsigned long long id, unsigned long long version, long long tetEdgesID, long long tetEdgesVersion) { projectName = proj; entityID = id, entityVersion = version; meshDataTetEdgesID = tetEdgesID, meshDataTetEdgesVersion = tetEdgesVersion; };
	void setNeedsInitialization(void) { needsInitialization = true; };  
	
	std::string getProjectName(void) { return projectName; };

	SceneNodeMesh *getMesh(void) { return mesh; };
	void setMesh(SceneNodeMesh *m) { mesh = m; };

	void ensureDataLoaded(void);

	bool getIsVolume(void) { return isVolume; };

	double getColorR(void) { return colorRGB[0]; };
	double getColorG(void) { return colorRGB[1]; };
	double getColorB(void) { return colorRGB[2]; };

	virtual void getPrefetch(std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs) override;

	virtual bool isItem3D(void) const override { return true; };

	void setDisplayTetEdges(bool displayEdges);

	void updateObjectColor(double r, double g, double b);

private:
	void updateVisibility(void);
	void loadEdgeData(void);

	std::string projectName;
	unsigned long long entityID;
	unsigned long long entityVersion;
	unsigned long long meshDataTetEdgesID;
	unsigned long long meshDataTetEdgesVersion;
	bool needsInitialization;

	SceneNodeMesh *mesh;

	double colorRGB[3];
	bool isVolume;

	osg::Node   *tetEdges;

	std::vector<int> faceID;
};

