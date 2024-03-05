//! @file PropertyGridItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Property.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyInput.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyInputFactory.h"

ot::PropertyGridItem::PropertyGridItem() : m_input(nullptr), m_itemBrush(QColor(0, 0, 0, 0)) {

}

ot::PropertyGridItem::~PropertyGridItem() {

}

bool ot::PropertyGridItem::setupFromConfig(Property * _config) {
	this->setText(0, QString::fromStdString(_config->propertyTitle()));
	if (m_input) delete m_input;
	m_input = PropertyInputFactory::createInput(_config);
	
	return true;
}

void ot::PropertyGridItem::finishSetup(void) {
	TreeWidget* tree = dynamic_cast<TreeWidget*>(this->treeWidget());
	if (!tree) {
		OT_LOG_EA("Tree cast failed");
		return;
	}
	tree->setItemWidget(this, 1, m_input->getQWidget());
	this->setFirstColumnSpanned(false);
}