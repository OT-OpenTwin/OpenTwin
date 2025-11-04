// @otlicense

// OpenTwin header
#include "OTWidgets/Label.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PixmapEditWidget.h"
#include "ScreenshotDialog.h"

// Qt header
#include <QtWidgets/qlayout.h>

ot::ScreenshotDialog::ScreenshotDialog(QWidget* _parent)
	: Dialog(_parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	Label* infoLabel = new Label("Use the mouse to select the area of the screenshot you want to keep.", this);
	infoLabel->setAlignment(Qt::AlignCenter);
	infoLabel->setWordWrap(true);
	QFont f = infoLabel->font();
	f.setItalic(true);
	infoLabel->setFont(f);
	layout->addWidget(infoLabel);

	QHBoxLayout* topLayout = new QHBoxLayout;
	layout->addLayout(topLayout, 1);
	m_editor = new PixmapEditWidget(this);
	topLayout->addStretch(1);
	topLayout->addWidget(m_editor);
	topLayout->addStretch(1);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	layout->addLayout(buttonLayout);
	buttonLayout->addStretch(1);

	PushButton* confirmButton = new PushButton("Confirm", this);
	buttonLayout->addWidget(confirmButton);
	connect(confirmButton, &QPushButton::clicked, this, &ScreenshotDialog::closeConfirm);

	PushButton* cancelButton = new PushButton("Cancel", this);
	buttonLayout->addWidget(cancelButton);
	connect(cancelButton, &QPushButton::clicked, this, &ScreenshotDialog::closeCancel);
}

ot::ScreenshotDialog::~ScreenshotDialog() {

}

void ot::ScreenshotDialog::setPixmap(const QPixmap& _pixmap) {
	m_editor->setPixmap(_pixmap);
}

void ot::ScreenshotDialog::setResultSize(const QSize& _size) {
	m_editor->setFixedSize(_size);
	m_editor->setResultSize(_size);
}

QPixmap ot::ScreenshotDialog::getResultPixmap() const {
	return m_editor->getResultPixmap();
}
