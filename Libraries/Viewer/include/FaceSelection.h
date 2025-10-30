// @otlicense
// File: FaceSelection.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
