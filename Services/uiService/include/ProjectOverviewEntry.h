// @otlicense

#pragma once

// OpenTwin header
#include "ProjectOverviewFilterData.h"
#include "OTCore/ProjectInformation.h"
#include "OTWidgets/TreeWidgetItem.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot {

	class ProjectOverviewEntry : public QObject, public ot::TreeWidgetItem {
		Q_OBJECT
		OT_DECL_NOCOPY(ProjectOverviewEntry)
		OT_DECL_NOMOVE(ProjectOverviewEntry)
		OT_DECL_NODEFAULT(ProjectOverviewEntry)
	public:
		ProjectOverviewEntry(const ot::ProjectInformation& _projectInfo);

		const ProjectInformation& getProjectInformation() const { return m_projectInfo; };

		void applyFilter(const QString& _generalFilter);
		void applyFilter(const ProjectOverviewFilterData& _filter);

		virtual bool operator<(const QTreeWidgetItem& _other) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Slots

	public Q_SLOTS:
		void updateSelectionState();
		void updateCheckState();

	private:
		ProjectInformation m_projectInfo;
	};

}