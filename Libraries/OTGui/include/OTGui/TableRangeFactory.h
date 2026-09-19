// @otlicense
// File: TableRangeFactory.h
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

// OpenTwin header
#include "OTCore/FactoryTemplate.h"
#include "OTGui/TableRange.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class OT_GUI_API_EXPORT TableRangeFactory : public FactoryTemplate<TableRange> {
	public:
		static TableRangeFactory& instance();
		
		//! @brief Creates a TableRange instance according to the key in the provided JSON object.
		//! The caller takes ownership of the created object.
		OT_DECL_NODISCARD static TableRange* create(const ConstJsonObject& _jsonObject);

	private:
		TableRangeFactory() = default;
		~TableRangeFactory() = default;
	};

	template <class T>
	class OT_GUI_API_EXPORT TableRangeFactoryRegistrar : public FactoryRegistrarTemplate<TableRangeFactory, T> {
	public:
		TableRangeFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<TableRangeFactory, T>(_key) {};
	};
}
