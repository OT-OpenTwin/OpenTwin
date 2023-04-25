// OpenTwin Header
#include <openTwin/GraphicItemPickerWidget.h>

// Qt Header
#include <QtWidgets/qsplitter.h>

ot::GraphicItemPickerWidget::GraphicItemPickerWidget() {
	// Create controls

}

ot::GraphicItemPickerWidget::~GraphicItemPickerWidget() {

}

QWidget* ot::GraphicItemPickerWidget::widget(void) {
	return m_splitter;
}