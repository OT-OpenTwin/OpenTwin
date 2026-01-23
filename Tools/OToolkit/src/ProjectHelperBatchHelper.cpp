// @otlicense

// OpenTwin header
#include "AppBase.h"
#include "ProjectHelperBatchHelper.h"
#include "ProjectHelperBatchHelperCustomItem.h"
#include "ProjectHelperBatchHelperProjectItem.h"
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

ProjectHelperBatchHelper::ProjectHelperBatchHelper() : m_rootWidget(nullptr), m_favouritesItem(nullptr) {
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

	tree->setColumnCount(static_cast<int>(ProjectHelperBatchHelperProjectItem::Columns::ColumnCount) + 1);

	QStringList headerLabels;
	headerLabels.push_back("Name");
	for (int i = 1; i <= static_cast<int>(ProjectHelperBatchHelperProjectItem::Columns::ColumnCount); i++) {
		headerLabels.push_back("");
	}
	tree->setHeaderLabels(headerLabels);

	tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	tree->header()->setSectionResizeMode(static_cast<int>(ProjectHelperBatchHelperProjectItem::Columns::ColumnCount), QHeaderView::Stretch);

	rootLayout->addWidget(m_tree->getQWidget(), 1);
}

ProjectHelperBatchHelper::~ProjectHelperBatchHelper() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

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
	refreshData();
}

