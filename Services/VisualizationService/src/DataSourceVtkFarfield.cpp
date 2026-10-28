// @otlicense
// File: DataSourceVtkFarfield.cpp
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

#include "DataSourceVtkFarfield.h"
#include "OTModelEntities/EntityBase.h"
#include "OTModelEntities/EntityResultFarfield.h"
#include "OTModelEntities/EntityAPI.h"

#include "OTDataStorage/DataLakeHelper.h"

#include "OTCore/JSON/JSONArray.h"
#include "OTCore/JSON/JSONObject.h"
#include "OTCore/JSON/JSONHelper.h"

#include <vtkNew.h>
#include <vtkStructuredGrid.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGridReader.h>
#include <vtkXMLRectilinearGridReader.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkTriangle.h>
#include <vtkTetra.h>

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <complex>
#include <cstddef>

#undef GetObject

DataSourceVtkFarfield::DataSourceVtkFarfield()
{
}

DataSourceVtkFarfield::~DataSourceVtkFarfield()
{
	FreeMemory();
}

bool DataSourceVtkFarfield::loadData(EntityBase* resultEntity, EntityBase* meshEntity)
{
	EntityResultFarfield* farfield = dynamic_cast<EntityResultFarfield*>(resultEntity);

	if (farfield != nullptr)
	{
		return loadData(farfield);
	}

	assert(0);
	return false;
}

bool DataSourceVtkFarfield::loadData(EntityResultFarfield* farfield)
{
    assert(farfield != nullptr);

	FreeMemory();

    EntityPropertiesInteger* thetaStepsProperty = dynamic_cast<EntityPropertiesInteger*>(farfield->getProperties().getProperty("Theta steps"));
    EntityPropertiesInteger* phiStepsProperty = dynamic_cast<EntityPropertiesInteger*>(farfield->getProperties().getProperty("Phi steps"));
    assert(thetaStepsProperty != nullptr);
    assert(phiStepsProperty != nullptr);

    int thetaSteps = thetaStepsProperty->getValue();
    int phiSteps = phiStepsProperty->getValue();

	DataLakeHelper dataLakeHelper;
	mongocxx::options::find options;
	std::string log;
	ot::JsonDocument result = dataLakeHelper.executeQuery(farfield->getDataLakeAccessCfg(), options, log);
    std::string resultString = ot::json::toJson(result);

    std::map<std::string, std::list<std::string>> varyingParameters = findVaryingParameters(result);

    if (!varyingParameters.empty())
    {
        // Report error message about varying parameters
        assert(0);
        return false;
    }

    // including theta = 180 and phi = 360.
    theta = std::vector<double>(thetaSteps + 1);
    phi = std::vector<double>(phiSteps + 1);

    // create angle fields in degrees.
    for (int i = 0; i <= thetaSteps; ++i)
    {
	    theta[i] = 180.0 * i / thetaSteps;
    }

    for (int j = 0; j <= phiSteps; ++j)
    {
	    phi[j] = 360.0 * j / phiSteps;
    }

    radius = readRadius(result, thetaSteps, phiSteps, 1e-8, farfield->getComplex());

	return true;
}

void DataSourceVtkFarfield::FreeMemory(void)
{
	radius.clear();
	theta.clear();
	phi.clear();
}

// Compare numeric values without a tolerance. In particular, 15 and 15.0
// compare equal, but large integer IDs must not be rounded to double.
bool DataSourceVtkFarfield::equalValues(const ot::JsonValue& a, const ot::JsonValue& b)
{
    if (a.GetType() != b.GetType())
        return false;

    if (a.IsNumber())
    {
        if (a.IsDouble() || b.IsDouble())
        {
            return a.IsLosslessDouble() && b.IsLosslessDouble()
                && a.GetDouble() == b.GetDouble();
        }
        if (a.IsInt64() && b.IsInt64())
            return a.GetInt64() == b.GetInt64();
        if (a.IsUint64() && b.IsUint64())
            return a.GetUint64() == b.GetUint64();
        return false;
    }

    if (a.IsArray())
    {
        if (a.Size() != b.Size())
            return false;
        for (ot::JsonSizeType i = 0; i < a.Size(); ++i)
        {
            if (!equalValues(a[i], b[i]))
                return false;
        }
        return true;
    }

    if (a.IsObject())
    {
        if (a.MemberCount() != b.MemberCount())
            return false;
        for (auto it = a.MemberBegin(); it != a.MemberEnd(); ++it)
        {
            const auto other = b.FindMember(it->name);
            if (other == b.MemberEnd() || !equalValues(it->value, other->value))
                return false;
        }
        return true;
    }

    return a == b;
}

