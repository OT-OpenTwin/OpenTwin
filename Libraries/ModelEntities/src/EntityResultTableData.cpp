// @otlicense
// File: EntityResultTableData.cpp
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

#include "EntityResultTableData.h"

static EntityFactoryRegistrar<EntityResultTableData<int32_t>> registrar32(EntityResultTableData<int32_t>::classNameBase() + "_" + ot::TypeNames::getInt32TypeName());
static EntityFactoryRegistrar<EntityResultTableData<int64_t>> registrar64(EntityResultTableData<int64_t>::classNameBase() + "_" + ot::TypeNames::getInt64TypeName());
static EntityFactoryRegistrar<EntityResultTableData<double>> registrarDouble(EntityResultTableData<double>::classNameBase() + "_" + ot::TypeNames::getDoubleTypeName());
static EntityFactoryRegistrar<EntityResultTableData<std::string>> registrarString(EntityResultTableData<std::string>::classNameBase() + "_" + ot::TypeNames::getStringTypeName());