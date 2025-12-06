// @otlicense
// File: EntityResultUnstructuredMeshVtk.h
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
#pragma warning(disable : 4251)

#include <list>

#include "EntityBase.h"
#include "EntityResultBase.h"
#include "EntityBinaryData.h"

class __declspec(dllexport) EntityResultUnstructuredMeshVtk : public EntityBase, public EntityResultBase
{
public:
	enum eQuantityType {SCALAR, VECTOR};

	EntityResultUnstructuredMeshVtk() : EntityResultUnstructuredMeshVtk(0, nullptr, nullptr, nullptr) {};
	EntityResultUnstructuredMeshVtk(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityResultUnstructuredMeshVtk();
	
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
	
	virtual std::string getClassName(void) const override { return "EntityResultUnstructuredMeshVtk"; };
	
	virtual entityType getEntityType(void) const override { return DATA;};

	void getData(std::string& quantityName, eQuantityType &quantityType, std::vector<char>& data);

	// Please note that setting the data also transfers the ownership of the EntityBinaryData object. The object must not be deleted outside the EntityResultUnstructuredMesh.
	void setData(const std::string &quantityName, eQuantityType quantityType, EntityBinaryData*& data);

private:
	
	std::string _quantityName;
	eQuantityType _quantityType = SCALAR;

	EntityBinaryData* _vtkData = nullptr;

	long long _vtkDataID = -1;
	long long _vtkDataVersion = -1;

	void clearAllBinaryData(void);

	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};
