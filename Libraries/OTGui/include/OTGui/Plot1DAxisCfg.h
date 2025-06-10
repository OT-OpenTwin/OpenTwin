//! @file Plot1DAxisCfg.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT Plot1DAxisCfg : public Serializable {
	public:
		Plot1DAxisCfg();

		Plot1DAxisCfg(const Plot1DAxisCfg& _other) = default;
		virtual ~Plot1DAxisCfg();

		Plot1DAxisCfg& operator = (const Plot1DAxisCfg& _other) = default;

		bool operator==(const Plot1DAxisCfg& _other) const;
		bool operator!=(const Plot1DAxisCfg& _other) const;

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setMin(double _min) { m_min = _min; };
		double getMin(void) const { return m_min; };

		void setMax(double _max) { m_max = _max; };
		double getMax(void) const { return m_max; };

		void setIsLogScale(bool _logScaleEnabled) { m_isLogScale = _logScaleEnabled; };
		bool getIsLogScale(void) const { return m_isLogScale; };

		void setIsAutoScale(bool _autoScaleEnabled) { m_isAutoScale = _autoScaleEnabled; };
		bool getIsAutoScale(void) const { return m_isAutoScale; };

		bool getAutoDetermineAxisLabel() const { return m_autoDetermineAxisLabel; }
		const std::string& getAxisLabel() const { return m_axisLabel; }
		void setAutoDetermineAxisLabel(const bool _autoDetermineAxisLabel) { m_autoDetermineAxisLabel = _autoDetermineAxisLabel; }
		void setAxisLabel(const std::string& _label) { m_axisLabel = _label; }


	private:
		double m_min;
		double m_max;
		bool m_isLogScale = false;
		bool m_isAutoScale = true;
		bool m_autoDetermineAxisLabel = true;
		std::string m_axisLabel = "";
		
	};

}