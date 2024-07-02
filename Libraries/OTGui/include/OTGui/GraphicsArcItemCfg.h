//! @file GraphicsArcItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Rect.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/Outline.h"
#include "OTGui/GraphicsItemCfg.h"

#define OT_FactoryKey_GraphicsArcItem "OT_GIArc"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsArcItemCfg : public ot::GraphicsItemCfg {
		OT_DECL_NOCOPY(GraphicsArcItemCfg)
	public:
		GraphicsArcItemCfg();
		virtual ~GraphicsArcItemCfg();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		//! \brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy(void) const override;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsArcItem); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setRect(const Point2DD& _topLeft, const Point2DD& _bottomRight) { this->setRect(RectD(_topLeft, _bottomRight)); };
		void setRect(const Point2DD& _topLeft, const Size2DD& _size) { this->setRect(RectD(_topLeft, _size)); };
		void setRect(const RectD& _rect) { m_rect = _rect; };
		const RectD& getRect(void) const { return m_rect; };

		void setStartAngle(double _startAngle) { m_startAngle = _startAngle; };
		double getStartAngle(void) const { return m_startAngle; };

		void setSpanAngle(double _spanAngle) { m_spanAngle = _spanAngle; };
		double getSpanAngle(void) const { return m_spanAngle; };

		void setLineWidth(double _width) { m_lineStyle.setWidth(_width); };
		double getLineWidth(void) const { return m_lineStyle.width(); };

		//! @brief Set the background painter.
		//! The item takes ownership of the painter.
		//! @param _painter Painter to set.
		void setPainter(Painter2D* _painter) { m_lineStyle.setPainter(_painter); };

		//! @brief Background painter.
		const Painter2D* getPainter(void) const { return m_lineStyle.painter(); };

		void setLineStyle(const OutlineF& _style) { m_lineStyle = _style; };
		const OutlineF& getLineStyle(void) const { return m_lineStyle; };

	private:
		RectD m_rect;
		double m_startAngle;
		double m_spanAngle;
		OutlineF m_lineStyle; //! \brief Line style.
	};

}
