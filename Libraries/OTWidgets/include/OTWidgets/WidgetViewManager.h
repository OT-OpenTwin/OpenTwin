//! @file WidgetViewManager.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/WidgetViewBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt ADS header
#include <ads/DockWidget.h>
#include <ads/DockManager.h>

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qbytearray.h>

// std header
#include <map>
#include <string>

class QAction;

namespace ads {
	class CDockAreaWidget;
}

namespace ot {

	class WidgetView;

	//! @brief The widget view manager is used to manage widget views.
	class OT_WIDGETS_API_EXPORT WidgetViewManager : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(WidgetViewManager)
	public:
		//! @brief Return the clobal instance
		static WidgetViewManager& instance(void);

		//! @brief Must be called upon startup, if no dock manager is provided a new one will be created
		void initialize(ads::CDockManager* _dockManager = nullptr);

		//! @brief Returns the dock manager that is managed by this widget view manager
		ads::CDockManager* getDockManager(void) const { return m_dockManager; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// View Management
		
		//! @brief Add the provided widget view.
		//! If a central widget view is set the provided view will be added to its dock area by taking the initial dock location into account.
		//! @param _owner Widget view owner.
		//! @param _view Widget view to add.
		//! \param _insertArea The relative area to add the view.
		bool addView(const BasicServiceInformation& _owner, WidgetView* _view, ads::DockWidgetArea _insertArea = ads::CenterDockWidgetArea);

		//! @brief Add the provided widget view to the specified dock area.
		//! @param _owner Widget view owner.
		//! @param _view Widget view to add.
		//! @param _parentArea The target area to add the view to.
		//! \param _insertArea The relative area to add the view.
		bool addView(const BasicServiceInformation& _owner, WidgetView* _view, ads::CDockAreaWidget* _parentArea, ads::DockWidgetArea _insertArea = ads::CenterDockWidgetArea);
		
		//! @brief Returns the widget view with the specified name.
		//! If the view does not exists return 0.
		//! @param _viewName Widget view name.
		WidgetView* findView(const std::string& _viewName) const;

		//! @brief Returns the widget view that owns this dock
		WidgetView* getViewFromDockWidget(ads::CDockWidget* _dock) const;

		//! @brief Close the widget view with the specified name.
		//! Widget views that are protected will be ignored.
		//! @param _viewName Widget view name.
		void closeView(const std::string& _viewName);

		//! @brief Close all widget views that belong to the given owner.
		//! Widget views that are protected will be ignored.
		//! @param _owner Widget view owner.
		void closeViews(const BasicServiceInformation& _owner);

		//! @brief Close all widget views.
		//! Widget views that are protected will be ignored.
		void closeViews(void);

		//! @brief Removes the provided widget view from the manager.
		//! The caller takes ownership.
		//! @param _view View to forget.
		void forgetView(WidgetView* _view);

		//! @brief Remove the widget view with the given name from the manager.
		//! The caller takes ownership of the view.
		//! @param _viewName Widget view name.
		WidgetView* forgetView(const std::string& _viewName);

		// ###########################################################################################################################################################################################################################################################################################################################

		// View manipulation

		//! @brief Sets the specified widget view as the current tab in its tab area.
		//! @param _viewName Widget view name.
		void setCurrentView(const std::string& _viewName);

		//! @brief Saves the current state that can be restored later.
		//! @param _version State version (see restoreState).
		std::string saveState(int _version = 0) const;

		//! @brief Restores the state.
		//! @param _state State to restore.
		//! @param _version State version. If the versions mismatch the restore state will cancel and return false.
		bool restoreState(std::string _state, int _version = 0);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Information gathering

		//! @brief Returns true if a view with the given name exists
		//! @param _viewName Name to check
		bool getViewExists(const std::string& _viewName) const;

		//! @brief Returns true if a view with the given title (or current title) exists
		//! @param _title Title to check
		bool getViewTitleExists(const QString& _title) const;

		//! \brief Returns true if the content of any of the views is modified.
		bool getAnyViewContentModified(void);

		//! @brief Return the dock toggle action
		QAction* getDockToggleAction(void) const { return m_dockToggleRoot; };

		WidgetView* getCurrentlyFocusedView(void) const;

		WidgetView* getLastFocusedView(void) const { return m_focusInfo.last; };
		WidgetView* getLastFocusedSideView(void) const { return m_focusInfo.lastSide; };
		WidgetView* getLastFocusedToolView(void) const { return m_focusInfo.lastTool; };
		WidgetView* getLastFocusedCentralView(void) const { return m_focusInfo.lastCentral; };

	Q_SIGNALS:
		void viewFocusLost(WidgetView* _view);
		void viewFocused(WidgetView* _view);
		void viewCloseRequested(WidgetView* _view);

	private Q_SLOTS:
		void slotViewFocused(ads::CDockWidget* _oldFocus, ads::CDockWidget* _newFocus);
		void slotViewCloseRequested(void);
		void slotUpdateViewVisibility(void);

	private:
		WidgetViewManager();
		~WidgetViewManager();

		//! \brief Adds the view and stores the information.
		//! The view's dock widget will get the IconManager::getApplicationIcon() set.
		//! \param _parentArea The target area to add the view to.
		//! \param _insertArea The relative area to add the view.
		bool addViewImpl(const BasicServiceInformation& _owner, WidgetView* _view, ads::CDockAreaWidget* _parentArea, ads::DockWidgetArea _insertArea);

		ads::CDockAreaWidget* determineBestParentArea(WidgetView* _newView) const;

		ads::CDockAreaWidget* determineBestRestoreArea(WidgetView* _view) const;

		ads::CDockAreaWidget* determineBestRestoreArea(WidgetView* _view, WidgetViewBase::ViewFlag _viewType) const;

		std::list<std::string>* findViewNameList(const BasicServiceInformation& _owner);
		std::list<std::string>* findOrCreateViewNameList(const BasicServiceInformation& _owner);

		ads::CDockManager* m_dockManager; //! @brief Dock manager managed by this manager
		QAction*           m_dockToggleRoot; //! @brief Action containing the toggle dock visibility menu and actions
		struct FocusInfo {
			WidgetView* last;
			WidgetView* lastCentral;
			WidgetView* lastSide;
			WidgetView* lastTool;
		};
		FocusInfo        m_focusInfo;

		std::map<BasicServiceInformation, std::list<std::string>*> m_viewOwnerMap; //! @brief Maps owners to widget view names
		std::map<std::string, std::pair<BasicServiceInformation, WidgetView*>> m_viewNameMap; //! @brief Maps Widget view names to widget views and their owners
	};

}
