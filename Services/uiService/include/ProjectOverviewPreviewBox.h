// @otlicense

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