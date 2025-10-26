// @otlicense

#pragma once

// OpenTwin header
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
		bool hasOtherUser(const std::string& _currentUser) const;
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

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots

	private Q_SLOTS:
		void slotRefreshAllSelection();
		void slotRefreshSelection(QTreeWidgetItem* _item, int _column);
		void slotOpenRequested(QTreeWidgetItem* _item, int _column);
		void slotFilterProjects();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		int getProjectCount(const QTreeWidgetItem* _parent) const;
		bool hasOtherUser(const QTreeWidgetItem* _parent, const std::string& _currentUser) const;
		void filterProjects(QTreeWidgetItem* _parent);

		ProjectOverviewHeader* m_header;

		bool m_resultsExceeded;
		QString m_generalFilter;
		TreeWidget* m_tree;
		DataMode m_mode;
	};

}