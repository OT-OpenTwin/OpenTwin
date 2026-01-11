// @otlicense
// File: GraphicsDecorationCfg.h
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
#include "OTCore/Size2D.h"
#include "OTGui/PenCfg.h"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsDecorationCfg : public Serializable {
	public:
		enum Symbol : int32_t {
			NoSymbol,
			Triangle,
			FilledTriange,
			Arrow,
			VArrow,
			Circle,
			FilledCircle,
			Square,
			FilledSquare,
			Diamond,
			FilledDiamond,
			Cross
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Static functions

		static std::string symbolToString(Symbol _symbol);
		static Symbol stringToSymbol(const std::string& _symbol);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructors / Destructor / Assignment

		GraphicsDecorationCfg(Symbol _symbol = Symbol::NoSymbol, bool _reverse = false);
		GraphicsDecorationCfg(const GraphicsDecorationCfg& _other);
		GraphicsDecorationCfg(GraphicsDecorationCfg&& _other) noexcept;
		GraphicsDecorationCfg(const ConstJsonObject& _jsonObject);
		virtual ~GraphicsDecorationCfg();

		GraphicsDecorationCfg& operator = (const GraphicsDecorationCfg& _other);
		GraphicsDecorationCfg& operator = (GraphicsDecorationCfg&& _other) noexcept;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setSymbol(Symbol _symbol) { m_symbol = _symbol; };
		Symbol getSymbol() const { return m_symbol; };

		void setReverse(bool _reverse) { m_reverse = _reverse; };
		bool getReverse() const { return m_reverse; };

		void setSize(const Size2DD& _size) { m_size = _size; };
		void setSize(double _width, double _height) { m_size = Size2DD(_width, _height); };
		const Size2DD& getSize() const { return m_size; };

		void setOutlinePen(const PenFCfg& _pen) { m_outlinePen = _pen; };
		const PenFCfg& getOutlinePen() const { return m_outlinePen; };

		//! @brief Set the fill painter.
		//! @param _painter Pointer to the painter to set. The decoration takes ownership of the painter.
		void setFillPainter(Painter2D* _painter);
		const Painter2D* getFillPainter() const { return m_fillPainter; };

	private:
		Symbol m_symbol;
		bool m_reverse;
		Size2DD m_size;

		PenFCfg m_outlinePen;
		Painter2D* m_fillPainter;

	};

}
