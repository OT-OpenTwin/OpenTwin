// @otlicense
// File: WidgetView.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTGui/WidgetViewBase.h"
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
	class WidgetBase;
	class WidgetViewManager;

	//! @class WidgetView
	//! @brief The WidgetView class is used to integrate the Qt-ADS functionallity into open twin.
	class OT_WIDGETS_API_EXPORT WidgetView : public QObject {
		Q_OBJECT
			OT_DECL_NODEFAULT(WidgetView)
			OT_DECL_NOCOPY(WidgetView)
	public:
		enum InsertFlag {
			NoInsertFlags = 0 << 0,
			KeepCurrentFocus = 1 << 0
		};
		typedef ot::Flags<InsertFlag> InsertFlags;

		static std::string createStoredViewName(const WidgetViewBase& _view);

		//! @brief Creates the name that should be used in the view management logic.
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

		void addVisualizingItem(UID _itemId) { m_visualizingItems.addSelectedNavigationItem(_itemId); };
		void setVisualizingItems(const ot::UIDList& _itemIds) { m_visualizingItems.setSelectedNavigationItems(_itemIds); };
		void removeVisualizingItem(UID _itemId) { m_visualizingItems.removeSelectedNavigationItem(_itemId); };
		void clearVisualizingItems() { m_visualizingItems.setSelectedNavigationItems(UIDList()); };
		const SelectionInformation& getVisualizingItems() const { return m_visualizingItems; };

		virtual void getDebugInformation(JsonObject& _object, JsonAllocator& _allocator) const;

	Q_SIGNALS:
		void closeRequested();
		void viewDataModifiedChanged();
		void pinnedChanged(bool _isPinned);

	protected:

		//! @brief Call this function as soon as the widget for this view is created to add it to the dock
		void addWidgetInterfaceToDock(WidgetBase* _interface);
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
		
		SelectionInformation m_visualizingItems;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::WidgetView::InsertFlag, ot::WidgetView::InsertFlags)