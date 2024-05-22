//! @file GraphicsLineItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Point2D.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/Outline.h"
#include "OTGui/GraphicsItemCfg.h"

#define OT_FactoryKey_GraphicsLineItem "OT_GILine"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsLineItemCfg : public ot::GraphicsItemCfg {
		OT_DECL_NOCOPY(GraphicsLineItemCfg)
	public:
		GraphicsLineItemCfg();
		virtual ~GraphicsLineItemCfg();

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
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsLineItem); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setFrom(const Point2DD& _pt) { m_from = _pt; };
		const Point2DD& getFrom(void) const { return m_from; };

		void setTo(const Point2DD& _pt) { m_to = _pt; };
		const Point2DD& getTo(void) const { return m_to; };

		void setWidth(double _width) { m_lineStyle.setWidth(_width); };
		double getWidth(void) const { return m_lineStyle.width(); };

		//! @brief Set the background painter.
		//! The item takes ownership of the painter.
		//! @param _painter Painter to set.
		void setPainter(Painter2D* _painter) { m_lineStyle.setPainter(_painter); };

		//! @brief Background painter.
		const Painter2D* getPainter(void) const { return m_lineStyle.painter(); };

		void setLineStyle(const OutlineF& _style) { m_lineStyle = _style; };
		const OutlineF& getLineStyle(void) const { return m_lineStyle; };

	private:
		Point2DD m_from; //! \brief Starting point.
		Point2DD m_to; //! \brief End point.
		OutlineF m_lineStyle; //! \brief Line style.
	};

}
