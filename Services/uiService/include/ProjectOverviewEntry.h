// @otlicense
// File: ProjectOverviewEntry.h
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