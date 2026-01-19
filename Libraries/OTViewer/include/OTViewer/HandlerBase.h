// @otlicense
// File: HandlerBase.h
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

#include <osg/Geode>
#include <osg/Switch>

namespace osg
{
	class Node;
	class Switch;
	class Drawable;
}

#include <osg/Vec3d>

#include <list>

class HandlerBase;
class Viewer;

class HandlerNotifier
{
public:
	HandlerNotifier() {};
	virtual ~HandlerNotifier() {};

	virtual void handlerInteraction(HandlerBase *handler) = 0;
};

class HandlerBase
{
public:
	HandlerBase() : selectorDrawable(nullptr) {};
	virtual ~HandlerBase() {};

	virtual osg::Node *getNode(void) = 0;

	virtual void mouseOver(bool flag) = 0;

	void createSelectLine(osg::Switch *node, std::list<osg::Vec3d> &pointList, bool visible = false);

	osg::Drawable *getSelectorDrawable(void) { return selectorDrawable; }

	virtual void setInteraction(Viewer *viewer, int intersectionIndex, double intersectionRatio) = 0;
	virtual void setInteractionReference(int intersectionIndex, double intersectionRatio) = 0;

	void beginInteraction(void);
	void endInteraction(void);

	void addNotifier(HandlerNotifier *notifier);
	void removeNotifier(HandlerNotifier *notifier);

	void reportInteraction(void);

private:
	osg::Drawable *selectorDrawable;
	osg::ref_ptr<osg::Geode> selectLine;
	osg::ref_ptr<osg::Switch> switchNode;

	std::list<HandlerNotifier *> notifiers;
};

