// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/WidgetBase.h"
#include "ProjectHelperBatchHelperItem.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qsettings.h>
#include <QtCore/qjsonarray.h>

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

	void restoreState(QSettings& _settings);
	bool saveState(QSettings& _settings);

public Q_SLOTS:
	void slotRefreshData();
	void slotSelectionChanged();
	void slotItemChanged(QTreeWidgetItem* _item, int _column);
	void slotItemDoubleClicked(QTreeWidgetItem* _item, int _column);

private:
	void restoreTreeData(const QByteArray& _data);
	QByteArray saveTreeData() const;
	void saveCheckedItems(QJsonArray& _checkedItems, QTreeWidgetItem* _parentItem) const;
	
	QTreeWidgetItem* findItemByPath(const QString& _path) const;
	QTreeWidgetItem* findItemByPath(QTreeWidgetItem* _parentItem, QStringList _pathParts) const;
	QString itemPath(QTreeWidgetItem* _item) const;

	void refreshDir(const QString& _rootPath, const QString& _childName, const ProjectHelperBatchHelperItem::CreateFlags& _flags = ProjectHelperBatchHelperItem::NoCreateFlags);

	void refreshSelectionFromCheckState(QTreeWidgetItem* _parent);

	QWidget* m_rootWidget;
	ot::TreeWidgetFilter* m_tree;
	ot::LineEdit* m_rootPathEdit;

};