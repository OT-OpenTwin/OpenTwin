// @otlicense

//! @file Plot1DDataBaseCfg.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Plot1DCfg.h"
#include "OTGui/Plot1DCurveCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT Plot1DDataBaseCfg : public Plot1DCfg {
	public:
		Plot1DDataBaseCfg();
		Plot1DDataBaseCfg(const Plot1DDataBaseCfg& _other);
		Plot1DDataBaseCfg(const Plot1DCfg& _other);
		virtual ~Plot1DDataBaseCfg();

		Plot1DDataBaseCfg& operator = (const Plot1DDataBaseCfg& _other);
		Plot1DDataBaseCfg& operator = (const Plot1DCfg& _other);
		
		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void addCurve(const Plot1DCurveCfg& _curve);
		void addCurve(Plot1DCurveCfg&& _curve);
		void setCurves(const std::list<Plot1DCurveCfg>& _curves) { m_curves = _curves; };
		const std::list<Plot1DCurveCfg>& getCurves(void) const { return m_curves; };

		//! @brief Updates the curve version.
		//! Returns true if the version has been changed.
		bool updateCurveVersion(ot::UID _curveEntityUID, ot::UID _newCurveEntityVersion);

	private:
		std::list<Plot1DCurveCfg> m_curves;
	};

}