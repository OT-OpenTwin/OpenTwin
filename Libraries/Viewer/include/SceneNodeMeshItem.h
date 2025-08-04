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
class SceneNodeMesh;

class SceneNodeMeshItem : public SceneNodeBase
{
public:
	SceneNodeMeshItem();
	virtual ~SceneNodeMeshItem();

	virtual void setTransparent(bool t) override;
	virtual void setWireframe(bool w) override;
	virtual void setVisible(bool v) override;
	virtual void setHighlighted(bool h) override;

	void setStorage(const std::string &proj, unsigned long long id, unsigned long long version, long long tetEdgesID, long long tetEdgesVersion) { projectName = proj; entityID = id, entityVersion = version; meshDataTetEdgesID = tetEdgesID, meshDataTetEdgesVersion = tetEdgesVersion; };
	void setNeedsInitialization(void) { needsInitialization = true; };  
	
	std::string getProjectName(void) { return projectName; };

	Model *getModel(void) { return model; };
	void setModel(Model *m) { model = m; };

	SceneNodeMesh *getMesh(void) { return mesh; };
	void setMesh(SceneNodeMesh *m) { mesh = m; };

	void ensureDataLoaded(void);

	bool getIsVolume(void) { return isVolume; };

	double getColorR(void) { return colorRGB[0]; };
	double getColorG(void) { return colorRGB[1]; };
	double getColorB(void) { return colorRGB[2]; };

	virtual void getPrefetch(std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs) override;

	virtual bool isItem3D(void) const override { return true; };

	void setDisplayTetEdges(bool displayEdges);

	void updateObjectColor(double r, double g, double b);

private:
	void updateVisibility(void);
	void loadEdgeData(void);

	std::string projectName;
	unsigned long long entityID;
	unsigned long long entityVersion;
	unsigned long long meshDataTetEdgesID;
	unsigned long long meshDataTetEdgesVersion;
	bool needsInitialization;

	Model *model;
	SceneNodeMesh *mesh;

	double colorRGB[3];
	bool isVolume;

	osg::Node   *tetEdges;

	std::vector<int> faceID;
};

