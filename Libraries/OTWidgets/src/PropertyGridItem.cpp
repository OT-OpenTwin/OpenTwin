//! @file PropertyGridItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyInput.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyInputFactory.h"

ot::PropertyGridItem::PropertyGridItem()
	: m_input(nullptr), m_propertyBrush(QColor(Qt::white)), m_type(Property::NullType), m_isDeleteable(false)
{

}

ot::PropertyGridItem::~PropertyGridItem() {

}

bool ot::PropertyGridItem::setupFromConfig(const Property * _config) {
	m_name = _config->propertyName();
	m_type = _config->getPropertyType();
	this->setText(0, QString::fromStdString(_config->propertyTitle()));
	if (m_input) delete m_input;
	m_input = PropertyInputFactory::createInput(_config);
	m_isDeleteable = _config->propertyFlags() & Property::IsDeletable;
	
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

void ot::PropertyGridItem::setTitle(const QString& _title) {
	this->setText(0, _title);
}

QString ot::PropertyGridItem::getTitle(void) const {
	return this->text(0);
}

void ot::PropertyGridItem::setInput(PropertyInput* _input) {
	TreeWidget* tree = dynamic_cast<TreeWidget*>(this->treeWidget());
	if (!tree) {
		OT_LOG_EA("Tree cast failed");
		return;
	}

	if (m_input) {
		this->disconnect(m_input, &PropertyInput::inputValueChanged, this, &PropertyGridItem::inputValueChanged);
		tree->removeItemWidget(this, 1);
		delete m_input;
	}
	m_input = _input;
	
	tree->setItemWidget(this, 1, m_input->getQWidget());
	this->connect(m_input, &PropertyInput::inputValueChanged, this, &PropertyGridItem::inputValueChanged);
}

void ot::PropertyGridItem::slotValueChanged(void) {
	Q_EMIT inputValueChanged();
}