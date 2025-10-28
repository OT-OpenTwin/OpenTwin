// @otlicense

#pragma once

// OpenTwin header
#include "ProjectOverviewFilterData.h"
#include "OTCore/ProjectInformation.h"
#include "OTGui/ExtendedProjectInformation.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtWidgets/qwidget.h>

// std header
#include <map>
#include <mutex>

class QTreeWidgetItem;

namespace ot {

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
		enum DataMode {
			RecentMode,
			AllMode
		};

		ProjectOverviewWidget(QWidget* _parent = (QWidget*)nullptr);
		~ProjectOverviewWidget();

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };
		ProjectOverviewTree* getTree() { return m_tree; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setMultiSelectionEnabled(bool _enabled);
		bool getMultiSelectionEnabled() const;

		void setGeneralFilter(const QString& _filter);

		DataMode getDataMode() const { return m_mode; };
		int getProjectCount() const;
		bool getProjectsReultsExceeded() const { return m_resultsExceeded; };
		
		std::list<ProjectInformation> getAllProjects() const;
		std::list<ProjectInformation> getSelectedProjects() const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Signals

	Q_SIGNALS:
		void selectionChanged();
		void projectOpenRequested(const ProjectInformation& _projectInfo);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Slots

	public Q_SLOTS:
		void clear();
		void refreshProjectList();
		void refreshRecentProjects();
		void refreshAllProjects();
		void filterProjects(const ProjectOverviewFilterData& _filterData);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots

	private Q_SLOTS:
		void slotSelectionChanged();
		void slotItemChanged(QTreeWidgetItem* _item, int _column);
		void slotOpenRequested(QTreeWidgetItem* _item, int _column);
		void slotBasicFilterProjects();
		void slotWorkerFinished();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		void startLoading(const ProjectInformation& _projectInfo);
		void stopLoading();
		void worker();
		void addEntry(ProjectOverviewEntry* _entry);
		void updateCategories();
		void updateCategoryItem(TreeWidgetItem* _categoryItem);
		void updateCheckStates(QTreeWidgetItem* _parent);
		int getProjectCount(const QTreeWidgetItem* _parent) const;
		void getAllProjects(const QTreeWidgetItem* _parent, std::list<ProjectInformation>& _lst) const;
		void basicFilterProjects(QTreeWidgetItem* _parent);
		void filterProjects(const QTreeWidgetItem* _parent, const ProjectOverviewFilterData& _filterData);

		std::atomic_bool m_isLoading;
		std::mutex m_mutex;
		std::unique_ptr<std::thread> m_loadingThread;
		ProjectInformation m_currentlyLoadingProject;
		ExtendedProjectInformation m_importedProjectData;

		std::map<std::string, TreeWidgetItem*> m_categoryItems;

		ProjectOverviewHeader* m_header;
		ProjectOverviewPreviewBox* m_previewBox;

		bool m_resultsExceeded;
		QString m_generalFilter;
		ProjectOverviewTree* m_tree;
		DataMode m_mode;
	};

}