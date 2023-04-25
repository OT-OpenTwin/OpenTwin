#pragma once

namespace osg
{
	class Node;
	class Switch;
}

#include "Geometry.h"
#include "Viewer.h"
#include "SceneNodeBase.h"

#include <string>
#include <ctime>
#include <list>

#include <osg/Array>
#include <osg/Material>

class Model;
class Viewer;

class SceneNodeVTK : public SceneNodeBase
{
public:
	SceneNodeVTK();
	virtual ~SceneNodeVTK();

	virtual void setTransparent(bool t) override;
	virtual void setWireframe(bool w) override;
	virtual void setVisible(bool v) override;
	virtual void setHighlighted(bool h) override;
	
	Model *getModel(void) { return model; };
	void setModel(Model *m) { model = m; };

	virtual bool isItem1D(void) { return false; };
	virtual bool isItem3D(void) { return true; };

	void updateVTKNode(const std::string &projName, unsigned long long visualizationDataID, unsigned long long visualizationDataVersion);

private:
	void deleteShapeNode(void);
	osg::Node *createOSGNodeFromVTK();
	void reportTime(const std::string &message, std::time_t &timer);
	std::string loadDataItem(unsigned long long entityID, unsigned long long entityVersion);

	Model *model;
	bool initialized;
	std::string projectName;
	unsigned long long dataID;
	unsigned long long dataVersion;
};

