// @otlicense
// File: Plot1DAxisCfg.h
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
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#pragma warning (disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT Plot1DAxisCfg : public Serializable {
		OT_DECL_DEFCOPY(Plot1DAxisCfg)
		OT_DECL_DEFMOVE(Plot1DAxisCfg)
	public:
		Plot1DAxisCfg();
		virtual ~Plot1DAxisCfg();

		bool operator==(const Plot1DAxisCfg& _other) const;
		bool operator!=(const Plot1DAxisCfg& _other) const;

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
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