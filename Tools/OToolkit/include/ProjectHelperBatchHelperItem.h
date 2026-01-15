// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/TreeWidgetItem.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot {
	class TreeWidget;
}

class ProjectHelperBatchHelperItem : public QObject, public ot::TreeWidgetItem {
	Q_OBJECT
	OT_DECL_NOCOPY(ProjectHelperBatchHelperItem)
	OT_DECL_NOMOVE(ProjectHelperBatchHelperItem)
public:
	enum class Columns : int {
		Name,
		Edit,
		Build,
		Rebuild,
		Clear,

		ColumnCount
	};

	static void createFromPath(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _path);

	ProjectHelperBatchHelperItem();
	virtual ~ProjectHelperBatchHelperItem();

private Q_SLOTS:
	void slotBuild();
	void slotRebuild();
	void slotClear();
	void slotEdit();

private:
	QString m_rootPath;
	QString m_editPath;
	QString m_clearPath;
	QString m_buildPath;

};