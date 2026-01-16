// @otlicense
// File: FixtureEntityWithDynamicFields.cpp
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

#include "FixtureEntityWithDynamicFields.h"

void FixtureEntityWithDynamicFields::OrderDocumentsHierarchical(EntityWithDynamicFields& entity)
{
	entity.OrderGenericDocumentsHierarchical();
}

EntityWithDynamicFields FixtureEntityWithDynamicFields::createEntityWithSampleData()
{
    EntityWithDynamicFields entity;
    using ot::Variable;

    const std::string base = "/DynamicFields";

    // -------------------------------
    // Parameter / Time(s)
    // -------------------------------
    const std::string paramId = "45999440657432576";
    const std::string paramPath = base + "/Parameter/" + paramId;

    entity.InsertInField(
        "DataTypeName",
        { Variable("double") },
        paramPath
    );

    entity.InsertInField(
        "Label",
        { Variable("Time(s)") },
        paramPath
    );

    entity.InsertInField(
        "Name",
        { Variable("Time(s)") },
        paramPath
    );

    entity.InsertInField(
        "Unit",
        { Variable("") },
        paramPath
    );

    // Values array
    std::list<Variable> values;
    constexpr double start = 5e-7;
    constexpr double step = 2.5e-10;
    constexpr int count = 1000; // matches your sequence length

    for (int i = 0; i < count; ++i)
    {
        values.emplace_back(start + i * step);
    }

    entity.InsertInField(
        "Values",
        std::move(values),
        paramPath
    );

    // -------------------------------
    // Quantity / Current(A)
    // -------------------------------
    const std::string quantityId = "45999440657432577";
    const std::string quantityPath = base + "/Quantity/" + quantityId;

    entity.InsertInField(
        "DataDimensions",
        { Variable(1) },
        quantityPath
    );

    entity.InsertInField(
        "Label",
        { Variable("Current(A)\r") },
        quantityPath
    );

    entity.InsertInField(
        "Name",
        { Variable("Current(A)\r") },
        quantityPath
    );

    // ParameterDependencies
    entity.InsertInField(
        "$numberLong",
        { Variable(paramId) },
        quantityPath + "/ParameterDependencies"
    );

    // ValueDescriptions
    const std::string valueDescPath =
        quantityPath + "/ValueDescriptions/" + quantityId;

    entity.InsertInField(
        "DataTypeName",
        { Variable("double") },
        valueDescPath
    );

    entity.InsertInField(
        "Label",
        { Variable("") },
        valueDescPath
    );

    entity.InsertInField(
        "Name",
        { Variable("") },
        valueDescPath
    );

    entity.InsertInField(
        "Unit",
        { Variable("") },
        valueDescPath
    );
    return entity;
}
