//! @file PropertyGridItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyInput.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/PropertyInputFactory.h"

// Qt header
#include <QtWidgets/qlayout.h>

ot::PropertyGridItem::PropertyGridItem()
	: m_input(nullptr), m_parentGroup(nullptr)
{
	m_titleLayoutW = new QWidget;
	m_titleLayoutW->setObjectName("PropertyGridItemTitleLayout");
	QHBoxLayout* titleLayout = new QHBoxLayout(m_titleLayoutW);
	titleLayout->setContentsMargins(0, 0, 0, 0);
	m_titleLabel = new Label;
	m_titleLabel->setObjectName("PropertyGridItemTitleLabel");
	m_deleteLabel = new Label;
	m_deleteLabel->setObjectName("PropertyGridItemDeleteLabel");
	m_deleteLabel->setHidden(true);

	titleLayout->addWidget(m_titleLabel, 1);
	titleLayout->addWidget(m_deleteLabel);

	this->slotGlobalStyleChanged(GlobalColorStyle::instance().getCurrentStyle());

	this->connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &PropertyGridItem::slotGlobalStyleChanged);
	this->connect(m_deleteLabel, &Label::mousePressed, this, &PropertyGridItem::slotDeleteRequested);
}

ot::PropertyGridItem::~PropertyGridItem() {
	if (m_input) delete m_input;
	m_input = nullptr;
}

bool ot::PropertyGridItem::setupFromConfig(const Property * _config) {
	m_titleLabel->setText(QString::fromStdString(_config->getPropertyTitle()));
	if (m_input) delete m_input;
	m_input = PropertyInputFactory::createInput(_config);

	m_deleteLabel->setHidden(!(m_input->data().getPropertyFlags() & Property::IsDeletable));

	this->connect(m_input, &PropertyInput::inputValueChanged, this, &PropertyGridItem::slotValueChanged);

	return true;
}

void ot::PropertyGridItem::finishSetup(void) {
	TreeWidget* tree = dynamic_cast<TreeWidget*>(this->treeWidget());
	if (!tree) {
		OT_LOG_EA("Tree cast failed");
		return;
	}
	tree->setItemWidget(this, 0, m_titleLayoutW);
	tree->setItemWidget(this, 1, m_input->getQWidget());
	this->setHidden(this->getPropertyData().getPropertyFlags() & Property::IsHidden);
	this->setFirstColumnSpanned(false);
}

std::string ot::PropertyGridItem::getGroupName(void) const {
	if (m_parentGroup) return m_parentGroup->getName();
	else {
		OT_LOG_EA("No parent group set");
		return std::string();
	}
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
		this->disconnect(m_input, &PropertyInput::inputValueChanged, this, &PropertyGridItem::slotValueChanged);
		tree->removeItemWidget(this, 1);
		delete m_input;
	}
	m_input = _input;
	
	tree->setItemWidget(this, 1, m_input->getQWidget());
	this->connect(m_input, &PropertyInput::inputValueChanged, this, &PropertyGridItem::slotValueChanged);
}

ot::PropertyBase ot::PropertyGridItem::getPropertyData(void) const {
	if (m_input) return m_input->data();
	else return PropertyBase();
}

std::string ot::PropertyGridItem::getPropertyType(void) const {
	if (m_input) return m_input->getPropertyType();
	else return std::string();
}

void ot::PropertyGridItem::slotValueChanged(void) {
	ot::Property* signalProperty = this->createSignalProperty();
	Q_EMIT inputValueChanged(signalProperty);
	if (signalProperty) delete signalProperty;
}

void ot::PropertyGridItem::slotDeleteRequested(void) {
	ot::Property* signalProperty = this->createSignalProperty();
	Q_EMIT deleteRequested(signalProperty);
	if (signalProperty) delete signalProperty;
}

void ot::PropertyGridItem::slotGlobalStyleChanged(const ColorStyle& _style) {
	QString pth = GlobalColorStyle::instance().getCurrentStyle().getFile(ColorStyleFileEntry::PropertyItemDeleteIcon);
	QIcon ico(pth);
	m_deleteLabel->setPixmap(ico.pixmap(16, 16));
}

void ot::PropertyGridItem::clear(void) {

}

ot::Property* ot::PropertyGridItem::createSignalProperty(void) {
	OTAssertNullptr(m_input);
	
	// Create property configuration
	Property* prop = m_input->createPropertyConfiguration();
	OTAssertNullptr(prop);

	// Create group configuration(s)
	PropertyGridGroup* group = m_parentGroup;
	PropertyGroup* groupCfg = nullptr;
	while (group) {
		PropertyGroup* newGroup = group->createConfiguration(false);
		if (groupCfg) {
			newGroup->addChildGroup(groupCfg);
		}
		else {
			newGroup->addProperty(prop);
		}
		groupCfg = newGroup;
		group = group->getParentPropertyGroup();
	}

	return prop;
}
