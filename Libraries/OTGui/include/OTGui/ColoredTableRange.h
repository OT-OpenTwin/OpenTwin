// @otlicense
// File: ColoredTableRange.h
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
#include "OTCore/Color.h"
#include "OTCore/Serializable.h"
#include "OTGui/TableRange.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <memory>

namespace ot {

	// Serialization keys - no defines
	inline constexpr const char* c_coloredTableRangeRangeKey = "Range";
	inline constexpr const char* c_coloredTableRangeColorKey = "Color";

	//! @class ColoredTableRange
	//! @brief Associates an ot::TableRange (polymorphic) with an ot::Color. Maintains value semantics via deep cloning.
	class OT_GUI_API_EXPORT ColoredTableRange : public ot::Serializable {
	public:
		ColoredTableRange();
		ColoredTableRange(const TableRange& _range, const Color& _color);
		ColoredTableRange(std::shared_ptr<TableRange> _range, const Color& _color);
		ColoredTableRange(const ColoredTableRange& _other);
		ColoredTableRange(ColoredTableRange&& _other) noexcept;
		virtual ~ColoredTableRange() = default;

		ColoredTableRange& operator=(const ColoredTableRange& _other);
		ColoredTableRange& operator=(ColoredTableRange&& _other) noexcept;

		bool operator==(const ColoredTableRange& _other) const;
		bool operator!=(const ColoredTableRange& _other) const { return !(*this == _other); }

		// Inherited via Serializable
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		const TableRange& getRange() const { return *m_range; }
		TableRange& getRange() { return *m_range; }
		std::shared_ptr<TableRange> getRangeShared() const { return m_range; }

		void setRange(const TableRange& _range);
		void setRange(std::shared_ptr<TableRange> _range);

		const Color& getColor() const { return m_color; }
		Color& getColor() { return m_color; }
		void setColor(const Color& _color) { m_color = _color; }

	private:
		std::shared_ptr<TableRange> m_range;
		Color m_color;
	};

}