// Input: an already parsed JSON object containing a Data array.
// Result: only varying parameters; distinct values in first-occurrence order.
// An empty Data array returns an empty map (no evidence of variation).
// Missing parameter fields and duplicate row keys are treated as data errors.
// JSON objects used as parameter values must have unique member names.
// The document is read only; the returned map owns all its strings.
// Values are serialized with ot::json::toJson, including quotes for strings.
std::map<std::string, std::list<std::string>> DataSourceVtkFarfield::findVaryingParameters(const ot::ConstJsonObject& document, const std::set<std::string>& ignoredFields)
{
    // Check before calling getArray: the wrapper otherwise asserts and logs
    // a missing or incorrectly typed field, then returns an empty fallback.
    if (!ot::json::isArray(document, "Data"))
        throw std::invalid_argument("JSON must contain a Data array");

    const ot::ConstJsonArray data = ot::json::getArray(document, "Data");

    struct ObservedValues
    {
        ot::JsonSizeType rowCount = 0;
        std::vector<const ot::JsonValue*> distinct;
    };
    std::map<std::string, ObservedValues> observed;

    for (ot::JsonSizeType rowIndex = 0; rowIndex < data.Size(); ++rowIndex)
    {
        if (!ot::json::isObject(data, rowIndex))
            throw std::invalid_argument("Data entries must be objects");

        const ot::ConstJsonObject row = ot::json::getObject(data, rowIndex);
        std::set<std::string> rowNames;
        for (auto it = row.MemberBegin(); it != row.MemberEnd(); ++it)
        {
            const std::string name(it->name.GetString(), it->name.GetStringLength());
            if (!rowNames.insert(name).second)
                throw std::invalid_argument("Duplicate field in Data entry: " + name);

            if (ignoredFields.find(name) != ignoredFields.end())
                continue;

            auto& values = observed[name];
            ++values.rowCount;

            const auto found = std::find_if(
                values.distinct.begin(), values.distinct.end(),
                [&it](const ot::JsonValue* existing)
                {
                    return equalValues(*existing, it->value);
                });

            if (found == values.distinct.end())
                values.distinct.push_back(&it->value);
        }
    }

    std::map<std::string, std::list<std::string>> result;
    for (const auto& entry : observed)
    {
        const auto& name = entry.first;
        const auto& values = entry.second;

        if (values.rowCount != data.Size())
            throw std::invalid_argument("Parameter is missing from some Data entries: " + name);

        if (values.distinct.size() <= 1)
            continue;

        auto& outputValues = result[name];
        for (const auto* value : values.distinct)
            outputValues.push_back(ot::json::toJson(*value));
    }
    return result;
}

// Also accept an ot::JsonValue or an ot::JsonDocument directly.
std::map<std::string, std::list<std::string>> DataSourceVtkFarfield::findVaryingParameters(const ot::JsonValue& document, const std::set<std::string>& ignoredFields)
{
    if (!document.IsObject())
        throw std::invalid_argument("JSON root must be an object");

    return findVaryingParameters(document.GetObject(), ignoredFields);
}

double DataSourceVtkFarfield::getFiniteRadiusNumber(const ot::ConstJsonObject& object, const char* name, ot::JsonSizeType rowIndex)
{
    const auto member = object.FindMember(name);
    if (member == object.MemberEnd() || !member->value.IsNumber())
    {
        throw std::invalid_argument(
            "Data[" + std::to_string(rowIndex) + "]." + name
            + " must be a number");
    }

    // JsonValue::GetDouble accepts both integer and double JSON numbers.
    // ot::json::getDouble checks IsDouble and would reject integer literals.
    const double value = member->value.GetDouble();
    if (!std::isfinite(value))
    {
        throw std::invalid_argument(
            "Data[" + std::to_string(rowIndex) + "]." + name
            + " must be finite");
    }
    return value;
}

// Find the nearest index, then verify that the angle really is on the grid.
// No interpolation or modulo operation is applied: 0 and 360 stay distinct.
// Preconditions: angle is finite; steps > 0; 0 <= tolerance < half a grid step.
bool DataSourceVtkFarfield::getRadiusGridIndex(double angleDeg, double extentDeg, int steps, double toleranceDeg, std::size_t& index)
{
    if (angleDeg < -toleranceDeg || angleDeg > extentDeg + toleranceDeg)
        return false;

    const double stepDeg = extentDeg / static_cast<double>(steps);
    const double nearest = std::round(angleDeg / stepDeg);
    if (nearest < 0.0 || nearest > static_cast<double>(steps))
        return false;

    const double gridAngle = nearest == static_cast<double>(steps)
        ? extentDeg : nearest * stepDeg;
    if (std::abs(angleDeg - gridAngle) > toleranceDeg)
        return false;

    index = static_cast<std::size_t>(nearest);
    return true;
}

