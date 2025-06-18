//! @file WidgetViewManager.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/WidgetViewBase.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/SelectionInformation.h"
#include "OTWidgets/WidgetViewDockManager.h"

// Qt ADS header
#include <ads/DockWidget.h>

// Qt header
#include <QtCore/qtimer.h>
#include <QtCore/qobject.h>
#include <QtCore/qbytearray.h>

// std header
#include <list>
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
		typedef std::pair<std::string, WidgetViewBase::ViewType> ViewNameTypeListEntry;
		typedef std::list<ViewNameTypeListEntry> ViewNameTypeList;
		typedef std::pair<BasicServiceInformation, ot::WidgetView*> ViewEntry;

		enum ManagerConfigFlag : uint32_t {
			NoFlags                            = 0 << 0, //! \brief No manager flags.
			InputFocusCentralViewOnFocusChange = 1 << 0, //! \brief Central views will get widget input focus when their view tab was focused.
			InputFocusSideViewOnFocusChange    = 1 << 1, //! \brief Side views will get widget input focus when their view tab was focused.
			InputFocusToolViewOnFocusChange    = 1 << 2, //! \brief Tool views will get widget input focus when their view tab was focused.
			IgnoreInputFocusOnViewInsert       = 1 << 3, //! \brief View widgets won't get input focus when a view is added.

			//! \brief Same as setting InputFocusCentralViewOnFocusChange | InputFocusSideViewOnFocusChange | InputFocusToolViewOnFocusChange
			InputFocusOnAnyViewFocusChange     = InputFocusCentralViewOnFocusChange | InputFocusSideViewOnFocusChange | InputFocusToolViewOnFocusChange,

			//! \brief Mask used to unset any input focus on focus changed.
			InputFocusOnFocusChangeMask        = ~InputFocusOnAnyViewFocusChange,


			//! \brief If enabled the manager will try to determine the best parent dock when inserting a new view.
			//! The last focused view of the same category will be used as the main source for the parent dock area. <br>
			//! If no view of the same category was ever focused the "oldest" view of the same category will be used. <br>
			//! If there does not exist any view in the same category the next higher category will be checked (Tool < Side < Central).
			UseBestAreaFinderOnViewInsert      = 1 << 4
			
		};
		typedef Flags<ManagerConfigFlag> ManagerConfigFlags;

		//! @brief Return the clobal instance
		static WidgetViewManager& instance();

		//! @brief Must be called upon startup, if no dock manager is provided a new one will be created
		void initialize(WidgetViewDockManager* _dockManager = nullptr);

		//! @brief Returns the dock manager that is managed by this widget view manager
		ads::CDockManager* getDockManager() const { return m_dockManager; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// View Management
		
		//! @brief Add the provided widget view.
		//! If a central widget view is set the provided view will be added to its dock area by taking the initial dock location into account.
		//! @param _owner Widget view owner.
		//! @param _view Widget view to add.
		//! \param _insertArea The relative area to add the view.
		bool addView(const BasicServiceInformation& _owner, WidgetView* _view, const WidgetView::InsertFlags& _insertFlags = WidgetView::InsertFlags(WidgetView::NoInsertFlags), WidgetView* _parentView = (WidgetView*)nullptr);

		//! @brief Returns the widget view with the specified name.
		//! If the view does not exists return 0.
		//! @param _entityName Widget view name.
		WidgetView* findView(const std::string& _entityName, WidgetViewBase::ViewType _type) const;

		//! \brief Returns the widget view that matches the title.
		//! Will log and assert if more than one view with the same title were found.
		//! Checks the view title and the current view title.
		WidgetView* findViewFromTitle(const std::string& _viewTitle) const;

		//! \brief Searches for the view and owner of the view.
		//! Returns true if the view was found.
		bool findViewAndOwner(const std::string& _entityName, WidgetViewBase::ViewType _type, WidgetView*& _view, BasicServiceInformation& _owner) const;

		//! @brief Returns the widget view that owns this dock
		WidgetView* getViewFromDockWidget(ads::CDockWidget* _dock) const;

		//! @brief Close and destroy the provided view.
		//! @param _view The view to close.
		void closeView(WidgetView* _view);

		//! @brief Close the widget view with the specified name.
		//! Widget views that are protected will be ignored.
		//! @param _entityName Widget view name.
		void closeView(const std::string& _entityName, WidgetViewBase::ViewType _type);

		//! @brief Close all widget views that belong to the given owner.
		//! Widget views that are protected will be ignored.
		//! @param _owner Widget view owner.
		void closeViews(const BasicServiceInformation& _owner);

		//! @brief Close all widget views.
		//! Widget views that are protected will be ignored.
		void closeViews();

		//! @brief Emits a close request for all unpinned views that match the criteria.
		//! @param _flags Flags that must be set at the view.
		//! @param _activeSelection The current selection. Views that don't have at least one matching selection will be considered.
		//! @param _ignoreCurrent If true the current view will be ignored.
		void requestCloseUnpinnedViews(const WidgetViewBase::ViewFlags& _flags, const SelectionInformation& _activeSelection, bool _ignoreCurrent);

		//! @brief Removes the provided widget view from the manager.
		//! The caller takes ownership.
		//! @param _view View to forget.
		void forgetView(WidgetView* _view);

		//! @brief Remove the widget view with the given name from the manager.
		//! The caller takes ownership of the view.
		//! @param _entityName Widget view name.
		WidgetView* forgetView(const std::string& _entityName, WidgetViewBase::ViewType _type);

		//! @brief Renames the view with the given name.
		void renameView(const std::string& _oldEntityName, const std::string _newEntityName);

		// ###########################################################################################################################################################################################################################################################################################################################

		// View manipulation

		//! @brief Sets the specified widget view as the current tab in its tab area.
		//! @param _entityName Widget view name.
		void setCurrentView(const std::string& _entityName, WidgetViewBase::ViewType _type);

		//! \brief Sets the current view title.
		void setCurrentViewFromTitle(const std::string& _viewTitle);

		//! @brief Saves the current state that can be restored later.
		//! @param _version State version (see restoreState).
		std::string saveState(int _version = 0);

		//! @brief Restores the state.
		//! @param _state State to restore.
		//! @param _version State version. If the versions mismatch the restore state will cancel and return false.
		bool restoreState(std::string _state, int _version = 0);

		//! @brief Restore the stored initial state.
		//! An initial state is save the first time a state is saved or restored.
		void applyInitialState();

		void setConfigFlag(ManagerConfigFlag _flag, bool _active = true) { m_config.setFlag(_flag, _active); };
		void setConfigFlags(const ManagerConfigFlags& _flags) { m_config = _flags; };
		const ManagerConfigFlags& getConfigFlags() const { return m_config; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Information gathering

		//! @brief Returns the owner for the given view.
		//! Returns default BasicServiceInformation if the view is not stored in this manager.
		BasicServiceInformation getOwnerFromView(WidgetView* _view) const;

		//! @brief Returns the widget view from the provided widget.
		//! This only works if the view was registered.
		WidgetView* findViewFromWidget(QWidget* _widget) const;

		//! @brief Returns true if a view with the given name exists
		//! @param _entityName Name to check
		bool getViewExists(const std::string& _entityName, WidgetViewBase::ViewType _type) const;

		//! @brief Returns true if a view with the given title (or current title) exists
		//! @param _title Title to check
		bool getViewTitleExists(const std::string& _title) const;

		//! @brief Returns all view names that belong to the given owner.
		ViewNameTypeList getViewNamesFromOwner(const BasicServiceInformation& _owner) const;

		//! @brief Returns all view names with the given type that belong to the given owner.
		std::list<std::string> getViewNamesFromOwner(const BasicServiceInformation& _owner, WidgetViewBase::ViewType _type) const;

		//! \brief Returns true if the content of any of the views is modified.
		bool getAnyViewContentModified();

		//! @brief Return the dock toggle action
		QAction* getDockToggleAction() const { return m_dockToggleRoot; };

		WidgetView* getCurrentlyFocusedView() const;

		WidgetView* getLastFocusedView() const { return m_focusInfo.last; };
		WidgetView* getLastFocusedSideView() const { return m_focusInfo.lastSide; };
		WidgetView* getLastFocusedToolView() const { return m_focusInfo.lastTool; };
		WidgetView* getLastFocusedCentralView() const { return m_focusInfo.lastCentral; };

	Q_SIGNALS:
		void viewFocusChanged(WidgetView* _focusedView, WidgetView* _previousView);
		void viewCloseRequested(WidgetView* _view);
		void viewTabClicked(WidgetView* _view);
		void viewDataModifiedChanged(ot::WidgetView* _view);

	public Q_SLOTS:
		void slotViewFocused(ads::CDockWidget* _oldFocus, ads::CDockWidget* _newFocus);

	private Q_SLOTS:
		void slotViewCloseRequested();
		void slotUpdateViewVisibility();
		void slotViewTabClicked();
		void slotViewDataModifiedChanged();
		void slotCloseUnpinnedViews();

	private:
		enum ManagerState {
			DefaultState        = 0 << 0, //! \brief Default manager state.
			InsertViewState     = 1 << 0, //! \brief View insert in progress.
			CloseViewState      = 1 << 1, //! \brief View Remove in progress.
			MulticloseViewState = 1 << 2 //! \brief Multiple views are closed at the same time.
		};
		typedef Flags<ManagerState> ManagerStateFlags;

		struct FocusInfo {
			WidgetView* last;
			WidgetView* lastCentral;
			WidgetView* lastSide;
			WidgetView* lastTool;
		};

		struct AutoCloseInfo {
			WidgetViewBase::ViewFlags flags;
			SelectionInformation activeSelection;
			bool ignoreCurrent;
		};

		WidgetViewManager();
		~WidgetViewManager();

		bool getViewExists(const ViewNameTypeListEntry& _entry) const;

		WidgetView* findView(const ViewNameTypeListEntry& _entry) const;

		//! \brief Adds the view and stores the information.
		//! The view's dock widget will get the IconManager::getApplicationIcon() set.
		//! \param _parentArea The target area to add the view to.
		//! \param _insertArea The relative area to add the view.
		bool addViewImpl(const BasicServiceInformation& _owner, WidgetView* _view, const WidgetView::InsertFlags& _insertFlags, WidgetView* _parentView);

		ads::CDockAreaWidget* getBestDockArea(const WidgetView* _view) const;
		ads::CDockAreaWidget* getFirstMatchingView(WidgetViewBase::ViewFlag _viewTypeFlag) const;

		ViewNameTypeList* findViewNameTypeList(const BasicServiceInformation& _owner);
		ViewNameTypeList* findOrCreateViewNameTypeList(const BasicServiceInformation& _owner);

		void handleViewCloseRequest(WidgetView* _view);

		WidgetViewDockManager* m_dockManager; //! @brief Dock manager managed by this manager
		QAction*           m_dockToggleRoot; //! @brief Action containing the toggle dock visibility menu and actions
		QTimer m_autoCloseTimer;

		ManagerStateFlags m_state;
		ManagerConfigFlags m_config;
		std::string m_initialState;
		int m_initialStateVersion;

		FocusInfo        m_focusInfo;

		AutoCloseInfo    m_autoCloseInfo;

		std::map<BasicServiceInformation, ViewNameTypeList*> m_viewOwnerMap; //! @brief Maps owners to widget view names and types
		std::list<ViewEntry> m_views; //! @brief Contains all views and their owners.

		WidgetViewDockComponentsFactory* m_dockComponentsFactory;

		OT_ADD_FRIEND_FLAG_FUNCTIONS(ManagerState)
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::WidgetViewManager::ManagerConfigFlag)