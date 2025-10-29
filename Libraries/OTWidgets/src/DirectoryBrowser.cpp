// @otlicense

//! @file DirectoryBrowser.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/DirectoryBrowser.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qtreeview.h>
#include <QtWidgets/qfilesystemmodel.h>

ot::DirectoryBrowser::DirectoryBrowser() {
	// Create layouts
	QVBoxLayout* rootLay = new QVBoxLayout(this);

	QHBoxLayout* searchLay = new QHBoxLayout;
	searchLay->setContentsMargins(0, 0, 0, 0);
	searchLay->addWidget(new Label("Find"));
	searchLay->addWidget(m_rootEdit = new LineEdit);

	rootLay->addLayout(searchLay);

	// Create Qt file system model
	m_model = new QFileSystemModel(this);
	m_model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
	m_model->setRootPath(QDir::rootPath());
	m_model->setOption(QFileSystemModel::DontResolveSymlinks);
	m_model->setOption(QFileSystemModel::DontUseCustomDirectoryIcons);

	// Create tree view to display file system model
	m_treeView = new QTreeView(this);
	m_treeView->setModel(m_model);
	m_treeView->setAnimated(false);
	m_treeView->setIndentation(20);
	m_treeView->setSortingEnabled(true);
	
	for (int i = 1; i <= 3; i++) {
		m_treeView->setColumnHidden(i, true);
	}
	
	m_rootIx = m_treeView->rootIndex();

	rootLay->addWidget(m_treeView);

	// Connect signals
	this->connect(m_rootEdit, &LineEdit::editingFinished, this, &DirectoryBrowser::slotRootChanged);
	this->connect(m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DirectoryBrowser::slotSelectionChanged);
}

ot::DirectoryBrowser::~DirectoryBrowser() {

}

void ot::DirectoryBrowser::slotRootChanged(void) {
	QString txt = m_rootEdit->text();
	
	QModelIndex newRoot = m_rootIx;

	if (!txt.isEmpty()) {
		QStringList lst = txt.replace('\\', '/').split('/');

		QString path;
		for (int i = 0; i < lst.count(); i++) {
			if (i > 0) {
				path.append('/');
			}
			path.append(lst[i]);

			QModelIndex ix = m_model->index(path);
			if (ix.isValid()) {
				m_treeView->expand(ix);
				newRoot = ix;
			}
		}
	}

	m_treeView->setCurrentIndex(newRoot);
	this->slotSelectionChanged();
}

void ot::DirectoryBrowser::slotSelectionChanged(void) {
	QModelIndex currentIndex = m_treeView->selectionModel()->currentIndex();
	if (!currentIndex.isValid()) {
		return;
	}

	QString path = m_model->filePath(currentIndex);
	Q_EMIT currentPathChanged(path);
}

QString ot::DirectoryBrowser::getItemText(const QModelIndex& _ix) {
	OTAssert(_ix.isValid(), "Invalid index");
	QMap<int, QVariant> itemData = m_model->itemData(_ix);
	return itemData.value(0, QString()).toString();
}

