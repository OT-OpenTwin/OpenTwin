// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtCore/qobject.h>

class QTreeWidgetItem;

namespace ot {
	class LineEdit;
	class TreeWidgetItem;
	class TreeWidgetFilter;
}

class ProjectHelperBatchHelper : public QObject, public ot::WidgetBase {
	Q_OBJECT
	OT_DECL_NOCOPY(ProjectHelperBatchHelper)
	OT_DECL_NOMOVE(ProjectHelperBatchHelper)
public:
	ProjectHelperBatchHelper();
	virtual ~ProjectHelperBatchHelper();

	virtual QWidget* getQWidget() override { return m_rootWidget; };
	virtual const QWidget* getQWidget() const override { return m_rootWidget; };

	void setRootPath(const QString& _path);
	QString getRootPath() const;

public Q_SLOTS:
	void slotRefreshData();

private:
	void refreshDir(const QString& _rootPath, const QString& _childName);

	QWidget* m_rootWidget;
	ot::TreeWidgetFilter* m_tree;
	ot::LineEdit* m_rootPathEdit;

};