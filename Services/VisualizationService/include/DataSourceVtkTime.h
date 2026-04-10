// @otlicense
// File: DataSourceVtkTime.h
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

#include "OTCore/CoreTypes.h"

#include "VisualizationServiceTypes.h"
#include "DataSourceManagerItem.h"

#include <vtkRectilinearGrid.h>
#include <vtkNew.h>

#include <string>
#include <vector>

class EntityResultVtkTime;

class DataSourceVtkTime : public DataSourceManagerItem
{
public:
	DataSourceVtkTime();
	virtual ~DataSourceVtkTime();

	virtual bool loadData(EntityBase *resultEntity, EntityBase *meshEntity) override;

	vtkRectilinearGrid* GetVtkGrid() { return vtkGrid; };

	double GetXMinCoordinate();
	double GetYMinCoordinate();
	double GetZMinCoordinate();
	double GetXMaxCoordinate();
	double GetYMaxCoordinate();
	double GetZMaxCoordinate();

	bool GetHasPointScalar() { return hasPointScalar; }
	bool GetHasPointVector() { return hasPointVector; }
	bool GetHasCellScalar() { return hasCellScalar; }
	bool GetHasCellVector() { return hasCellVector; }

	double getScaleFactor() { return scaleFactor; }

	void ensureDataLoaded(double time);

private:
	bool loadData(EntityResultVtkTime* resultData);
	void getTimeData(double time, std::vector<char>& data);
	std::pair<ot::UID, ot::UID> findClosestDataItem(double time);

	void FreeMemory();

	vtkRectilinearGrid* vtkGrid;

	double scaleFactor = 1.0;

	std::string quantityName;

	std::list<std::pair<ot::UID, ot::UID>> dataEntityList;
	std::list<double>  dataEntityTimeList;

	double currentTime = -1.0;

	bool hasPointScalar = false;
	bool hasCellScalar = false;
	bool hasPointVector = false;
	bool hasCellVector = false;
};
