//! @file BlockConfigurationGraphicsObject.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"
#include "OTBlockEditorAPI/BlockConfigurationObject.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/Margins.h"
#include "OTGui/LengthLimitation.h"

namespace ot {

	class BLOCKEDITORAPI_API_EXPORT BlockConfigurationGraphicsObject : public ot::BlockConfigurationObject {
	public:
		BlockConfigurationGraphicsObject(const std::string& _name = std::string());
		virtual ~BlockConfigurationGraphicsObject() {};

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		void setOrientation(Orientation _orientation) { m_orientation = _orientation; };
		Orientation orientation(void) const { return m_orientation; };

		void setMargins(MarginsD& _margins) { m_margins = _margins; };
		void setMargins(double _top, double _right, double _bottom, double _left);
		const MarginsD& margins(void) const { return m_margins; };

		void setIsUserMoveable(bool _isUserMoveable = true) { m_isUserMoveable = _isUserMoveable; };
		bool isUserMoveable(void) const { return m_isUserMoveable; };

		void setHeightLimits(const LengthLimitation& _limit) { m_heightLimits = _limit; };
		const LengthLimitation& heightLimits(void) const { return m_heightLimits; };

		void setWidthLimits(const LengthLimitation& _limit) { m_widthLimits = _limit; };
		const LengthLimitation& widthLimits(void) const { return m_widthLimits; };

	private:
		Orientation			m_orientation;
		MarginsD			m_margins;
		bool				m_isUserMoveable;
		LengthLimitation	m_heightLimits;
		LengthLimitation	m_widthLimits;
	};

}