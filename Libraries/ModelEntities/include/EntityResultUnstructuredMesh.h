// @otlicense
// File: EntityResultUnstructuredMesh.h
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
#include "OldTreeIcon.h"
#include "EntityBinaryData.h"

class __declspec(dllexport) EntityResultUnstructuredMesh : public EntityBase
{
public:
	EntityResultUnstructuredMesh() : EntityResultUnstructuredMesh(0, nullptr, nullptr, nullptr, "") {};
	EntityResultUnstructuredMesh(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityResultUnstructuredMesh();
	
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
	
	virtual std::string getClassName(void) const override { return "EntityResultUnstructuredMesh"; };
	
	virtual entityType getEntityType(void) const override { return DATA;};

	void GetPointCoordData(size_t& numberPoints, double *&x, double*& y, double*& z);
	void GetCellData(size_t& numberCells, size_t& sizeCellData, int*& cells);

	// Please note that setting the data also transfers the ownership of the EntityBinaryData objects. The objects must not be deleted outside the EntityResultUnstructuredMesh.
	void setMeshData(size_t numberPoints, size_t numberCells, size_t sizeCellData, EntityBinaryData*& xcoord, EntityBinaryData*& ycoord, EntityBinaryData*& zcoord, EntityBinaryData*& cellData);

private:
	
	long long _numberPoints = 0;
	long long _numberCells  = 0;
	long long _sizeCellData = 0;

	EntityBinaryData* _xcoord = nullptr;
	EntityBinaryData* _ycoord = nullptr;
	EntityBinaryData* _zcoord = nullptr;

	EntityBinaryData* _cellData = nullptr;

	long long _xCoordDataID = -1;
	long long _xCoordDataVersion = -1;
	long long _yCoordDataID = -1;
	long long _yCoordDataVersion = -1;
	long long _zCoordDataID = -1;
	long long _zCoordDataVersion = -1;

	long long _cellDataID = -1;
	long long _cellDataVersion = -1;

	void clearAllBinaryData(void);
	void updateIDFromObjects(void);

	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};