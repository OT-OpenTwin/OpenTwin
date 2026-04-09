// @otlicense
// File: EntityResultVtkTime.h
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

class __declspec(dllexport) EntityResultVtkTime : public EntityBase, public EntityResultBase
{
public:
	EntityResultVtkTime() : EntityResultVtkTime(0, nullptr, nullptr, nullptr) {};
	EntityResultVtkTime(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityResultVtkTime();
	
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
	
	virtual std::string getClassName(void) const override { return "EntityResultVtkTime"; };
	
	virtual entityType getEntityType(void) const override { return DATA;};

	void getTimeData(double time, std::string& quantityName, std::vector<char>& data);

	// Please note that setting the data also transfers the ownership of the EntityBinaryData objects. The objects must not be deleted outside the EntityResultUnstructuredMesh.
	void setTimeData(const std::string &quantityName, const std::list<std::pair<ot::UID, ot::UID>> &dataEntityList, const std::list<double> &dataEntityTimeList);

	void setScaleFactor(double value) { _scaleFactor = value; }
	double getScaleFactor(void) { return _scaleFactor; }

private:
	std::pair<ot::UID, ot::UID> findClosestDataItem(double time);
	
	std::string _quantityName;

	std::list<std::pair<ot::UID, ot::UID>> _dataEntityList;
	std::list<double>  _dataEntityTimeList;

	double _scaleFactor = 1.0;

	EntityBinaryData* _vtkData = nullptr;
	double _currentTime = 0.0;

	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
};
