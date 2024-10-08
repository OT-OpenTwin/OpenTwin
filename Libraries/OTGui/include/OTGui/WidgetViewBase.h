//! @file WidgetViewBase.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>
#include <string>

#pragma warning(disable:4251)

namespace ot {

	//! @class WidgetViewBase
	//! @brief General widget view information class.
	//! The WidgetViewBase holds the general information of a WidgetView.
	class OT_GUI_API_EXPORT WidgetViewBase : public ot::Serializable {
	public:
		//! @brief ViewFlags are used to describe the view behaviour in the frontend.
		enum ViewFlag {
			NoViewFlags              = 0x00, //! @brief Default value (no flags set)
			ViewIsCloseable          = 0x01, //! @brief If set the view may be closed
			ViewDefaultCloseHandling = 0x02, //! @brief If set and 'ViewIsCloseable' is set the view will close with the default close behaviour, otherwise a closeRequested signal is emitted.
			ViewIsPinnable           = 0x04, //! @brief If set the view may be pinned to the auto hide areas.
			ViewIsCentral            = 0x10, //! @brief If set the view is assumed to be a main view and therefore rather be located in the middle of the screen (Should not be mixed with ViewIsSide or ViewIsTool).
			ViewIsSide               = 0x20, //! @brief If set the view is assumed to be a main view and therefore rather be located in the middle of the screen (Should not be mixed with ViewIsCentral or ViewIsTool).
			ViewIsTool               = 0x40  //! @brief If set the view is assumed to be a main view and therefore rather be located in the middle of the screen (Should not be mixed with ViewIsCentral or ViewIsSide).
		};
		typedef Flags<ViewFlag> ViewFlags; //! /ref ViewFlag "ViewFlag"

		//! @brief The ViewDockLocation is used to determine the dock location when adding the view to the manager.
		enum ViewDockLocation {
			Default, //! @brief Default (Central) dock location.
			Left, //! @brief Left dock location.
			Top, //! @brief Top dock location.
			Right, //! @brief Right dock location.
			Bottom //! @brief Bottom dock location.
		};

		//! @brief Returns a string representation of the provided view flag.
		//! @param _flag The single view flag to be converted to a string.
		static std::string toString(ViewFlag _flag);

		//! @brief Returns The view flag represented by the provided string.
		//! @param _flag The string representing the flag (/ref toString(ViewFlag _flag) "Flag to string").
		static ViewFlag stringToViewFlag(const std::string& _flag);

		//! @brief Returns a string list contaning a string representation of every set flag in the provided view flags.
		//! @param _flags The flags to be converted to a string list.
		static std::list<std::string> toStringList(ViewFlags _flags);

		//! @brief Returns the view flags represented by the provided string list.
		//! @param _flags The string representations of the set flags (/ref toStringList(ViewFlags _flags) "Flags to string list").
		static ViewFlags stringListToViewFlags(const std::list<std::string>& _flags);

		//! @brief Returns the string representation of the provided view dock location.
		//! @param The view dock location to be converted to a string.
		static std::string toString(ViewDockLocation _dockLocation);

		//! @brief Returns the view dock location represented by the provided string.
		//! @param _dockLocation The string representing the dock location (/ref toString(ViewDockLocation _dockLocation) "Dock location to string")
		static ViewDockLocation stringToDockLocation(const std::string& _dockLocation);

		// ###########################################################################################################################################################################################################################################################################################################################

		WidgetViewBase();
		WidgetViewBase(const std::string& _nameAndTitle, ViewFlags _flags = ViewFlags());
		WidgetViewBase(const std::string& _name, const std::string& _title, ViewFlags _flags = ViewFlags());
		WidgetViewBase(const std::string& _name, const std::string& _title, ViewDockLocation _dockLocation, ViewFlags _flags = ViewFlags());
		WidgetViewBase(const WidgetViewBase& _other);
		virtual ~WidgetViewBase();

		WidgetViewBase& operator = (const WidgetViewBase& _other);

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

		//! @brief Set the widget view name.
		//! @param _name Name to set.
		void setName(const std::string& _name) { m_name = _name; };

		//! @brief Widget view name.
		const std::string& getName(void) const { return m_name; };

		//! @brief Set the widget view title.
		//! @note /ref title() "Note title()"
		void setTitle(const std::string& _title) { m_title = _title; };

		//! @brief Returns the WidgetView title.
		//! @note Note that in case of an empty title the name will be returned instead.
		const std::string& getTitle(void) const { return (m_title.empty() ? m_name : m_title); };

		//! @brief Set the widget view flag.
		//! @param _flag Flag to set.
		//! @param _active If false the flag will be unset instead.
		void setFlag(ViewFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };

		//! @brief Replace the current flags with the flags provided.
		void setFlags(ViewFlags _flags) { m_flags = _flags; };

		//! @brief Returns the current flags.
		ViewFlags getFlags(void) const { return m_flags; };

		//! @brief Set the dock location.
		//! @param _dockLocation The dock location to set.
		void setDockLocation(ViewDockLocation _dockLocation) { m_dockLocation = _dockLocation; };

		//! @brief Returns the dock location.
		ViewDockLocation getDockLocation(void) const { return m_dockLocation; };

	private:
		std::string m_name; //! @brief WidgetView name.
		std::string m_title; //! @brief WidgetView title (if empty the name will be used).

		ViewFlags m_flags; //! @brief WidgetView flags.
		ViewDockLocation m_dockLocation; //! @brief WidgetView dock location.
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::WidgetViewBase::ViewFlag)