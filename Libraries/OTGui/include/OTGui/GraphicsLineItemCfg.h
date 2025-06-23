//! @file GraphicsLineItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Point2D.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/PenCfg.h"
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

		//! \brief Set the line origin relative to the item's position.
		//! The coordinates must be greater or equal 0.
		void setFrom(double _x, double _y) { this->setFrom(Point2DD(_x, _y)); };

		//! \brief Set the line origin relative to the item's position.
		//! The coordinates must be greater or equal 0.
		void setFrom(const Point2DD& _pt) { m_from = _pt; };

		//! \brief Line origin relative to the item's position.
		const Point2DD& getFrom(void) const { return m_from; };

		//! \brief Set the line destination relative to the item's position.
		//! The coordinates must be greater or equal 0.
		void setTo(double _x, double _y) { this->setTo(Point2DD(_x, _y)); };

		//! \brief Set the line destination relative to the item's position.
		//! The coordinates must be greater or equal 0.
		void setTo(const Point2DD& _pt) { m_to = _pt; };

		//! \brief Line destination relative to the item's position.
		const Point2DD& getTo(void) const { return m_to; };

		//! \brief Set the line width.
		//! The value must be greater or equal to 0.
		void setWidth(double _width) { m_lineStyle.setWidth(_width); };
		double getWidth(void) const { return m_lineStyle.getWidth(); };

		//! @brief Set the background painter.
		//! The item takes ownership of the painter.
		//! @param _painter Painter to set.
		void setPainter(Painter2D* _painter) { m_lineStyle.setPainter(_painter); };

		//! @brief Background painter.
		const Painter2D* getPainter(void) const { return m_lineStyle.getPainter(); };

		void setLineStyle(const PenFCfg& _style) { m_lineStyle = _style; };
		const PenFCfg& getLineStyle(void) const { return m_lineStyle; };

	private:
		Point2DD m_from; //! \brief Starting point.
		Point2DD m_to; //! \brief End point.
		PenFCfg m_lineStyle; //! \brief Line style.
	};

}
