// @otlicense
// File: ScreenshotDialog.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
