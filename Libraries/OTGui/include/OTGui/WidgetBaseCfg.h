//! @file WidgetBaseCfg.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
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

	private:
		std::string m_name;
		std::string m_toolTip;
	};

}