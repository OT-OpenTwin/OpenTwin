// @otlicense
// File: EntityResultCartesianMeshVtk.h
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

#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/EntityResultBase.h"
#include "OTModelEntities/EntityBinaryData.h"

class __declspec(dllexport) EntityResultCartesianMeshVtk : public EntityBase, public EntityResultBase
{
public:
	enum eQuantityType {VECTOR_COMPLEX_MAG_PHASE};

	EntityResultCartesianMeshVtk() : EntityResultCartesianMeshVtk(0, nullptr, nullptr, nullptr) {};
	EntityResultCartesianMeshVtk(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityResultCartesianMeshVtk();
	
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
	
	virtual std::string getClassName(void) const override { return "EntityResultCartesianMeshVtk"; };
	
	virtual entityType getEntityType(void) const override { return DATA;};

	void getComplexData(std::string& quantityName, eQuantityType& quantityType, std::vector<char>& dataAbs, std::vector<char>& dataArg);

	// Please note that setting the data also transfers the ownership of the EntityBinaryData objects. The objects must not be deleted outside the EntityResultUnstructuredMesh.
	void setComplexData(const std::string &quantityName, eQuantityType quantityType, EntityBinaryData*& dataAbs, EntityBinaryData*& dataArg);

private:
	
	std::string _quantityName;
	eQuantityType _quantityType = VECTOR_COMPLEX_MAG_PHASE;

	EntityBinaryData* _vtkDataAbs = nullptr;
	EntityBinaryData* _vtkDataArg = nullptr;

	long long _vtkDataIDAbs = -1;
	long long _vtkDataVersionAbs = -1;

	long long _vtkDataIDArg = -1;
	long long _vtkDataVersionArg = -1;

	void clearAllBinaryData(void);

	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};
