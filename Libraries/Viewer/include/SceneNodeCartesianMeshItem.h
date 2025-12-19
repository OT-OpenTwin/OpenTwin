// @otlicense
// File: SceneNodeCartesianMeshItem.h
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
#include <vector>

#include <osg/Array>

class Model;
class SceneNodeCartesianMesh;

class SceneNodeCartesianMeshItem : public SceneNodeBase
{
public:
	SceneNodeCartesianMeshItem();
	virtual ~SceneNodeCartesianMeshItem();

	virtual void setTransparent(bool t) override;
	virtual void setWireframe(bool w) override;
	virtual void setVisible(bool v) override;
	virtual void setHighlighted(bool h) override;
	
	SceneNodeCartesianMesh *getMesh(void) { return m_mesh; };
	void setMesh(SceneNodeCartesianMesh *m) { m_mesh = m; };

	bool getIsVolume(void) { return m_isVolume; };

	double getColorR(void) { return m_colorRGB[0]; };
	double getColorG(void) { return m_colorRGB[1]; };
	double getColorB(void) { return m_colorRGB[2]; };

	virtual bool isItem3D(void) const override { return true; };

	void setFacesList(const std::vector<int>& _faces);
	void setColor(double r, double g, double b) { m_colorRGB[0] = r; m_colorRGB[1] = g; m_colorRGB[2] = b; }

private:
	void updateVisibility(void);

	SceneNodeCartesianMesh* m_mesh;

	double m_colorRGB[3];
	bool m_isVolume;

	std::vector<int> m_faceID;
};

