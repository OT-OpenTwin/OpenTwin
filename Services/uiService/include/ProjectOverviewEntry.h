// @otlicense

#pragma once

// OpenTwin header
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
		enum ColumnIndex {
			Checked,
			Type,
			Name,
			Owner,
			Groups,
			Modified,
			Count
		};

		ProjectOverviewEntry(const ot::ProjectInformation& _projectInfo);

		const ProjectInformation& getProjectInformation() const { return m_projectInfo; };

		void applyFilter(const QString& _generalFilter);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Slots

	public Q_SLOTS:
		void updateSelectionState();
		void updateCheckState();

	private:
		ProjectInformation m_projectInfo;
	};

}