bool ProjectHelperBatchHelper::saveState(QSettings& _settings) {
	_settings.setValue("ProjectHelper.BatchHelper.RootPath", getRootPath());
	saveTreeData(_settings);

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Slots

void ProjectHelperBatchHelper::clear() {
	OTAssertNullptr(m_tree);
	m_tree->getTreeWidget()->clear();
	m_favouritesItem = nullptr;
}

void ProjectHelperBatchHelper::slotRefreshData() {
	saveTreeData(*AppBase::instance()->createSettingsInstance());
	refreshData();
}

void ProjectHelperBatchHelper::slotSelectionChanged() {
	TreeWidget* tree = m_tree->getTreeWidget();
	QSignalBlocker blocker(tree);
	refreshSelectionFromCheckState(tree->invisibleRootItem());
	refreshFavourites();
}

void ProjectHelperBatchHelper::slotItemChanged(QTreeWidgetItem* _item, int _column) {
	QSignalBlocker blocker(m_tree->getTreeWidget());
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

	slotSelectionChanged();
}

void ProjectHelperBatchHelper::slotItemDoubleClicked(QTreeWidgetItem* _item, int _column) {
	if (_item->parent() == nullptr) {
		return;
	}
	if (m_favouritesItem && m_favouritesItem->hasChild(_item)) {
		ProjectHelperBatchHelperItem* itm = dynamic_cast<ProjectHelperBatchHelperItem*>(_item);
		if (!itm) {
			return;
		}

		auto refItm = m_tree->getTreeWidget()->findItem(itm->getOriginalItemPath());
		if (refItm) {
			_item = refItm;
		}
		else {
			PHBH_LOGE("Original item for favourite not found: " + itm->getOriginalItemPath());
			return;
		}
	}

	_item->setCheckState(0, (_item->checkState(0) == Qt::Checked) ? Qt::Unchecked : Qt::Checked);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Data helper

void ProjectHelperBatchHelper::refreshData() {
	clear();

	QString rootPath = m_rootPathEdit->text();
	if (rootPath.isEmpty()) {
		return;
	}
	rootPath.replace('\\', '/');
	while (rootPath.endsWith('/')) {
		rootPath.chop(1);
	}

	TreeWidget* tree = m_tree->getTreeWidget();

	m_favouritesItem = new TreeWidgetItem;
	m_favouritesItem->setText(0, "Favourites");
	m_favouritesItem->setIcon(0, QIcon(":/images/Tree/Favorite.png"));
	tree->addTopLevelItem(m_favouritesItem);

	refreshProjectsDir(rootPath, QIcon(":/images/Tree/Library.png"), "Libraries", ProjectHelperBatchHelperProjectItem::RemoveOTPrefix);
	refreshProjectsDir(rootPath, QIcon(":/images/Tree/Service.png"), "Services");
	refreshProjectsDir(rootPath, QIcon(":/images/Tree/Tool.png"), "Tools");

	TreeWidgetItem* generalItm = new TreeWidgetItem;
	generalItm->setText(0, "General");
	generalItm->setFlags(Qt::ItemIsEnabled);
	generalItm->setIcon(0, QIcon(":/images/Tree/Text.png"));
	tree->addTopLevelItem(generalItm);
	generalItm->setExpanded(true);

	ProjectHelperBatchHelperCustomItem::createFromPath(tree, generalItm, rootPath);

	refreshFavourites();

	restoreTreeData(*AppBase::instance()->createSettingsInstance());
}

void ProjectHelperBatchHelper::restoreTreeData(const QSettings& _settings) {
	QByteArray treeData = _settings.value("ProjectHelper.BatchHelper.TreeData", QByteArray()).toByteArray();
	if (!treeData.isEmpty()) {
		restoreTreeData(treeData);
	}
}

void ProjectHelperBatchHelper::restoreTreeData(const QByteArray& _data) {
	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(_data, &err);
	if (err.error != QJsonParseError::NoError) {
		PHBH_LOGE("Failed to parse tree data: " + err.errorString());
		return;
	}
	QJsonObject rootObj = doc.object();

	TreeWidget* tree = m_tree->getTreeWidget();
	
	QJsonArray checkedArray = rootObj["Checked"].toArray();
	for (const QJsonValue& val : checkedArray) {
		QString path = val.toString();
		QTreeWidgetItem* itm = tree->findItem(path);
		if (itm != nullptr && itm->parent()) {
			QSignalBlocker blocker(tree);
			itm->setCheckState(0, Qt::Checked);
		}
	}
	
	slotSelectionChanged();

	tree->collapseAll();

	QJsonArray expandedArray = rootObj["Expanded"].toArray();
	for (const QJsonValue& val : expandedArray) {
		QString path = val.toString();
		QTreeWidgetItem* itm = tree->findItem(path);
		if (itm != nullptr) {
			QSignalBlocker blocker(tree);
			itm->setExpanded(true);
		}
	}

	
}

void ProjectHelperBatchHelper::saveTreeData(QSettings& _settings) const {
	_settings.setValue("ProjectHelper.BatchHelper.TreeData", saveTreeData());
}

QByteArray ProjectHelperBatchHelper::saveTreeData() const {
	TreeWidget* tree = m_tree->getTreeWidget();

	QJsonObject rootObj;
	
	QJsonArray expandedArray;
	QJsonArray checkedArray;
	saveItemsState(tree->invisibleRootItem(), expandedArray, checkedArray);
	rootObj["Expanded"] = expandedArray;
	rootObj["Checked"] = checkedArray;

	QJsonDocument doc(rootObj);
	return doc.toJson(QJsonDocument::Compact);
}

void ProjectHelperBatchHelper::saveItemsState(QTreeWidgetItem* _parentItem, QJsonArray& _expandedItems, QJsonArray& _checkedItems) const {
	for (int i = 0; i < _parentItem->childCount(); i++) {
		TreeWidgetItem* itm = dynamic_cast<TreeWidgetItem*>(_parentItem->child(i));
		if (itm) {
			if (itm->isExpanded()) {
				_expandedItems.push_back(itm->getTreeWidgetItemPath());
			}
			if (itm->checkState(0) == Qt::Checked) {
				_checkedItems.push_back(itm->getTreeWidgetItemPath());
			}
			saveItemsState(itm, _expandedItems, _checkedItems);
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Refresh

void ProjectHelperBatchHelper::refreshFavourites() {
	TreeWidget* tree = m_tree->getTreeWidget();

	if (!m_favouritesItem) {
		return;
	}
	else {
		// Reset favorite selection
		QSignalBlocker blocker(tree);
		auto currentSelection = tree->selectedItems();

		for (QTreeWidgetItem* selItm : currentSelection) {
			if (m_favouritesItem->hasChild(selItm)) {
				selItm->setSelected(false);
			}
		}
	}

	// Check if favourites are still checked
	refreshFavourites(m_favouritesItem);

	// Check for new favourites
	auto selection = tree->selectedItems();
	for (QTreeWidgetItem* selItm : selection) {
		ProjectHelperBatchHelperItem* actualItem = dynamic_cast<ProjectHelperBatchHelperItem*>(selItm);
		if (!actualItem) {
			continue;
		}

		if (m_favouritesItem->hasChild(actualItem->getTreeWidgetItemPath())) {
			continue;
		}

		// Add to favourites
		
		TreeWidgetItem* parentItem = m_favouritesItem;
		if (actualItem->QTreeWidgetItem::parent()) {
			parentItem = createFavouritesContainer(actualItem->QTreeWidgetItem::parent()->text(0));
			parentItem->setIcon(0, actualItem->QTreeWidgetItem::parent()->icon(0));
		}

		actualItem->createFavItem(m_tree->getTreeWidget(), parentItem);
	}

	m_favouritesItem->setHidden(m_favouritesItem->childCount() == 0);
	if (m_favouritesItem->childCount() > 0) {
		sortAll(m_favouritesItem);
	}
}

void ProjectHelperBatchHelper::refreshFavourites(QTreeWidgetItem* _parent) {
	TreeWidget* tree = m_tree->getTreeWidget();

	for (int i = 0; i < _parent->childCount(); i++) {
		QTreeWidgetItem* child = _parent->child(i);
		if (child) {
			ProjectHelperBatchHelperItem* childItm = dynamic_cast<ProjectHelperBatchHelperItem*>(child);
			if (!childItm) {
				refreshFavourites(child);
				if (child->childCount() == 0) {
					_parent->removeChild(child);
					delete child;
					i--;
				}
			}
			else {
				QTreeWidgetItem* refItm = tree->findItem(childItm->getOriginalItemPath());
				if (refItm == nullptr || refItm->checkState(0) == Qt::Unchecked) {
					childItm->setHidden(true);

					ProjectHelperBatchHelperItem* actualRefItm = dynamic_cast<ProjectHelperBatchHelperItem*>(refItm);
					if (actualRefItm) {
						actualRefItm->referenceDestroyed(childItm);
					}

					_parent->removeChild(childItm);
					delete childItm;
					i--;
				}
			}
		}
	}
}

TreeWidgetItem* ProjectHelperBatchHelper::createFavouritesContainer(const QString& _containerName) {
	for (int i = 0; i < m_favouritesItem->childCount(); i++) {
		QTreeWidgetItem* childItm = m_favouritesItem->child(i);
		if (childItm->text(0) == _containerName) {
			TreeWidgetItem* actualChild = dynamic_cast<TreeWidgetItem*>(childItm);
			if (actualChild) {
				return actualChild;
			}
			else {
				PHBH_LOGE("Invalid favourites container item found \"" + _containerName + "\"");
			}
		}
	}

	TreeWidgetItem* containerItm = new TreeWidgetItem;
	containerItm->setText(0, _containerName);
	containerItm->setFlags(Qt::ItemIsEnabled);
	m_favouritesItem->addChild(containerItm);
	containerItm->setExpanded(true);
	return containerItm;
}

void ProjectHelperBatchHelper::sortAll(QTreeWidgetItem* _parent) {
	for (int i = 0; i < _parent->childCount(); i++) {
		sortAll(_parent->child(i));
	}
	_parent->sortChildren(0, Qt::AscendingOrder);
}

void ProjectHelperBatchHelper::refreshProjectsDir(const QString& _rootPath, const QIcon& _icon, const QString& _childName, const ProjectHelperBatchHelperProjectItem::CreateFlags& _flags) {
	TreeWidgetItem* rootItm = new TreeWidgetItem;
	rootItm->setText(0, _childName);
	rootItm->setIcon(0, _icon);
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
		ProjectHelperBatchHelperProjectItem::createFromPath(m_tree->getTreeWidget(), rootItm, pth, _flags);
	}

	rootItm->setExpanded(true);
}

void ProjectHelperBatchHelper::refreshSelectionFromCheckState(QTreeWidgetItem* _parent) {
	for (int i = 0; i < _parent->childCount(); i++) {
		QTreeWidgetItem* childItm = _parent->child(i);
		if (childItm->checkState(0) == Qt::Checked) {
			if (!childItm->isSelected() && childItm->parent() && !m_favouritesItem->hasChild(childItm)) {
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

void ProjectHelperBatchHelper::resetSelection(QTreeWidgetItem* _parent) {
	for (int i = 0; i < _parent->childCount(); i++) {
		QTreeWidgetItem* childItm = _parent->child(i);
		if (childItm->isSelected()) {
			childItm->setSelected(false);
		}
		resetSelection(childItm);
	}
}