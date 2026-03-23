// @otlicense
// File: Painter2DFactory.h
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
#include "OTGui/Painter/Painter2D.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class OT_GUI_API_EXPORT Painter2DFactory : public FactoryTemplate<Painter2D> {
	public:
		static Painter2DFactory& instance();
		
		//! @brief Creates a Painter2D instance according to the key in the provided JSON object.
		//! The caller takes ownership of the created object.
		OT_DECL_NODISCARD static Painter2D* create(const ConstJsonObject& _jsonObject);

	private:
		Painter2DFactory() = default;
		~Painter2DFactory() = default;
	};

	template <class T>
	class OT_GUI_API_EXPORT Painter2DFactoryRegistrar : public FactoryRegistrarTemplate<Painter2DFactory, T> {
	public:
		Painter2DFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<Painter2DFactory, T>(_key) {};
	};
}
