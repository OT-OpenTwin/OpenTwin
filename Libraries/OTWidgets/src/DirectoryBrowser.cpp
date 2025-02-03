//! @file DirectoryBrowser.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/DirectoryBrowser.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtWidgets/qlayout.h>

ot::DirectoryBrowser::DirectoryBrowser() {
	m_rootWidget = new QWidget;
	QVBoxLayout* rootLayout = new QVBoxLayout(m_rootWidget);
	QHBoxLayout* searchLayout = new QHBoxLayout;
	searchLayout->setContentsMargins(QMargins(0, 2, 0, 2));

	searchLayout->addWidget(new Label("Root:"));
	searchLayout->addWidget(m_search = new LineEdit);
	rootLayout->addLayout(searchLayout);

	m_tree = new TreeWidgetFilter;
	rootLayout->addWidget(m_tree->getQWidget());
	TreeWidget* tree = m_tree->getTreeWidget();
	OTAssertNullptr(tree);
	tree->setHeaderHidden(true);
	tree->setSelectionMode(QAbstractItemView::SingleSelection);

	m_search->setText(QDir::rootPath());

	QTreeWidgetItem* m_rootIItem = new QTreeWidgetItem;
	m_rootIItem->setText(0, m_search->text());

	this->connect(m_search, &LineEdit::editingFinished, this, &DirectoryBrowser::slotSearchChanged);
	this->connect(tree, &TreeWidget::itemSelectionChanged, this, &DirectoryBrowser::slotSelectionChanged);
}

ot::DirectoryBrowser::~DirectoryBrowser() {

}

void ot::DirectoryBrowser::goTo(const QString& _path) {
	QString path = _path;
	path.replace('\\', '/');
	QStringList lst = path.split('/');
	this->goTo(lst);
}

void ot::DirectoryBrowser::goTo(const QStringList& _path) {
	QStringList tmp = _path;
	QTreeWidgetItem* itm = this->buildPath(m_tree->getTreeWidget()->invisibleRootItem(), tmp);
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

	return this->findItemFromPath(m_tree->getTreeWidget()->invisibleRootItem(), lst, _returnSubset);
}

QTreeWidgetItem* ot::DirectoryBrowser::findItemFromPath(const QStringList& _tidyPath, bool _returnSubset) {
	QStringList lst = _tidyPath;
	if (lst.isEmpty()) {
		return nullptr;
	}

	return this->findItemFromPath(m_tree->getTreeWidget()->invisibleRootItem(), lst, _returnSubset);
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

QTreeWidgetItem* ot::DirectoryBrowser::buildPath(QTreeWidgetItem* _parentItem, QStringList& _tidyPath) {
	if (_tidyPath.isEmpty()) {
		return nullptr;
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
	if (!_childItem || _childItem == m_tree->getTreeWidget()->invisibleRootItem()) {
		return;
	}
	_resultPath.push_front(_childItem->text(0));
	this->getItemPath(_childItem->parent(), _resultPath);
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
