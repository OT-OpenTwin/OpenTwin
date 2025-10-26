// @otlicense

#pragma once

// OpenTwin header
#include "ProjectOverviewFilterData.h"
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtWidgets/qmenu.h>

class QListWidget;
class QListWidgetItem;

namespace ot {

	class Label;
	class LineEdit;
	class ProjectOverviewWidget;

	class ProjectOverviewFilter : public QMenu {
		Q_OBJECT
		OT_DECL_NOCOPY(ProjectOverviewFilter)
		OT_DECL_NOMOVE(ProjectOverviewFilter)
		OT_DECL_NODEFAULT(ProjectOverviewFilter)
	public:
		ProjectOverviewFilter(ProjectOverviewWidget* _overview, int _logicalIndex);
		virtual ~ProjectOverviewFilter() = default;

		void setTitle(const QString& _title);

		void setOptions(const QStringList& _options);
		ProjectOverviewFilterData getFilterData() const;

		void updateCheckedStatesFromData(const ProjectOverviewFilterData& _data);

		bool isConfirmed() const { return m_isConfirmed; };

	Q_SIGNALS:
		void filterChanged(const ProjectOverviewFilterData& _filterData);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots

	private Q_SLOTS:
		void slotConfirm();
		void slotCancel();
		void slotTextChanged();
		void slotCheckedChanged(QListWidgetItem* _item);

	private:
		int m_logicalIndex;
		bool m_isConfirmed;

		Label* m_title;
		LineEdit* m_filterEdit;
		QListWidget* m_optionsList;
	};

}