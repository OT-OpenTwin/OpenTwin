// @otlicense

#pragma once

// OpenTwin header
#include "ProjectOverviewFilterData.h"
#include "ProjectOverviewPreviewData.h"
#include "OTCore/ProjectInformation.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtWidgets/qwidget.h>

// std header
#include <mutex>

class QTreeWidgetItem;

namespace ot {

	class TreeWidget;
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
		TreeWidget* getTree() { return m_tree; };

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
		int getProjectCount(const QTreeWidgetItem* _parent) const;
		void getAllProjects(const QTreeWidgetItem* _parent, std::list<ProjectInformation>& _lst) const;
		void basicFilterProjects(QTreeWidgetItem* _parent);
		void filterProjects(const QTreeWidgetItem* _parent, const ProjectOverviewFilterData& _filterData);

		std::atomic_bool m_isLoading;
		std::mutex m_mutex;
		std::unique_ptr<std::thread> m_loadingThread;
		ProjectInformation m_currentlyLoadingProject;
		ProjectOverviewPreviewData m_importedProjectData;

		ProjectOverviewHeader* m_header;
		ProjectOverviewPreviewBox* m_previewBox;

		bool m_resultsExceeded;
		QString m_generalFilter;
		TreeWidget* m_tree;
		DataMode m_mode;
	};

}