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

class SceneNodeResult1DItem : public SceneNodeBase
{
public:
	SceneNodeResult1DItem();
	virtual ~SceneNodeResult1DItem();

	virtual void setTransparent(bool t) override;
	virtual void setWireframe(bool w) override;
	virtual void setVisible(bool v) override;
	virtual void setHighlighted(bool h) override;

	void setModelEntityVersion(unsigned long long version) { modelEntityVersion = version; };
	void setStorage(const std::string &proj) { projectName = proj; };
	
	std::string getProjectName(void) { return projectName; };

	Model *getModel(void) { return model; };
	void setModel(Model *m) { model = m; };

	virtual bool isItem1D(void) { return true; };
	virtual bool isItem3D(void) { return false; };

	unsigned long long getModelEntityVersion(void) { return modelEntityVersion; };


private:
	std::string projectName;
	unsigned long long modelEntityVersion;

	Model *model;
};

