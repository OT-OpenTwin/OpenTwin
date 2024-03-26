//! @file PropertyGrid.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/PropertyGridItemDelegate.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>

class ot::PropertyGrid::PropertyGridTree : public TreeWidget {
protected:
	virtual void mousePressEvent(QMouseEvent* _event) override {
		QModelIndex index = indexAt(_event->pos());
		bool last_state = isExpanded(index);
		
		TreeWidget::mousePressEvent(_event);
		
		if (index.isValid() && last_state == isExpanded(index))
			setExpanded(index, !last_state);
			
	}

	virtual void drawRow(QPainter* _painter, const QStyleOptionViewItem& _options, const QModelIndex& _index) const override {
		QStyleOptionViewItem opt = _options;
		if (_index.isValid()) {

			QTreeWidgetItem* item = this->topLevelItem(_index.row());

			PropertyGridGroup* g = dynamic_cast<PropertyGridGroup*>(item);
			PropertyGridItem* i = dynamic_cast<PropertyGridItem*>(item);
			if (g) {
				_painter->fillRect(_options.rect, g->groupBrush());
			}
			else if (i) {
				_painter->fillRect(_options.rect, i->itemBrush());
			}
		}
		TreeWidget::drawRow(_painter, opt, _index);
	}
};

ot::PropertyGrid::PropertyGrid(QObject* _parentObject) : QObject(_parentObject), m_defaultGroup(nullptr) {
	m_tree = new PropertyGrid::PropertyGridTree;
	m_tree->setColumnCount(2);
	m_tree->setHeaderLabels({ "Name", "Value" });
	m_tree->setItemDelegate(new PropertyGridItemDelegate(m_tree));
	m_tree->setIndentation(0);
	this->connect(m_tree, &TreeWidget::itemChanged, this, &PropertyGrid::slotItemChanged);
}

ot::PropertyGrid::~PropertyGrid() {
	delete m_tree;
}

QWidget* ot::PropertyGrid::getQWidget(void) {
	return m_tree;
}

ot::TreeWidget* ot::PropertyGrid::getTreeWidget(void) const {
	return m_tree;
}

void ot::PropertyGrid::setupFromConfig(const PropertyGridCfg& _config) {
	for (Property* itm : _config.defaultGroup()->properties()) {
		PropertyGridItem* newItm = new PropertyGridItem;
		newItm->setupFromConfig(itm);
		this->addRootItem(newItm);
		newItm->finishSetup();
	}
	for (PropertyGroup* group : _config.rootGroups()) {
		PropertyGridGroup* newGroup = new PropertyGridGroup;
		newGroup->setupFromConfig(group);
		this->addGroup(newGroup);
		newGroup->finishSetup();
	}
}

void ot::PropertyGrid::addRootItem(PropertyGridItem* _item) {
	m_tree->addTopLevelItem(_item);
}

void ot::PropertyGrid::addGroup(PropertyGridGroup* _group) {
	m_tree->addTopLevelItem(_group);
}

void ot::PropertyGrid::clear(void) {
	m_tree->clear();
}

void ot::PropertyGrid::slotPropertyChanged() {

}

void ot::PropertyGrid::slotItemChanged(QTreeWidgetItem* _item, int _column) {

}