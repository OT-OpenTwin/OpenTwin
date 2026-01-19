// @otlicense

// OpenTwin header
#include "ProjectHelperBatchHelper.h"
#include "ProjectHelperBatchHelperItem.h"
#include "ProjectHelperBatchHelperCustomItem.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/TreeWidgetItem.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OToolkitAPI/OToolkitAPI.h"

// Qt header
#include <QtCore/qdiriterator.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qheaderview.h>

#define PHBH_LOG(___message) OTOOLKIT_LOG("BatchHelper", ___message)
#define PHBH_LOGW(___message) OTOOLKIT_LOGW("BatchHelper", ___message)
#define PHBH_LOGE(___message) OTOOLKIT_LOGE("BatchHelper", ___message)

using namespace ot;

ProjectHelperBatchHelper::ProjectHelperBatchHelper() : m_rootWidget(nullptr) {
	m_rootWidget = new QWidget;
	QVBoxLayout* rootLayout = new QVBoxLayout(m_rootWidget);

	QHBoxLayout* topLayout = new QHBoxLayout;
	rootLayout->addLayout(topLayout);

	topLayout->addWidget(new Label("Root path:", m_rootWidget));
	m_rootPathEdit = new LineEdit(m_rootWidget);
	topLayout->addWidget(m_rootPathEdit, 1);

	PushButton* refreshButton = new PushButton("Refresh", m_rootWidget);
	connect(refreshButton, &PushButton::clicked, this, &ProjectHelperBatchHelper::slotRefreshData);
	topLayout->addWidget(refreshButton);

	m_tree = new TreeWidgetFilter(m_rootWidget);
	TreeWidget* tree = m_tree->getTreeWidget();
	connect(tree, &TreeWidget::itemSelectionChanged, this, &ProjectHelperBatchHelper::slotSelectionChanged);
	connect(tree, &TreeWidget::itemChanged, this, &ProjectHelperBatchHelper::slotItemChanged);
	connect(tree, &TreeWidget::itemDoubleClicked, this, &ProjectHelperBatchHelper::slotItemDoubleClicked);

	tree->setColumnCount(static_cast<int>(ProjectHelperBatchHelperItem::Columns::ColumnCount) + 1);

	QStringList headerLabels;
	headerLabels.push_back("Name");
	for (int i = 1; i <= static_cast<int>(ProjectHelperBatchHelperItem::Columns::ColumnCount); i++) {
		headerLabels.push_back("");
	}
	tree->setHeaderLabels(headerLabels);

	tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	tree->header()->setSectionResizeMode(static_cast<int>(ProjectHelperBatchHelperItem::Columns::ColumnCount), QHeaderView::Stretch);

	rootLayout->addWidget(m_tree->getQWidget(), 1);
}

ProjectHelperBatchHelper::~ProjectHelperBatchHelper() {

}

void ProjectHelperBatchHelper::setRootPath(const QString& _path) {
	m_rootPathEdit->setText(_path);
}

QString ProjectHelperBatchHelper::getRootPath() const {
	return m_rootPathEdit->text();
}

void ProjectHelperBatchHelper::restoreState(QSettings& _settings) {
	QString batchPth = _settings.value("ProjectHelper.BatchHelper.RootPath", "").toString();
	if (batchPth.isEmpty()) {
		batchPth = qgetenv("OPENTWIN_DEV_ROOT");
	}
	setRootPath(batchPth);
	slotRefreshData();

	QByteArray treeData = _settings.value("ProjectHelper.BatchHelper.TreeData", QByteArray()).toByteArray();
	if (!treeData.isEmpty()) {
		restoreTreeData(treeData);
	}
}

bool ProjectHelperBatchHelper::saveState(QSettings& _settings) {
	_settings.setValue("ProjectHelper.BatchHelper.RootPath", getRootPath());
	_settings.setValue("ProjectHelper.BatchHelper.TreeData", saveTreeData());

	return true;
}

void ProjectHelperBatchHelper::restoreTreeData(const QByteArray& _data) {
	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(_data, &err);
	if (err.error != QJsonParseError::NoError) {
		PHBH_LOGE("Failed to parse tree data: " + err.errorString());
		return;
	}
	TreeWidget* tree = m_tree->getTreeWidget();
	tree->collapseAll();

	QJsonObject rootObj = doc.object();
	QJsonArray expandedArray = rootObj["Expanded"].toArray();
	for (const QJsonValue& val : expandedArray) {
		QString path = val.toString();
		QTreeWidgetItem* itm = findItemByPath(path);
		if (itm != nullptr) {
			itm->setExpanded(true);
		}
	}

	QJsonArray checkedArray = rootObj["Checked"].toArray();
	for (const QJsonValue& val : checkedArray) {
		QString path = val.toString();
		QTreeWidgetItem* itm = findItemByPath(path);
		if (itm != nullptr && itm->parent()) {
			itm->setCheckState(0, Qt::Checked);
		}
	}

}

QByteArray ProjectHelperBatchHelper::saveTreeData() const {
	TreeWidget* tree = m_tree->getTreeWidget();

	QJsonObject rootObj;
	QJsonArray expandedArray;
	for (int i = 0; i < tree->topLevelItemCount(); i++) {
		QTreeWidgetItem* topItm = tree->topLevelItem(i);
		if (topItm->isExpanded()) {
			expandedArray.push_back(topItm->text(0));
		}
	}
	rootObj["Expanded"] = expandedArray;

	QJsonArray checkedArray;
	saveCheckedItems(checkedArray, tree->invisibleRootItem());
	rootObj["Checked"] = checkedArray;
	QJsonDocument doc(rootObj);
	return doc.toJson(QJsonDocument::Compact);
}

