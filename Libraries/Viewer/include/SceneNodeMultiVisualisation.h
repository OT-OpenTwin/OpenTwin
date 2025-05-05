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

	void setStorage(const std::string& _projectName) { m_projectName = _projectName; }

	std::string getProjectName(void) { return m_projectName; }

	virtual bool isItem1D(void) { return false; };
	virtual bool isItem3D(void) { return false; };

private:
	std::string m_projectName;
	
};

