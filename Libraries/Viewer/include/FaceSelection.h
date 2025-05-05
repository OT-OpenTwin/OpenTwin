#pragma once
#include "SceneNodeGeometry.h"

class FaceSelection
{
public:
	FaceSelection() : modelID(0), selectedItem(nullptr), faceId(0) {}
	virtual ~FaceSelection() {}

	void setData(unsigned long long _modelID) { modelID = _modelID; }

	unsigned long long getModelID(void) const { return modelID; }

	void setSelectedItem(SceneNodeGeometry* item) { selectedItem = item; }
	SceneNodeGeometry* getSelectedItem(void) const { return selectedItem; }

	void setFaceId(unsigned long long id) { faceId = id; }
	unsigned long long getFaceId(void) const { return faceId; }

	void setFaceName(const std::string& name) { faceName = name; }
	std::string getFaceName(void) { return faceName; }

	bool operator==(const FaceSelection& other) { return (selectedItem == other.getSelectedItem() && faceId == other.getFaceId()); }

private:
	unsigned long long modelID;
	SceneNodeGeometry* selectedItem;
	unsigned long long faceId;
	std::string faceName;
};

bool operator==(const FaceSelection& left, const FaceSelection& right);
