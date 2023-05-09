//! @file TreeWidgetFilter.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/LineEdit.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>

ot::TreeWidgetFilter::TreeWidgetFilter(ot::TreeWidget* _tree, int _filterColumn) : m_tree(_tree), m_filterColumn(_filterColumn) {
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

	m_layout->addWidget(m_lineEdit->getWidget());
	m_layout->addWidget(m_tree->getWidget(), 1);

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
	if (m_widgetFlags.flagIsSet(ot::ApplyFilterOnReturn)) {
		dispatchFilter(m_lineEdit->text());
	}
}

void ot::TreeWidgetFilter::slotTextChanged(void) {
	if (m_widgetFlags.flagIsSet(ot::ApplyFilterOnTextChange)) {
		dispatchFilter(m_lineEdit->text());
	}
}

void ot::TreeWidgetFilter::dispatchFilter(const QString& _text) { 
	if (_text.isEmpty()) {
		if (m_lastFilter == _text) return;
		m_lastFilter = _text;
		
		clearFilter();
		
		emit filterCleared();
	}
	else {
		if (m_lastFilter == _text) return;
		m_lastFilter = _text;

		applyFilter(_text);

		emit filterChanged(m_lastFilter);
	}
}

void ot::TreeWidgetFilter::setAllVisible(QTreeWidgetItem* _item) {
	for (int i = 0; i < _item->childCount(); i++) {
		setAllVisible(_item->child(i));
	}
	_item->setHidden(false);
}

bool ot::TreeWidgetFilter::filterChilds(QTreeWidgetItem* _item, const QString& _filter) {
	bool vis = false;
	
	for (int i = 0; i < _item->childCount(); i++) {
		if (filterChilds(_item->child(i), _filter)) vis = true;
	}
	
	if (_item->text(m_filterColumn).toLower().contains(_filter)) vis = true;
	
	_item->setHidden(!vis);
	return vis;
}