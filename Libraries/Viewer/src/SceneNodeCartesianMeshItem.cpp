// @otlicense
// File: SceneNodeCartesianMeshItem.cpp
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

#include "stdafx.h"

#include "SceneNodeCartesianMeshItem.h"
#include "SceneNodeCartesianMesh.h"
#include "Geometry.h"

#include "DataBase.h"
#include "Model.h"

#include <osg/StateSet>
#include <osg/Node>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/LightModel>

SceneNodeCartesianMeshItem::SceneNodeCartesianMeshItem() :
	model(nullptr),
	mesh(nullptr),
	colorRGB{0.0, 0.0, 0.0},
	isVolume(false)
{

}

SceneNodeCartesianMeshItem::~SceneNodeCartesianMeshItem()
{
	getMesh()->removeOwner(this, faceID);
}

void SceneNodeCartesianMeshItem::updateVisibility(void)
{
	getMesh()->updateFaceStatus(faceID);
}

void SceneNodeCartesianMeshItem::setTransparent(bool t)
{
	if (t == isTransparent()) return;

	SceneNodeBase::setTransparent(t);
	updateVisibility();
}

void SceneNodeCartesianMeshItem::setWireframe(bool w)
{
	if (w == isWireframe()) return;

	SceneNodeBase::setWireframe(w);
	updateVisibility();
}

void SceneNodeCartesianMeshItem::setVisible(bool v)
{
	if (v == isVisible()) return;

	if (v)
	{
		getMesh()->ensureDataLoaded();
	}

	SceneNodeBase::setVisible(v);
	updateVisibility();
}

void SceneNodeCartesianMeshItem::setHighlighted(bool h)
{
	return;
	if (h == isHighlighted()) return;

	SceneNodeBase::setHighlighted(h);
	updateVisibility();
}

void SceneNodeCartesianMeshItem::setFacesList(std::vector<int> &faces)
{ 
	faceID = faces; 

	for (auto face : faceID)
	{
		getMesh()->addOwner(this, face);
	}

	updateVisibility();
}
