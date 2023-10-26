//! @file LinearGradientPainter2D.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/Color.h"
#include "OTGui/Painter2D.h"

#define OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg "OT_P2DCGrad"

namespace ot {

	class OT_GUI_API_EXPORT LinearGradientPainter2DStop : public ot::Serializable {
	public:
		LinearGradientPainter2DStop();
		LinearGradientPainter2DStop(double _pos, const ot::Color& _color);
		LinearGradientPainter2DStop(const LinearGradientPainter2DStop& _other);
		virtual ~LinearGradientPainter2DStop();

		LinearGradientPainter2DStop& operator = (const LinearGradientPainter2DStop& _other);
		bool operator == (const LinearGradientPainter2DStop& _other) const;
		bool operator != (const LinearGradientPainter2DStop& _other) const;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		void setPos(double _pos) { m_pos = _pos; };
		double pos(void) const { return m_pos; };

		void setColor(const ot::Color& _color) { m_color = _color; };
		const ot::Color& color(void) const { return m_color; };

	private:
		double    m_pos;
		ot::Color m_color;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_GUI_API_EXPORT LinearGradientPainter2D : public ot::Painter2D {
	public:
		LinearGradientPainter2D();
		LinearGradientPainter2D(const std::vector<LinearGradientPainter2DStop>& _stops);
		virtual ~LinearGradientPainter2D();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg); };

		void addStop(const LinearGradientPainter2DStop& _stop);
		void addStops(const std::vector<LinearGradientPainter2DStop>& _stops);
		const std::vector<LinearGradientPainter2DStop>& stops(void) const { return m_stops; };

	private:
		std::vector<LinearGradientPainter2DStop> m_stops;

		LinearGradientPainter2D(const LinearGradientPainter2D&) = delete;
		LinearGradientPainter2D& operator = (const LinearGradientPainter2D&) = delete;
	};

}