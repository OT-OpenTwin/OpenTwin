// @otlicense
// File: WidgetBaseCfg.h
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
#include "OTGui/GuiTypes.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT WidgetBaseCfg : public Serializable {
	public:
		static std::string typeStringJsonKey(void) { return "Type"; };
		
		WidgetBaseCfg();
		WidgetBaseCfg(const std::string& _name);
		WidgetBaseCfg(const WidgetBaseCfg&) = default;
		WidgetBaseCfg(WidgetBaseCfg&&) = default;
		virtual ~WidgetBaseCfg();

		WidgetBaseCfg& operator = (const WidgetBaseCfg&) = default;
		WidgetBaseCfg& operator = (WidgetBaseCfg&&) = default;

		virtual std::string getWidgetBaseTypeString(void) const = 0;

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setToolTip(const std::string& _toolTip) { m_toolTip = _toolTip; };
		const std::string& getToolTip(void) const { return m_toolTip; };

		void setLockFlag(LockType _flag, bool _set = true) { m_lockFlags.setFlag(_flag, _set); };
		void setLockFlags(const LockTypes& _flags) { m_lockFlags = _flags; };
		const LockTypes& getLockFlags() const { return m_lockFlags; };

	private:
		std::string m_name;
		std::string m_toolTip;
		LockTypes m_lockFlags;
	};

}