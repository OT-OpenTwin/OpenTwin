//! @file WidgetViewBase.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/BasicEntityInformation.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>
#include <string>

#pragma warning(disable:4251)

namespace ot {

	//! @class WidgetViewBase
	//! @brief General widget view information class.
	//! The WidgetViewBase holds the general information of a WidgetView.
	class OT_GUI_API_EXPORT WidgetViewBase : public BasicEntityInformation {
		OT_DECL_NODEFAULT(WidgetViewBase)
	public:
		//! @brief ViewFlags are used to describe the view behaviour in the frontend.
		enum ViewFlag {
			NoViewFlags              = 0 << 0, //! @brief Default value (no flags set)
			ViewIsCloseable          = 1 << 0, //! @brief If set the view may be closed
			ViewDefaultCloseHandling = 1 << 1, //! @brief If set and 'ViewIsCloseable' is set the view will close with the default close behaviour (close the view), otherwise a closeRequested signal is emitted.
			ViewIsPinnable           = 1 << 2, //! @brief If set the view may be pinned to the auto hide areas.
			ViewIsCentral            = 1 << 3, //! @brief If set the view is assumed to be a main view and therefore rather be located in the middle of the screen (Should not be mixed with ViewIsSide or ViewIsTool).
			ViewIsSide               = 1 << 4, //! @brief If set the view is assumed to be a side view and therefore rather be located on the outer dock areas of the screen (Should not be mixed with ViewIsCentral or ViewIsTool).
			ViewIsTool               = 1 << 5, //! @brief If set the view is assumed to be a tool view and therefore rather be located on the outer dock areas of the screen (Should not be mixed with ViewIsCentral or ViewIsSide).
			ViewNameAsTitle          = 1 << 6  //! @brief If set the view name is going to be used as the view title.
		};
		typedef Flags<ViewFlag> ViewFlags; //! /ref ViewFlag "ViewFlag"

		//! @brief The ViewDockLocation is used to determine the dock location when adding the view to the manager.
		enum ViewDockLocation {
			Default, //! @brief Default (Central) dock location.
			Left,    //! @brief Left dock location.
			Top,     //! @brief Top dock location.
			Right,   //! @brief Right dock location.
			Bottom   //! @brief Bottom dock location.
		};

		enum ViewType {
			View3D,             //! @brief 3D view.
			View1D,             //! @brief 1D plot view.
			ViewText,           //! @brief Text editor view.
			ViewTable,          //! @brief Table view.
			ViewVersion,        //! @brief Version graph view.
			ViewGraphics,       //! @brief Graphics view.
			ViewGraphicsPicker, //! @brief Graphics picker view.
			ViewProperties,     //! @brief Properties view.
			ViewNavigation,     //! @brief Navigation view.
			CustomView          //! @brief Custom view.
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

		//! @brief Returns the string representation of the provided view type.
		static std::string toString(ViewType _type);

		//! @brief Returns the view type represented by the provided string.
		static ViewType stringToViewType(const std::string& _type);

		// ###########################################################################################################################################################################################################################################################################################################################

		WidgetViewBase(ViewType _type, ViewFlags _flags = ViewFlags());
		WidgetViewBase(const std::string& _entityName, const std::string& _title, ViewType _type, ViewFlags _flags = ViewFlags());
		WidgetViewBase(const std::string& _entityName, const std::string& _title, ViewDockLocation _dockLocation, ViewType _type, ViewFlags _flags = ViewFlags());
		WidgetViewBase(const WidgetViewBase& _other);
		virtual ~WidgetViewBase();

		WidgetViewBase& operator = (const WidgetViewBase& _other);

		bool operator == (const WidgetViewBase& _other) const;
		bool operator != (const WidgetViewBase& _other) const;

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

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& getTitle(void) const { return m_title; };

		//! @brief Set the widget view flag.
		//! @param _flag Flag to set.
		//! @param _active If false the flag will be unset instead.
		void setViewFlag(ViewFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };

		//! @brief Replace the current flags with the flags provided.
		void setViewFlags(ViewFlags _flags) { m_flags = _flags; };

		//! @brief Returns the current flags.
		ViewFlags getViewFlags(void) const { return m_flags; };

		void setViewType(ViewType _type) { m_type = _type; };
		ViewType getViewType(void) const { return m_type; };

		//! @brief Set the dock location.
		//! @param _dockLocation The dock location to set.
		void setDockLocation(ViewDockLocation _dockLocation) { m_dockLocation = _dockLocation; };

		//! @brief Returns the dock location.
		ViewDockLocation getDockLocation(void) const { return m_dockLocation; };

		void setEntityInformation(const BasicEntityInformation& _info);

	private:
		std::string m_title; //! @brief WidgetView title (if empty the name will be used).

		ViewFlags m_flags; //! @brief WidgetView flags.
		ViewType m_type; //! @brief View Type
		ViewDockLocation m_dockLocation; //! @brief WidgetView dock location.
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::WidgetViewBase::ViewFlag)