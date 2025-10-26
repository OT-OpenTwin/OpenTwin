// @otlicense

#pragma once

// OpenTwin header
#include "ProjectOverviewFilterData.h"
#include "OTCore/ProjectInformation.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtCore/qobject.h>

class QTreeWidgetItem;

namespace ot {

	class TreeWidget;
	class ProjectOverviewHeader;

	class ProjectOverviewWidget : public QObject, public WidgetBase {
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

		virtual QWidget* getQWidget() override;
		virtual const QWidget* getQWidget() const override;
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
		void slotRefreshAllSelection();
		void slotRefreshSelection(QTreeWidgetItem* _item, int _column);
		void slotOpenRequested(QTreeWidgetItem* _item, int _column);
		void slotBasicFilterProjects();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		int getProjectCount(const QTreeWidgetItem* _parent) const;
		void getAllProjects(const QTreeWidgetItem* _parent, std::list<ProjectInformation>& _lst) const;
		void basicFilterProjects(QTreeWidgetItem* _parent);
		void filterProjects(const QTreeWidgetItem* _parent, const ProjectOverviewFilterData& _filterData);

		ProjectOverviewHeader* m_header;

		bool m_resultsExceeded;
		QString m_generalFilter;
		TreeWidget* m_tree;
		DataMode m_mode;
	};

}