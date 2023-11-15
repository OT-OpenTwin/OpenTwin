//! @file GraphicsItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItemCfg "OT_GICElli"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORTONLY GraphicsEllipseItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsEllipseItemCfg(double _radiusX = 5., double _radiusY = 5., ot::Painter2D* _backgroundPainter = (ot::Painter2D*)nullptr);
		virtual ~GraphicsEllipseItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItemCfg); };

		void setRadiusX(double _radius) { m_radiusX = _radius; };
		double radiusX(void) const { return m_radiusX; };
		void setRadiusY(double _radius) { m_radiusY = _radius; };
		double radiusY(void) const { return m_radiusY; };

		void setBorder(const ot::Border& _border) { m_border = _border; };
		const ot::Border& border(void) const { return m_border; };

		void setBackgroundPainer(ot::Painter2D* _painter);
		ot::Painter2D* backgroundPainter(void) { return m_backgroundPainter; };

	private:
		double m_radiusX;
		double m_radiusY;
		ot::Border m_border;
		ot::Painter2D* m_backgroundPainter;

		GraphicsEllipseItemCfg(GraphicsEllipseItemCfg&) = delete;
		GraphicsEllipseItemCfg& operator = (GraphicsEllipseItemCfg&) = delete;
	};

}
