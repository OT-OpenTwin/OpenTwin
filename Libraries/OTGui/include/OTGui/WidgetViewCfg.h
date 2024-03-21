//! @file WidgetViewCfg.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT WidgetViewCfg : public ot::Serializable {
	public:
		enum ViewFlags {
			NoViewFlags     = 0x00,
			ViewIsCloseable = 0x01
		};

		enum ViewDockLocation {
			Default,
			Left,
			Top,
			Right,
			Bottom
		};

		static std::string viewFlagToString(ViewFlags _flag);
		static ViewFlags stringToViewFlag(const std::string& _flag);
		static std::list<std::string> viewFlagsToStringList(ViewFlags _flags);
		static ViewFlags stringListToViewFlags(const std::list<std::string>& _flags);

		static std::string dockLocationToString(ViewDockLocation _dockLocation);
		static ViewDockLocation stringToDockLocation(const std::string& _dockLocation);

		// ###########################################################################################################################################################################################################################################################################################################################

		WidgetViewCfg();
		virtual ~WidgetViewCfg();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Pure virtual functions

		virtual std::string getViewType(void) const = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter/Getter

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& name(void) const { return m_name; };

		void setParentViewName(const std::string& _parentName) { m_parentViewName = _parentName; };
		const std::string& parentViewName(void) const { return m_parentViewName; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& title(void) const { return m_title; };

		void setFlags(ViewFlags _flags) { m_flags = _flags; };
		ViewFlags flags(void) const { return m_flags; };

		void setDockLocation(ViewDockLocation _dockLocation) { m_dockLocation = _dockLocation; };
		ViewDockLocation dockLocation(void) const { return m_dockLocation; };

	private:
		std::string m_name;
		std::string m_parentViewName;
		std::string m_title;

		ViewFlags m_flags;
		ViewDockLocation m_dockLocation;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::WidgetViewCfg::ViewFlags)