// thetaSteps and phiSteps are numbers of intervals, not numbers of points.
// The output is std::vector<std::vector<std::complex<double>>>[thetaIndex][phiIndex], including both endpoints.
//
// Precondition: the caller has selected ONE parameter combination
// (Frequency, Quantity, Series, etc.), e.g. using findVaryingParameters.
//
// Off-grid angles and angles outside [0,180] / [0,360] are ignored.
// The absolute tolerance is in degrees and must be smaller than half a step.
// Values is read only for accepted grid points and stored as {value, 0.0}.
// All grid points, including Phi=360, must exist in Data.
// Identical duplicates are accepted; conflicting duplicates throw.
// No averaging, seam copying or pole merging is performed.
std::vector<std::vector<std::complex<double>>> DataSourceVtkFarfield::readRadius(const ot::ConstJsonObject& document, int thetaSteps, int phiSteps, double angleToleranceDeg, bool complex)
{
    if (thetaSteps <= 0 || phiSteps <= 0)
        throw std::invalid_argument("thetaSteps and phiSteps must be positive");

    const double thetaStepDeg = 180.0 / static_cast<double>(thetaSteps);
    const double phiStepDeg = 360.0 / static_cast<double>(phiSteps);
    if (!std::isfinite(angleToleranceDeg) || angleToleranceDeg < 0.0
        || angleToleranceDeg >= 0.5 * thetaStepDeg
        || angleToleranceDeg >= 0.5 * phiStepDeg)
    {
        throw std::invalid_argument(
            "Angle tolerance must be finite, non-negative and smaller than half a grid step");
    }

    if (!ot::json::isArray(document, "Data"))
        throw std::invalid_argument("JSON must contain a Data array");
    const ot::ConstJsonArray data = ot::json::getArray(document, "Data");

    const std::size_t thetaCount = static_cast<std::size_t>(thetaSteps) + 1;
    const std::size_t phiCount = static_cast<std::size_t>(phiSteps) + 1;
    std::vector<std::vector<std::complex<double>>> radiusVector(thetaCount, std::vector<std::complex<double>>(phiCount));
    std::vector<std::vector<unsigned char>> assigned(
        thetaCount, std::vector<unsigned char>(phiCount, 0));

    for (ot::JsonSizeType rowIndex = 0; rowIndex < data.Size(); ++rowIndex)
    {
        if (!ot::json::isObject(data, rowIndex))
            throw std::invalid_argument("Data entries must be objects");
        const ot::ConstJsonObject row = ot::json::getObject(data, rowIndex);

        const double thetaValue = getFiniteRadiusNumber(row, "Theta", rowIndex);
        const double phiValue = getFiniteRadiusNumber(row, "Phi", rowIndex);
        std::size_t i = 0;
        std::size_t j = 0;

        if (!getRadiusGridIndex(thetaValue, 180.0, thetaSteps, angleToleranceDeg, i)
            || !getRadiusGridIndex(phiValue, 360.0, phiSteps, angleToleranceDeg, j))
        {
            continue;
        }

        double valueRe = 0.0;
        double valueIm = 0.0;

        if (complex)
        {
            if (!ot::json::isObject(row, "Values"))
            {
                throw std::invalid_argument("Values must be an object for complex data");
            }

            const ot::ConstJsonObject values = ot::json::getObject(row, "Values");

            valueRe = getFiniteRadiusNumber(values, "real", rowIndex);
            valueIm = getFiniteRadiusNumber(values, "imag", rowIndex);
        }
        else
        {
            valueRe = getFiniteRadiusNumber(row, "Values", rowIndex);
        }

        const std::complex<double> value(valueRe, valueIm);

        if (assigned[i][j] && radiusVector[i][j] != value)
        {
            throw std::invalid_argument("Conflicting values at radius[" + std::to_string(i) + "][" + std::to_string(j) + "]");
        }

        radiusVector[i][j] = value;
        assigned[i][j] = 1;
    }

    for (std::size_t i = 0; i < thetaCount; ++i)
    {
        for (std::size_t j = 0; j < phiCount; ++j)
        {
            if (!assigned[i][j])
            {
                throw std::invalid_argument(
                    "Missing grid point at radius[" + std::to_string(i) + "][" + std::to_string(j) + "] (Theta="
                    + std::to_string(i * thetaStepDeg) + ", Phi="
                    + std::to_string(j * phiStepDeg) + ")");
            }
        }
    }
    return radiusVector;
}

// Accept ot::JsonDocument / ot::JsonValue directly, including root validation.
std::vector<std::vector<std::complex<double>>> DataSourceVtkFarfield::readRadius(const ot::JsonValue& document, int thetaSteps, int phiSteps, double angleToleranceDeg, bool complex)
{
    if (!document.IsObject())
        throw std::invalid_argument("JSON root must be an object");
    return readRadius(document.GetObject(), thetaSteps, phiSteps, angleToleranceDeg, complex);
}
