// @otlicense
// File: ProjectOverviewPreviewBox.h
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
#include "OTGui/ExtendedProjectInformation.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtCore/qparallelanimationgroup.h>
#include <QtWidgets/qwidget.h>

namespace ot {

	class Label;
	class TextEdit;
	class ImagePainterWidget;

	class ProjectOverviewPreviewBox : public QWidget {
		Q_OBJECT
		OT_DECL_NOCOPY(ProjectOverviewPreviewBox)
		OT_DECL_NOMOVE(ProjectOverviewPreviewBox)
	public:
		static constexpr QSize previewImageSize() { return QSize(255, 255); };

		ProjectOverviewPreviewBox(QWidget* _parent = nullptr);
		virtual ~ProjectOverviewPreviewBox();

		void unsetProject();
		void setProject(const ExtendedProjectInformation& _data);

	private Q_SLOTS:
		void slotDelayedCollapse();
		void slotAnimationFinished();

	private:
		const int c_expandedWidth = 300;

		QParallelAnimationGroup m_animation;
		QTimer m_collapseTimer;
		bool m_isExpanded;

		ImagePainterWidget* m_imageWidget;
		Label* m_name;
		Label* m_type;
		Label* m_projectGroup;
		Label* m_tags;
		Label* m_userGroups;
		TextEdit* m_description;
	};
} // namespace ot