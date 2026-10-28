// @otlicense
// File: DataSourceVtkFarfield.h
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
#include "OTCore/JSON/JSONArray.h"
#include "OTCore/JSON/JSONObject.h"
#include "OTCore/JSON/JSONHelper.h"

#include "VisualizationServiceTypes.h"
#include "DataSourceManagerItem.h"

#include <vtkStructuredGrid.h>
#include <vtkNew.h>

#include <string>
#include <vector>
#include <complex>
#include <map>
#include <list>
#include <set>

class EntityResultFarfield;

class DataSourceVtkFarfield : public DataSourceManagerItem
{
public:
	DataSourceVtkFarfield();
	virtual ~DataSourceVtkFarfield();

	virtual bool loadData(EntityBase *resultEntity, EntityBase *meshEntity) override;

	std::vector<double> &getTheta() { return theta; }
	std::vector<double> &getPhi() { return phi; }
	std::vector<std::vector<std::complex<double>>>& getRadius() { return radius; }

private:
	bool loadData(EntityResultFarfield* resultData);
	static bool equalValues(const ot::JsonValue& a, const ot::JsonValue& b);
	std::map<std::string, std::list<std::string>> findVaryingParameters(const ot::ConstJsonObject& document, const std::set<std::string>& ignoredFields = { "Theta", "Phi", "Values" });
	std::map<std::string, std::list<std::string>> findVaryingParameters(const ot::JsonValue& document, const std::set<std::string>& ignoredFields = { "Theta", "Phi", "Values" });
	double getFiniteRadiusNumber(const ot::ConstJsonObject& object, const char* name, ot::JsonSizeType rowIndex);
	bool getRadiusGridIndex(double angleDeg, double extentDeg, int steps, double toleranceDeg, std::size_t& index);
	std::vector<std::vector<std::complex<double>>> readRadius(const ot::ConstJsonObject& document, int thetaSteps, int phiSteps, double angleToleranceDeg, bool complex);
	std::vector<std::vector<std::complex<double>>> readRadius(const ot::JsonValue& document, int thetaSteps, int phiSteps, double angleToleranceDeg, bool complex);


	void FreeMemory();

	std::vector<double> theta;
	std::vector<double> phi;
	std::vector<std::vector<std::complex<double>>> radius;
};
