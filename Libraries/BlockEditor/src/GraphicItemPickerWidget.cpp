//! @file GraphicItemPickerWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/GraphicItemPickerWidget.h"

// Qt Header
#include <QtWidgets/qsplitter.h>

ot::GraphicItemPickerWidget::GraphicItemPickerWidget() : m_navigation(nullptr), m_preview(nullptr), m_previewScene(nullptr), m_splitter(nullptr) {
	// Create controls

}

ot::GraphicItemPickerWidget::~GraphicItemPickerWidget() {

}

QWidget* ot::GraphicItemPickerWidget::widget(void) {
	return m_splitter;
}