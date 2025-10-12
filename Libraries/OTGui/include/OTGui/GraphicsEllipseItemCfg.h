//! @file GraphicsEllipseItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PenCfg.h"
#include "OTGui/GraphicsItemCfg.h"

#define OT_FactoryKey_GraphicsEllipseItem "OT_GIElli"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsEllipseItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsEllipseItemCfg(double _radiusX = 5., double _radiusY = 5., ot::Painter2D* _backgroundPainter = (ot::Painter2D*)nullptr);
		virtual ~GraphicsEllipseItemCfg();

		//! @brief Creates a copy of this item.
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
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsEllipseItem); };

		void setRadiusX(double _radius) { m_radiusX = _radius; };
		double getRadiusX(void) const { return m_radiusX; };
		void setRadiusY(double _radius) { m_radiusY = _radius; };
		double getRadiusY(void) const { return m_radiusY; };

		void setOutline(const ot::PenFCfg& _outline) { m_outline = _outline; };
		const ot::PenFCfg& getOutline(void) const { return m_outline; };

		void setBackgroundPainer(ot::Painter2D* _painter);
		const ot::Painter2D* getBackgroundPainter(void) const { return m_backgroundPainter; };

	private:
		double m_radiusX;
		double m_radiusY;
		ot::PenFCfg m_outline;
		ot::Painter2D* m_backgroundPainter;

		GraphicsEllipseItemCfg(GraphicsEllipseItemCfg&) = delete;
		GraphicsEllipseItemCfg& operator = (GraphicsEllipseItemCfg&) = delete;
	};

}
