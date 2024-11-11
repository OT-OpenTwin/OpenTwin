#pragma once

namespace osg
{
	class Node;
	class Switch;
}

#include "Geometry.h"
#include "SceneNodeBase.h"
#include "DataBase.h"
#include "TextVisualiser.h"


#include <string>
#include <map>

#include <osg/Array>

class Model;

class SceneNodeTextItem : public SceneNodeBase
{
public:
	SceneNodeTextItem();
	virtual ~SceneNodeTextItem();

	virtual void setSelected(bool _selection) override;

	void setStorage(const std::string &proj) { projectName = proj; };
	
	std::string getProjectName(void) { return projectName; };

	Model *getModel(void) { return model; };
	void setModel(Model *m) { model = m; };

	virtual bool isItem1D(void) { return false; };
	virtual bool isItem3D(void) { return false; };

	void addVisualiserText();

private:
	std::string projectName;

	bool textLoaded;
	std::string text;
	Model *model;
};

