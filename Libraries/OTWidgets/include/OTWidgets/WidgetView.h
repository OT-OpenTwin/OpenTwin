//! @file WidgetView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/WidgetViewBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/SelectionInformation.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

// std header
#include <string>

class QWidget;
class QAction;

namespace ot {

	class WidgetViewDock;
	class QWidgetInterface;
	class WidgetViewManager;
	
	//! @class WidgetView
	//! @brief The WidgetView class is used to integrate the Qt-ADS functionallity into open twin.
	class OT_WIDGETS_API_EXPORT WidgetView : public QObject {
		Q_OBJECT
		OT_DECL_NODEFAULT(WidgetView)
		OT_DECL_NOCOPY(WidgetView)
	public:
		enum InsertFlag {
			NoInsertFlags    = 0 << 0,
			KeepCurrentFocus = 1 << 0
		};
		typedef ot::Flags<InsertFlag> InsertFlags;

		static std::string createStoredViewName(const WidgetViewBase& _view);

		//! \brief Creates the name that should be used in the view management logic.
		//! The resulted name has the following syntax: <entityName>$<viewTypeString>
		static std::string createStoredViewName(const std::string& _entityName, WidgetViewBase::ViewType _viewType);

		WidgetView(WidgetViewBase::ViewType _viewType);
		virtual ~WidgetView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		//! @brief Returns the widget that is set to this widget view.
		virtual QWidget* getViewWidget() = 0;

		//! @brief Sets the input focus to the view widget.
		//! Custom widget views with nested widgets can override this method to set the focus to the correct widget.
		//! By default the root widget (widget returned by WidgetView::getViewWidget()) will get the focus set.
		virtual void setViewWidgetFocus();

		//! @brief Is called when the view was renamed by the widget view manager.
		virtual void viewRenamed() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setViewData(const WidgetViewBase& _data);
		WidgetViewBase& getViewData() { return m_data; };
		const WidgetViewBase& getViewData() const { return m_data; };

		//! @brief Returns the dock widget that belongs to this widget view.
		WidgetViewDock* getViewDockWidget() const { return m_dockWidget; };

		//! @brief Returns the dock widget toggle visibility action.
		//! Returns 0 if no widget view is set
		QAction* getViewToggleAction() const;

		//! @brief Permanent views wont be removed from the widget view manager when calling remove view.
		void setViewIsPermanent(bool _permanent = true) { m_isPermanent = _permanent; };

		//! @brief Permanent views wont be removed from the widget view manager when calling remove view.
		bool getViewIsPermanent() const { return m_isPermanent; };

		//! @brief Set the view modified state.
		//! Modified views will change the title to display an unsaved change.
		void setViewContentModified(bool _isModified);
		bool getViewContentModified() const { return m_isModified; };

		//! @brief Returns the widget view title that is currently displayed
		QString getCurrentViewTitle() const;

		void openView();

		void closeView();

		void setAsCurrentViewTab();

		bool isCurrentViewTab() const;

		void setSelectionInformation(const SelectionInformation& _info) { m_selectionInfo = _info; };
		const SelectionInformation& getSelectionInformation() const { return m_selectionInfo; };

	Q_SIGNALS:
		void closeRequested();
		void viewDataModifiedChanged();

	protected:

		//! @brief Call this function as soon as the widget for this view is created to add it to the dock
		void addWidgetInterfaceToDock(QWidgetInterface* _interface);
		void addWidgetToDock(QWidget* _widget);

	private Q_SLOTS:
		void slotCloseRequested();
		void slotToggleVisible();
		void slotPinnedChanged(bool _isPinned);

	private:
		friend class WidgetViewManager;

		WidgetViewDock* m_dockWidget; //! @brief Dock widget for this widget view

		WidgetViewBase m_data;
		bool m_isDeletedByManager; //! @brief If false the widget will deregister from the manager upon deleting
		bool m_isPermanent; //! @brief If set the widget wont be removed by the manager
		bool m_isModified; //! @brief If set the current view content was modified

		SelectionInformation m_selectionInfo;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::WidgetView::InsertFlag)