// @otlicense
// File: SceneNodeContainer.cpp
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

#include "SceneNodeContainer.h"

#include <osg/Node>
#include <osg/Switch>

SceneNodeContainer::SceneNodeContainer()
{
	// Create a group osg node
	m_shapeNode = new osg::Switch;

	m_shapeNode->setAllChildrenOn();
}

SceneNodeContainer::~SceneNodeContainer()
{
	// We first need to recursively delete all children, since the osg nodes will also recursively delete their children as well.
	std::list<SceneNodeBase*> currentChilds = getChildren();
	for (auto child : currentChilds)
	{
		delete child;
	}
}
