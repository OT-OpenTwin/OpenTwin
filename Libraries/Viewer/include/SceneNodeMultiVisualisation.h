#pragma once

namespace osg
{
	class Node;
	class Switch;
}

#include "SceneNodeBase.h"
#include "TextVisualiser.h"

#include <string>
#include <map>
class Model;

class SceneNodeMultiVisualisation : public SceneNodeBase
{
public:
	SceneNodeMultiVisualisation() = default;
	virtual ~SceneNodeMultiVisualisation();

	virtual bool isItem1D(void) { return false; };
	virtual bool isItem3D(void) { return false; };

private:	
};

