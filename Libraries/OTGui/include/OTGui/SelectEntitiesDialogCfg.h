//! @file SelectEntitiesDialogCfg.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/NavigationTreeItem.h"
#include "OTGui/NavigationTreePackage.h"

// std header
#include <list>
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT SelectEntitiesDialogCfg : public NavigationTreePackage {
	public:
		SelectEntitiesDialogCfg();
		virtual ~SelectEntitiesDialogCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& title(void) const { return m_title; };

	private:
		std::string m_title;
	};

}