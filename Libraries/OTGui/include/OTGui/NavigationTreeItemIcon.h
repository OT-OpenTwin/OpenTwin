// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT NavigationTreeItemIcon : public Serializable {
	public:
		NavigationTreeItemIcon();
		NavigationTreeItemIcon(const std::string& _visibleIcon, const std::string& _hiddenIcon);
		NavigationTreeItemIcon(const NavigationTreeItemIcon& _other);
		virtual ~NavigationTreeItemIcon();

		NavigationTreeItemIcon& operator = (const NavigationTreeItemIcon& _other);

		bool operator == (const NavigationTreeItemIcon& _other) const;
		bool operator != (const NavigationTreeItemIcon& _other) const;
		
		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setVisibleIcon(const std::string& _visibleIcon) { m_visibleIcon = _visibleIcon; };
		const std::string& getVisibleIcon(void) const { return m_visibleIcon; };

		void setHiddenIcon(const std::string& _hiddenIcon) { m_hiddenIcon = _hiddenIcon; };
		const std::string& getHiddenIcon(void) const { return m_hiddenIcon; };

	private:
		std::string m_visibleIcon;
		std::string m_hiddenIcon;
	};

}
