//! @file TreeWidgetFilter.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/TreeWidgetItem.h"
#include "OTWidgets/TreeWidgetFilter.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>

ot::TreeWidgetFilter::TreeWidgetFilter(ot::TreeWidget* _tree, int _filterColumn) 
	: m_tree(_tree), m_filterColumn(_filterColumn) {
	m_layoutW = new QWidget;
	m_layoutW->setObjectName("OT_W_TreeWidgetFilterTemplateLayoutW");
	//m_layoutW->setContentsMargins(0, 0, 0, 0);

	m_layout = new QVBoxLayout(m_layoutW);
	m_layout->setObjectName("OT_W_TreeWidgetFilterTemplateLayout");
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_lineEdit = new ot::LineEdit;
	m_lineEdit->setObjectName("OT_W_TreeWidgetFilterTemplateLineEdit");
	m_lineEdit->setPlaceholderText("Search");

	if (m_tree == nullptr) {
		m_tree = new TreeWidget;
		m_tree->setObjectName("OT_W_TreeWidgetFilterTemplateTree");
	}

	m_layout->addWidget(m_lineEdit->getQWidget());
	m_layout->addWidget(m_tree->getQWidget(), 1);

	connect(m_lineEdit, &ot::LineEdit::returnPressed, this, &ot::TreeWidgetFilter::slotReturnPressed);
	connect(m_lineEdit, &ot::LineEdit::textChanged, this, &ot::TreeWidgetFilter::slotTextChanged);
}

ot::TreeWidgetFilter::~TreeWidgetFilter() {
	if (m_tree) delete m_tree;
	if (m_lineEdit) delete m_lineEdit;
	if (m_layout) delete m_layout;
	if (m_layoutW) delete m_layoutW;
}

void ot::TreeWidgetFilter::applyFilter(const QString& _filterText) {
	for (int i = 0; i < m_tree->topLevelItemCount(); i++) {
		filterChilds(m_tree->topLevelItem(i), _filterText.toLower());
	}
}

void ot::TreeWidgetFilter::clearFilter(void) {
	for (int i = 0; i < m_tree->topLevelItemCount(); i++) {
		setAllVisible(m_tree->topLevelItem(i));
	}
}

void ot::TreeWidgetFilter::slotReturnPressed(void) {
	if (this->otWidgetFlags().flagIsSet(ot::ApplyFilterOnReturn)) {
		dispatchFilter(m_lineEdit->text());
	}
}

void ot::TreeWidgetFilter::slotTextChanged(void) {
	if (this->otWidgetFlags().flagIsSet(ot::ApplyFilterOnTextChange)) {
		dispatchFilter(m_lineEdit->text());
	}
}

void ot::TreeWidgetFilter::dispatchFilter(const QString& _text) { 
	if (_text.isEmpty()) {
		if (m_lastFilter == _text) return;
		m_lastFilter = _text;
		
		clearFilter();
		
		Q_EMIT filterCleared();
	}
	else {
		if (m_lastFilter == _text) return;
		m_lastFilter = _text;

		m_tree->collapseAll();
		applyFilter(_text);

		Q_EMIT filterChanged(m_lastFilter);
	}
}

void ot::TreeWidgetFilter::setAllVisible(QTreeWidgetItem* _item) {
	OTAssertNullptr(_item);
	TreeWidgetItem* treeItem = dynamic_cast<TreeWidgetItem*>(_item);
	if (treeItem) {
		if (treeItem->flags() & NavigationTreeItemFlag::ItemIsInvisible) return;
	}
	for (int i = 0; i < _item->childCount(); i++) {
		this->setAllVisible(_item->child(i));
	}
	_item->setHidden(false);
}

bool ot::TreeWidgetFilter::filterChilds(QTreeWidgetItem* _item, const QString& _filter) {
	OTAssertNullptr(_item);
	TreeWidgetItem* treeItem = dynamic_cast<TreeWidgetItem*>(_item);
	if (treeItem) {
		if (treeItem->flags() & NavigationTreeItemFlag::ItemIsInvisible) return false;
	}

	bool vis = false;
	
	for (int i = 0; i < _item->childCount(); i++) {
		if (filterChilds(_item->child(i), _filter)) vis = true;
	}
	
	if (_item->text(m_filterColumn).toLower().contains(_filter)) {
		vis = true;
		expandAllParents(_item->parent());
	}
	
	_item->setHidden(!vis);
	return vis;
}

void ot::TreeWidgetFilter::expandAllParents(QTreeWidgetItem* _item) {
	if (_item == nullptr) return;
	expandAllParents(_item->parent());
	_item->setExpanded(true);
}
