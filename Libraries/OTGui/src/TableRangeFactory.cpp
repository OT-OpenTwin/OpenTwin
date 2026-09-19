// @otlicense
// File: TableRangeFactory.cpp
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

// OpenTwin header
#include "OTGui/TableRangeFactory.h"

ot::TableRangeFactory& ot::TableRangeFactory::instance(void) {
	static TableRangeFactory g_instance;
	return g_instance;
}

ot::TableRange* ot::TableRangeFactory::create(const ConstJsonObject& _jsonObject) {
	if (_jsonObject.HasMember(c_tableRangeTypeKey)) {
		return TableRangeFactory::instance().createFromJSON(_jsonObject, c_tableRangeTypeKey);
	}
	else {
		// Legacy JSON support: default to Section
		TableRangeSection* range = new TableRangeSection();
		range->setFromJsonObject(_jsonObject);
		return range;
	}
}

// Register TableRange subclasses for all range types without defines
static ot::TableRangeFactoryRegistrar<ot::TableRangeTable> registrarTable("Table");
static ot::TableRangeFactoryRegistrar<ot::TableRangeSection> registrarSection("Section");
static ot::TableRangeFactoryRegistrar<ot::TableRangeColumn> registrarColumn("Column");
static ot::TableRangeFactoryRegistrar<ot::TableRangeRow> registrarRow("Row");
static ot::TableRangeFactoryRegistrar<ot::TableRangeCell> registrarCell("Cell");
