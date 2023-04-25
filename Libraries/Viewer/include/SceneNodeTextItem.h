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

class SceneNodeTextItem : public SceneNodeBase
{
public:
	SceneNodeTextItem();
	virtual ~SceneNodeTextItem();

	virtual void setSelected(bool s) override;

	void setTextEntityID(unsigned long long id) { textEntityID = id; };
	void setTextEntityVersion(unsigned long long version) { textEntityVersion = version; };
	void setStorage(const std::string &proj) { projectName = proj; };
	
	std::string getProjectName(void) { return projectName; };

	Model *getModel(void) { return model; };
	void setModel(Model *m) { model = m; };

	virtual bool isItem1D(void) { return false; };
	virtual bool isItem3D(void) { return false; };

private:
	void loadText(void);

	std::string projectName;
	unsigned long long textEntityID;
	unsigned long long textEntityVersion;

	bool textLoaded;
	std::string text;

	Model *model;
};

