//! @file PropertyGrid.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"

ot::PropertyGrid::PropertyGrid(QObject* _parentObject) : QObject(_parentObject), m_defaultGroup(nullptr) {
	m_tree = new TreeWidget;
}

ot::PropertyGrid::~PropertyGrid() {
	delete m_tree;
}

void ot::PropertyGrid::addGroup(PropertyGridGroup* _group) {
	m_tree->addTopLevelItem(_group);
}

void ot::PropertyGrid::slotPropertyChanged() {

}