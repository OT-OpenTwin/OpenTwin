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
class QAction;

namespace ads {
	class CDockWidget;
}

namespace ot {

	class WidgetViewManager;
	
	//! @class WidgetView
	//! @brief The WidgetView class is used to integrate the Qt-ADS functionallity into open twin.
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

		//! @brief Apply settings from the provided configuration.
		//! @param _config Configuration.
		virtual bool setupFromConfig(WidgetViewCfg* _config);

		void setViewData(const WidgetViewBase& _data);
		const WidgetViewBase& viewData(void) const { return m_data; };

		//! @brief Returns the dock widget that belongs to this widget view.
		ads::CDockWidget* getViewDockWidget(void) const { return m_dockWidget; };

		//! @brief Returns the dock widget toggle visibility action.
		//! Returns 0 if no widget view is set
		QAction* getViewToggleAction(void) const;

		//! @brief Protected views wont be removed from the widget view manager when calling remove view.
		void setViewIsProtected(bool _protected = true) { m_isProtected = _protected; };

		//! @brief Protected views wont be removed from the widget view manager when calling remove view.
		bool viewIsProtected(void) const { return m_isProtected; };

		//! @brief Set the view modified state.
		//! Modified views will change the title to display an unsaved change.
		void setViewContentModified(bool _isModified);
		bool isViewContentModified(void) const { return m_isModified; };

		//! @brief Returns the widget view title that is currently displayed
		QString currentViewTitle(void) const;

	protected:

		//! @brief Call this function as soon as the widget for this view is created to add it to the dock
		void addWidgetToDock(QWidget* _widget);

	private:
		friend class WidgetViewManager;

		ads::CDockWidget* m_dockWidget; //! @brief Dock widget for this widget view

		WidgetViewBase m_data;
		bool m_isDeletedByManager; //! @brief If false the widget will deregister from the manager upon deleting
		bool m_isProtected; //! @brief If set the widget wont be removed by the manager
		bool m_isModified; //! @brief If set the current view content was modified
	};

}