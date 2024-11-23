//! @file MenuClickableEntryCfg.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/MenuEntryCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT MenuClickableEntryCfg : public MenuEntryCfg {
	public:
		MenuClickableEntryCfg();
		MenuClickableEntryCfg(const std::string& _name, const std::string& _text, const std::string& _iconPath = std::string());
		MenuClickableEntryCfg(const MenuClickableEntryCfg& _other);
		virtual ~MenuClickableEntryCfg();

		MenuClickableEntryCfg& operator = (const MenuClickableEntryCfg& _other);

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText(void) const { return m_text; };

		void setIconPath(const std::string& _iconPath) { m_iconPath = _iconPath; };
		const std::string& getIconPath(void) const { return m_iconPath; };

		void setToolTip(const std::string& _toolTip) { m_toolTip = _toolTip; };
		const std::string& getToolTip(void) const { return m_toolTip; };

	private:
		std::string m_name;
		std::string m_text;
		std::string m_iconPath;
		std::string m_toolTip;
	};

}