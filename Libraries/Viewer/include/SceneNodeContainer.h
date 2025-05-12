#pragma once

#include "SceneNodeBase.h"

#include <string>

class SceneNodeContainer : public SceneNodeBase
{
public:
	SceneNodeContainer();
	virtual ~SceneNodeContainer();

	virtual bool isItem3D(void) { return false; };

};