/*

void ot::DirectoryBrowser::goTo(const QString& _path) {
	QString path = _path;
	path.replace('\\', '/');
	QStringList lst = path.split('/');
	this->goTo(lst);
}

void ot::DirectoryBrowser::goTo(const QStringList& _path) {
	QStringList tmp = _path;
	QTreeWidgetItem* itm = this->buildPath(m_rootItem, tmp);
	tmp.clear();
	this->getItemPath(itm, tmp);
	if (tmp != _path) {
		OT_LOG_E("Failed to create path");
	}
}

void ot::DirectoryBrowser::expandCurrent(void) {
	auto sel = m_tree->getTreeWidget()->selectedItems();
	if (sel.isEmpty()) {
		return;
	}
	QTreeWidgetItem* itm = sel.front();
	while (itm) {
		itm->setExpanded(true);
		itm = itm->parent();
	}
}

void ot::DirectoryBrowser::goToAndExpand(const QStringList& _path) {
	this->goTo(_path);
	this->expandCurrent();
}

void ot::DirectoryBrowser::slotSearchChanged(void) {

}

void ot::DirectoryBrowser::slotSelectionChanged(void) {
	auto sel = m_tree->getTreeWidget()->selectedItems();
	if (sel.isEmpty()) {
		return;
	}
	if (sel.size() > 1) {
		OT_LOG_E("Multiselection is not supported");
	}

	QStringList path;
	this->getItemPath(sel.front(), path);

	this->goToAndExpand(path);
}

QTreeWidgetItem* ot::DirectoryBrowser::findItemFromPath(const QString& _tidyPath, bool _returnSubset) {
	QStringList lst = _tidyPath.split('/', Qt::SkipEmptyParts);
	if (lst.isEmpty()) {
		return nullptr;
	}

	return this->findItemFromPath(m_rootItem, lst, _returnSubset);
}

QTreeWidgetItem* ot::DirectoryBrowser::findItemFromPath(const QStringList& _tidyPath, bool _returnSubset) {
	QStringList lst = _tidyPath;
	if (lst.isEmpty()) {
		return nullptr;
	}

	return this->findItemFromPath(m_rootItem, lst, _returnSubset);
}

QTreeWidgetItem* ot::DirectoryBrowser::findItemFromPath(QTreeWidgetItem* _parentItem, QStringList& _tidyPath, bool _returnSubset) {
	OTAssertNullptr(_parentItem);
	for (int i = 0; i < _parentItem->childCount(); i++) {
		OTAssertNullptr(_parentItem->child(i));
		if (_parentItem->child(i)->text(0) == _tidyPath.front()) {
			_tidyPath.pop_front();
			if (_tidyPath.isEmpty()) {
				return _parentItem->child(i);
			}
			else {
				QTreeWidgetItem* childResult = this->findItemFromPath(_parentItem->child(i), _tidyPath, _returnSubset);
				if (childResult) {
					return childResult;
				}
				else if (_returnSubset) {
					return _parentItem->child(i);
				}
				else {
					return nullptr;
				}
			}
		}
	}

	return nullptr;
}

void ot::DirectoryBrowser::updateRoot(const QString& _newRootPath) {
	QStringList lastPath;
	auto sel = m_tree->getTreeWidget()->selectedItems();
	if (!sel.isEmpty()) {
		this->getItemPath(sel.front(), lastPath);
	}

	QDir dir(_newRootPath);
	if (!dir.exists()) {
		return;
	}

	m_rootItem->setText(0, dir.dirName());
	QStringList newChildItems = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QString& newChild : newChildItems) {
		bool found = false;
		for (int i = 0; i < m_rootItem->childCount(); i++) {
			if (m_rootItem->child(i)->text(0) == newChild) {
				found = true;
				break;
			}
		}

		if (!found) {
			m_rootItem->addChild(new QTreeWidgetItem(QStringList({ newChild })));
		}
	}
}

QTreeWidgetItem* ot::DirectoryBrowser::buildPath(QTreeWidgetItem* _parentItem, QStringList& _tidyPath) {
	if (_tidyPath.isEmpty()) {
		return nullptr;
	}

	// Rescan directory
	QDir parentDir(this->getItemPathString(_parentItem));
	QStringList newChildItems = parentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QString& newChild : newChildItems) {
		bool found = false;
		for (int i = 0; i < _parentItem->childCount(); i++) {
			if (_parentItem->child(i)->text(0) == newChild) {
				found = true;
				break;
			}
		}

		if (!found) {
			_parentItem->addChild(new QTreeWidgetItem(QStringList({ newChild })));
		}
	}

	// Check if first item in path exists
	for (int i = 0; i < _parentItem->childCount(); i++) {
		// Find existing child
		if (_parentItem->child(i)->text(0) == _tidyPath.front()) {
			_tidyPath.pop_front();

			if (_tidyPath.isEmpty()) {
				// Item is end of list, return it (path exists)
				return _parentItem->child(i);
			}
			else {
				// Find nested child
				return this->buildPath(_parentItem->child(i), _tidyPath);
			}
		}
	}

	// Root child does not exist, create whole branch
	while (!_tidyPath.isEmpty()) {
		QTreeWidgetItem* item = new QTreeWidgetItem;
		item->setText(0, _tidyPath.front());
		_parentItem->addChild(item);

		_parentItem = item;
		_tidyPath.pop_front();
	}
	
	// The parent item can not be the root item since:
	// Path is not empty AND
	// No child of the parent item exists AND
	// Therefore at least one item has been added to the parent and the parent changed to the new item
	return _parentItem;
}

void ot::DirectoryBrowser::getItemPath(QTreeWidgetItem* _childItem, QStringList& _resultPath) {
	if (!_childItem) {
		return;
	}
	_resultPath.push_front(_childItem->text(0));

	if (_childItem != m_rootItem) {
		this->getItemPath(_childItem->parent(), _resultPath);
	}
}

QString ot::DirectoryBrowser::getItemPathString(QTreeWidgetItem* _itm) {
	if (!_itm) {
		return QString();
	}

	QString result = this->getItemPathString(_itm->parent());
	if (result.isEmpty()) {
		return _itm->text(0);
	}
	else {
		return result + "/" + _itm->text(0);
	}
}
*/