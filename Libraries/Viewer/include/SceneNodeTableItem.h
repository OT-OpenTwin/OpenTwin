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

class SceneNodeTableItem : public SceneNodeBase
{
public:
	SceneNodeTableItem();
	virtual ~SceneNodeTableItem();

	virtual void setSelected(bool s) override;

	void setTableEntityID(unsigned long long id) { tableEntityID = id; };
	void setTableEntityVersion(unsigned long long version) { tableEntityVersion = version; };
	void setStorage(const std::string &proj) { projectName = proj; };
	
	std::string getProjectName(void) { return projectName; };

	Model *getModel(void) { return model; };
	void setModel(Model *m) { model = m; };

	virtual bool isItem1D(void) { return false; };
	virtual bool isItem3D(void) { return false; };

private:
	void loadText(void);

	std::string projectName;
	unsigned long long tableEntityID;
	unsigned long long tableEntityVersion;

	bool textLoaded;
	std::string text;

	Model *model;
};