QTreeWidgetItem* ProjectHelperBatchHelper::findItemByPath(const QString& _path) const {
	QStringList pathParts = _path.split('/', Qt::SkipEmptyParts);
	if (pathParts.isEmpty()) {
		return nullptr;
	}
	else {
		return findItemByPath(m_tree->getTreeWidget()->invisibleRootItem(), pathParts);
	}
}

QTreeWidgetItem* ProjectHelperBatchHelper::findItemByPath(QTreeWidgetItem* _parentItem, QStringList _pathParts) const {
	for (int i = 0; i < _parentItem->childCount(); i++) {
		QTreeWidgetItem* childItm = _parentItem->child(i);
		if (childItm->text(0) == _pathParts.front()) {
			_pathParts.pop_front();
			if (_pathParts.isEmpty()) {
				return childItm;
			}
			else {
				return findItemByPath(childItm, _pathParts);
			}
		}
	}
	return nullptr;
}

QString ProjectHelperBatchHelper::itemPath(QTreeWidgetItem* _item) const {
	QString path = _item->text(0);
	if (_item->parent() != nullptr) {
		path.prepend(itemPath(_item->parent()) + '/');
	}
	return path;
}

void ProjectHelperBatchHelper::saveCheckedItems(QJsonArray& _checkedItems, QTreeWidgetItem* _parentItem) const {
	for (int i = 0; i < _parentItem->childCount(); i++) {
		QTreeWidgetItem* childItm = _parentItem->child(i);
		if (childItm->checkState(0) == Qt::Checked) {
			_checkedItems.push_back(itemPath(childItm));
		}
		saveCheckedItems(_checkedItems, childItm);
	}
}

void ProjectHelperBatchHelper::refreshDir(const QString& _rootPath, const QString& _childName, const ProjectHelperBatchHelperItem::CreateFlags& _flags) {
	TreeWidgetItem* rootItm = new TreeWidgetItem;
	rootItm->setText(0, _childName);
	rootItm->setCheckState(0, Qt::Unchecked);
	rootItm->setFlags(Qt::ItemIsEnabled);

	m_tree->getTreeWidget()->addTopLevelItem(rootItm);

	QDirIterator dirIt(_rootPath + "/" + _childName, QDir::Dirs | QDir::NoDotAndDotDot);
	while (dirIt.hasNext()) {
		dirIt.next();
		QString pth = dirIt.filePath();
		pth.replace('\\', '/');
		if (!pth.endsWith('/')) {
			pth.append('/');
		}
		ProjectHelperBatchHelperItem::createFromPath(m_tree->getTreeWidget(), rootItm, pth, _flags);
	}

	rootItm->setExpanded(true);
}

void ProjectHelperBatchHelper::refreshSelectionFromCheckState(QTreeWidgetItem* _parent) {
	for (int i = 0; i < _parent->childCount(); i++) {
		QTreeWidgetItem* childItm = _parent->child(i);
		if (childItm->checkState(0) == Qt::Checked) {
			if (!childItm->isSelected() && childItm->parent()) {
				childItm->setSelected(true);
			}
		}
		else {
			if (childItm->isSelected()) {
				childItm->setSelected(false);
			}
		}
		refreshSelectionFromCheckState(childItm);
	}
}

void ProjectHelperBatchHelper::slotRefreshData() {
	QString rootPath = m_rootPathEdit->text();
	if (rootPath.isEmpty()) {
		return;
	}
	rootPath.replace('\\', '/');
	while (rootPath.endsWith('/')) {
		rootPath.chop(1);
	}

	TreeWidget* tree = m_tree->getTreeWidget();

	tree->clear();
	refreshDir(rootPath, "Libraries", ProjectHelperBatchHelperItem::RemoveOTPrefix);
	refreshDir(rootPath, "Services");
	refreshDir(rootPath, "Tools");

	TreeWidgetItem* generalItm = new TreeWidgetItem;
	generalItm->setText(0, "General");
	generalItm->setCheckState(0, Qt::Unchecked);
	generalItm->setFlags(Qt::ItemIsEnabled);
	tree->addTopLevelItem(generalItm);
	generalItm->setExpanded(true);

	ProjectHelperBatchHelperCustomItem::createFromPath(tree, generalItm, rootPath);
}

void ProjectHelperBatchHelper::slotSelectionChanged() {
	TreeWidget* tree = m_tree->getTreeWidget();
	QSignalBlocker blocker(tree);
	refreshSelectionFromCheckState(tree->invisibleRootItem());
}

void ProjectHelperBatchHelper::slotItemChanged(QTreeWidgetItem* _item, int _column) {
	if (_item->parent() == nullptr) {
		return;
	}
	if (_item->checkState(0) == Qt::Checked) {
		if (!_item->isSelected()) {
			_item->setSelected(true);
		}
	}
	else {
		if (_item->isSelected()) {
			_item->setSelected(false);
		}
	}
}

void ProjectHelperBatchHelper::slotItemDoubleClicked(QTreeWidgetItem* _item, int _column) {
	if (_item->parent() == nullptr) {
		return;
	}

	_item->setCheckState(0, (_item->checkState(0) == Qt::Checked) ? Qt::Unchecked : Qt::Checked);
}

