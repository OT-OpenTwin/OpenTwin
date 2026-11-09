// @otlicense
// File: View3DMenuRequestData.h
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
#include "OTGui/Event/MenuRequestData.h"

namespace ot {

	class OT_GUI_API_EXPORT View3DMenuRequestData : public ot::MenuRequestData
	{
		OT_DECL_NOCOPY(View3DMenuRequestData)
		OT_DECL_NOMOVE(View3DMenuRequestData)
	public:
		View3DMenuRequestData();
		View3DMenuRequestData(const BasicEntityInformation& _basicEntityInformation);
		View3DMenuRequestData(BasicEntityInformation&& _basicEntityInformation) noexcept;
		virtual ~View3DMenuRequestData();

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		static constexpr const char* className() noexcept { return "View3DMenuRequestData"; };
		virtual std::string getClassName() const { return View3DMenuRequestData::className(); };

	};
}