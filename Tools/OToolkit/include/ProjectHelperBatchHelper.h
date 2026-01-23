// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/WidgetBase.h"
#include "ProjectHelperBatchHelperProjectItem.h"

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

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setRootPath(const QString& _path);
	QString getRootPath() const;

	void restoreState(QSettings& _settings);
	bool saveState(QSettings& _settings);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public: Slots

public Q_SLOTS:
	void clear();
	void slotRefreshData();
	void slotSelectionChanged();
	void slotItemChanged(QTreeWidgetItem* _item, int _column);
	void slotItemDoubleClicked(QTreeWidgetItem* _item, int _column);

private:

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Data helper

	void refreshData();
	void restoreTreeData(const QSettings& _settings);
	void restoreTreeData(const QByteArray& _data);
	void saveTreeData(QSettings& _settings) const;
	QByteArray saveTreeData() const;
	void saveItemsState(QTreeWidgetItem* _parentItem, QJsonArray& _expandedItems, QJsonArray& _checkedItems) const;
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Refresh

	void refreshFavourites();
	void refreshFavourites(QTreeWidgetItem* _parent);
	ot::TreeWidgetItem* createFavouritesContainer(const QString& _containerName);

	void sortAll(QTreeWidgetItem* _parent);
	void refreshProjectsDir(const QString& _rootPath, const QIcon& _icon, const QString& _childName, const ProjectHelperBatchHelperProjectItem::CreateFlags& _flags = ProjectHelperBatchHelperProjectItem::NoCreateFlags);

	void refreshSelectionFromCheckState(QTreeWidgetItem* _parent);
	void resetSelection(QTreeWidgetItem* _parent);

	QWidget* m_rootWidget;
	ot::TreeWidgetFilter* m_tree;
	ot::TreeWidgetItem* m_favouritesItem;
	ot::LineEdit* m_rootPathEdit;

};