#pragma once
#include "EdgeSelection.h"
#include "SceneNodeGeometry.h"

class EdgeSelection
{
public:
	EdgeSelection() : modelID(0), selectedItem(nullptr), faceId1(0), faceId2(0), node(nullptr) {}
	virtual ~EdgeSelection() {}

	void setData(unsigned long long _modelID) { modelID = _modelID; }
	unsigned long long getModelID(void) const { return modelID; }

	void setSelectedItem(SceneNodeGeometry* item) { selectedItem = item; }
	SceneNodeGeometry* getSelectedItem(void) const { return selectedItem; }

	void setFaceIds(unsigned long long id1, unsigned long long id2) { faceId1 = id1; faceId2 = id2; }
	unsigned long long getFaceId1(void) const { return faceId1; }
	unsigned long long getFaceId2(void) const { return faceId2; }

	void setEdgeName(const std::string& name) { edgeName = name; }
	std::string getEdgeName(void) { return edgeName; }

	bool operator==(const EdgeSelection& other) const {
		return (selectedItem == other.getSelectedItem()
			&& (faceId1 == other.getFaceId1() && faceId2 == other.getFaceId2()
				|| faceId1 == other.getFaceId2() && faceId2 == other.getFaceId1()));
	}

	void setNode(osg::Node* _node) { node = _node; };
	osg::Node* getNode() { return node; };

private:
	unsigned long long modelID;
	SceneNodeGeometry* selectedItem;
	unsigned long long faceId1;
	unsigned long long faceId2;
	osg::Node* node;
	std::string edgeName;
};