//! @file BlockPickerWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockPickerDockWidget.h"
#include "OTBlockEditor/BlockPickerWidget.h"
#include "OTBlockEditorAPI/BlockCategoryConfiguration.h"
#include "OpenTwinCore/otAssert.h"

ot::BlockPickerDockWidget::BlockPickerDockWidget(QWidget* _parentWidget)
	: QDockWidget(_parentWidget)
{
	m_widget = new BlockPickerWidget(this->calcWidgetOrientation());
	setWidget(m_widget->widget());
}

ot::BlockPickerDockWidget::BlockPickerDockWidget(const QString& _title, QWidget* _parentWidget)
	: QDockWidget(_title, _parentWidget)
{
	m_widget = new BlockPickerWidget(this->calcWidgetOrientation());
	setWidget(m_widget->widget());
}

ot::BlockPickerDockWidget::BlockPickerDockWidget(BlockPickerWidget* _customPickerWidget, const QString& _title, QWidget* _parentWidget)
	: QDockWidget(_title, _parentWidget), m_widget(_customPickerWidget)
{
	otAssert(m_widget, "No block picker widget provided");
	setWidget(m_widget->widget());
}

ot::BlockPickerDockWidget::~BlockPickerDockWidget() {
	if (m_widget) delete m_widget;
}

void ot::BlockPickerDockWidget::resizeEvent(QResizeEvent* _event) {
	QDockWidget::resizeEvent(_event);
	Qt::Orientation o = this->calcWidgetOrientation();
	if (o != m_widget->orientation()) m_widget->setOrientation(o);
}

void ot::BlockPickerDockWidget::addTopLevelCategory(ot::BlockCategoryConfiguration* _topLevelCategory) {
	m_widget->addTopLevelCategory(_topLevelCategory);
}

void ot::BlockPickerDockWidget::addTopLevelCategories(const std::list<ot::BlockCategoryConfiguration*>& _topLevelCategories) {
	m_widget->addTopLevelCategories(_topLevelCategories);
}

void ot::BlockPickerDockWidget::clear(void) {
	m_widget->clear();
}
