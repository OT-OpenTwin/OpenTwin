// @otlicense
// File: ProjectOverviewWidget.h
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
#include "ProjectOverviewFilterData.h"
#include "OTCore/ProjectFilterData.h"
#include "OTCore/ProjectInformation.h"
#include "OTGui/ExtendedProjectInformation.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtCore/qobject.h>
#include <QtWidgets/qwidget.h>

// std header
#include <map>
#include <mutex>

class QTreeWidgetItem;

namespace ot {

	class Label;
	class LineEdit;
	class TreeWidgetItem;
	class ProjectOverviewTree;
	class ProjectOverviewEntry;
	class ProjectOverviewHeader;
	class ProjectOverviewPreviewBox;

	class ProjectOverviewWidget : public QWidget, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(ProjectOverviewWidget)
		OT_DECL_NOMOVE(ProjectOverviewWidget)
	public:
		enum class ViewMode {
			Tree,
			List
		};

		static std::string toString(ViewMode _mode);
		static ViewMode viewModeFromString(const std::string& _modeStr);

		ProjectOverviewWidget(QWidget* _parent = (QWidget*)nullptr);
		~ProjectOverviewWidget();

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };
		ProjectOverviewTree* getTree() { return m_tree; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setViewMode(ViewMode _mode);
		ViewMode getViewMode() const { return m_viewMode; };

		void setMultiSelectionEnabled(bool _enabled);
		bool getMultiSelectionEnabled() const;

		int getProjectCount() const;
		
		std::list<ProjectInformation> getAllProjects() const;
		std::list<ProjectInformation> getSelectedProjects() const;

		void filterProjects(const ProjectOverviewFilterData& _filterData);

		QString getCurrentQuickFilter() const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Signals

	Q_SIGNALS:
		void selectionChanged();
		void filterReturnPressed(const QString& _text);
		void projectOpenRequested(const ProjectInformation& _projectInfo);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Slots

	public Q_SLOTS:
		void clear();
		void refreshProjects();
		void updateFilterOptions();
		void sort(int _logicalIndex, Qt::SortOrder _sortOrder);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots
		
	private Q_SLOTS:
		void slotSelectionChanged();
		void slotItemChanged(QTreeWidgetItem* _item, int _column);
		void slotOpenRequested(QTreeWidgetItem* _item, int _column);
		void slotBasicFilterReturnPressed();
		void slotBasicFilterTextChanged();
		void slotWorkerFinished();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		void setProjects(const std::list<ProjectInformation>& _projects);
		void startLoading(const ProjectInformation& _projectInfo);
		void stopLoading();
		void worker();
		void addEntry(ProjectOverviewEntry* _entry);
		void updateCheckStates(QTreeWidgetItem* _parent);
		int getProjectCount(const QTreeWidgetItem* _parent) const;
		void getAllProjects(const QTreeWidgetItem* _parent, std::list<ProjectInformation>& _lst) const;

		TreeWidgetItem* getOrCreateProjectGroupItem(const std::string& _groupName);

		std::atomic_bool m_isLoading;
		std::mutex m_mutex;
		std::unique_ptr<std::thread> m_loadingThread;
		ProjectInformation m_currentlyLoadingProject;
		ExtendedProjectInformation m_importedProjectData;

		ot::ProjectFilterData m_currentFilter;

		std::map<std::string, TreeWidgetItem*> m_projectGroupItems;

		ViewMode m_viewMode;

		ProjectOverviewHeader* m_header;
		ProjectOverviewPreviewBox* m_previewBox;

		Qt::SortOrder m_sortOrder;
		int m_lastSortColumn;

		bool m_resultsExceeded;

		LineEdit* m_basicFilter;
		QTimer m_basicFilterTimer;

		ProjectOverviewTree* m_tree;
		Label* m_countLabel;
	};

}