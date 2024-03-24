//! @file WidgetView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/WidgetViewCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <string>

class QWidget;

namespace ads {
	class CDockWidget;
}

namespace ot {

	class WidgetViewManager;
	
	class OT_WIDGETS_API_EXPORT WidgetView {
		OT_DECL_NOCOPY(WidgetView)
	public:
		WidgetView();
		virtual ~WidgetView();

		// ###########################################################################################################################################################################################################################################################################################################################

		//! @brief Returns the widget that is set to this widget view.
		virtual QWidget* getViewWidget(void) = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter/Getter

		//! @brief Apply settings from the provided configuration
		//! @param _config Configuration
		virtual bool setupFromConfig(WidgetViewCfg* _config);

		//! @brief Returns the dock widget that belongs to this widget view
		ads::CDockWidget* getViewDockWidget(void) const { return m_dockWidget; };

		//! @brief Protected views wont be removed from the widget view manager when calling remove view.
		void setViewIsProtected(bool _protected = true) { m_isProtected = _protected; };

		//! @brief Protected views wont be removed from the widget view manager when calling remove view.
		bool viewIsProtected(void) const { return m_isProtected; };

		//! @brief Set the view modified state.
		//! Modified views will change the title to display an unsaved change.
		void setViewContentModified(bool _isModified);
		bool isViewContentModified(void) const { return m_isModified; };

		//! @brief Set the widget view name.
		//! Widget view names must be unique.
		void setName(const std::string& _name);
		const std::string& name(void) const { return m_name; };

		//! @brief Set the widget view title that is displayed to the user.
		//! The displayed title will change when settings content modified
		void setViewTitle(const QString& _title);

		//! @brief Returns the widget view title
		QString viewTitle(void) const { return m_title; };

		//! @brief Returns the widget view title that is currently displayed
		QString currentViewTitle(void) const;

		//! @brief Set the initial dock location that will be used by the widget view manager.
		void setInitialiDockLocation(WidgetViewCfg::ViewDockLocation _location) { m_dockLocation = _location; };
		WidgetViewCfg::ViewDockLocation initialDockLocation(void) const { return m_dockLocation; };

	protected:

		//! @brief Call this function as soon as the widget for this view is created to add it to the dock
		void addWidgetToDock(QWidget* _widget);

	private:
		friend class WidgetViewManager;

		ads::CDockWidget* m_dockWidget; //! @brief Dock widget for this widget view

		bool m_isDeletedByManager; //! @brief If false the widget will deregister from the manager upon deleting
		bool m_isProtected; //! @brief If set the widget wont be removed by the manager
		bool m_isModified; //! @brief If set the current view content was modified
		std::string m_name; //! @brief Unique name
		QString m_title; //! @brief Title
		ot::WidgetViewCfg::ViewFlags m_flags;
		ot::WidgetViewCfg::ViewDockLocation m_dockLocation;
	};

}