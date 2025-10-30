// @otlicense
// File: GraphicsShapeItemCfg.h
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
#include "OTGui/Path2D.h"
#include "OTGui/PenCfg.h"
#include "OTGui/GraphicsItemCfg.h"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsShapeItemCfg : public ot::GraphicsItemCfg {
		OT_DECL_NOMOVE(GraphicsShapeItemCfg)
	public:
		static std::string className() { return "GraphicsShapeItemCfg"; };
		GraphicsShapeItemCfg();
		GraphicsShapeItemCfg(const GraphicsShapeItemCfg& _other);
		virtual ~GraphicsShapeItemCfg();

		GraphicsShapeItemCfg& operator = (const GraphicsShapeItemCfg&) = delete;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		//! @brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy(void) const override { return new GraphicsShapeItemCfg(*this); };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey() const override { return GraphicsShapeItemCfg::className(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Set the outline path.
		//! @param _path Outline path to set.
		void setOutlinePath(const Path2DF& _path) { m_path = _path; };

		//! @brief Outline path reference.
		Path2DF& getOutlinePath(void) { return m_path; };

		//! @brief Outline path const reference.
		const Path2DF& getOutlinePath(void) const { return m_path; };

		//! @brief Set the background painter.
		//! The item takes ownership of the painter.
		//! @param _painter Painter to set.
		void setBackgroundPainter(Painter2D* _painter);
		
		//! @brief Background painter.
		const Painter2D* getBackgroundPainter(void) const { return m_backgroundPainter; };

		//! @brief Returns the current background painter and replaces it with the default background painter.
		//! The caller takes ownership of the returned painter.
		Painter2D* takeBackgroundPainter(void);

		//! @brief Set the outline.
		//! @param _outline Outline to set.
		void setOutline(const PenFCfg& _outline) { m_outline = _outline; };

		//! @brief Outlin.
		const PenFCfg& getOutline(void) const { return m_outline; };

		//! @brief Set the outline painter.
		//! The item takes ownership of the painter.
		void setOutlinePainter(Painter2D* _painter) { m_outline.setPainter(_painter); };

		//! @brief Outline painter.
		const Painter2D* getOutlinePainter(void) const { return m_outline.getPainter(); };

		//! @brief Set the outline width.
		//! @param _w Width to set.
		void setOutlineWidth(double _w) { m_outline.setWidth(_w); };

		//! @brief Outline width.
		double getOutlineWidth(void) const { return m_outline.getWidth(); };
		
		//! @brief Sets the fill shape.
		//! @see getFillShape
		void setFillShape(bool _fill) { m_fillShape = _fill; };

		//! @brief If fill shape is enabled the shape will be filled, otherwise only the outline will be drawn.
		bool getFillShape(void) const { return m_fillShape; };

	private:
		Path2DF m_path; //! @brief Outline path.
		Painter2D* m_backgroundPainter; //! @brief Background painter.
		PenFCfg m_outline;
		bool m_fillShape;
	};

}
