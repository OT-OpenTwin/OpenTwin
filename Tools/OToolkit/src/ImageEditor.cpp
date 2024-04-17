//! @file ImageEditor.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit header
#include "ImageEditor.h"
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCore/StringHelper.h"
#include "OTWidgets/SpinBox.h"
#include "OTWidgets/Splitter.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/ImagePreview.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qfiledialog.h>

ImageEditor::ImageEditor() {

}

ImageEditor::~ImageEditor() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

bool ImageEditor::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;
	
	// Create layouts
	Splitter* rootSplitter = new Splitter;
	m_root = rootSplitter;

	QWidget* lLayW = new QWidget;
	QVBoxLayout* lLay = new QVBoxLayout(lLayW);

	QWidget* rLayW = new QWidget;
	QVBoxLayout* rLay = new QVBoxLayout(rLayW);

	// Create controls
	PushButton* btnImport = new PushButton("Import");
	PushButton* btnCalculate = new PushButton("Calculate");
	PushButton* btnExport = new PushButton("Export");

	m_original = new ImagePreview;
	m_converted = new ImagePreview;

	m_toolBar = new ImageEditorToolBar;

	QScrollArea* scrollL = new QScrollArea;
	scrollL->setWidgetResizable(true);
	scrollL->setWidget(m_original);

	QScrollArea* scrollR = new QScrollArea;
	scrollR->setWidgetResizable(true);
	scrollR->setWidget(m_converted);

	// Setup controls

	// Setup layouts
	rootSplitter->addWidget(lLayW);
	rootSplitter->addWidget(rLayW);

	lLay->addWidget(scrollL);
	lLay->addWidget(btnImport);
	lLay->addWidget(btnCalculate);

	rLay->addWidget(scrollR);
	rLay->addWidget(btnExport);

	// Create menu
	QAction* actionImport = _rootMenu->addAction("Import Image");
	QAction* actionCalculate = _rootMenu->addAction("Calculate");
	QAction* actionExport = _rootMenu->addAction("Export Image");
	
	// Initialize data
	
	// Connect signals
	this->connect(actionImport, &QAction::triggered, this, &ImageEditor::slotImport);
	this->connect(btnImport, &QPushButton::clicked, this, &ImageEditor::slotImport);

	this->connect(actionCalculate, &QAction::triggered, this, &ImageEditor::slotCalculate);
	this->connect(btnCalculate, &QPushButton::clicked, this, &ImageEditor::slotCalculate);

	this->connect(actionExport, &QAction::triggered, this, &ImageEditor::slotExport);
	this->connect(btnExport, &QPushButton::clicked, this, &ImageEditor::slotExport);
	
	_content.setRootWidget(rootSplitter);
	return true;
}

void ImageEditor::restoreToolSettings(QSettings& _settings) {

}

bool ImageEditor::prepareToolShutdown(QSettings& _settings) {
	return true;
}

void ImageEditor::toolWasShown(void) {
	m_toolBar->setHidden(false);
}

void ImageEditor::toolWasHidden(void) {
	m_toolBar->setHidden(true);
}

// ###########################################################################################################################################################################################################################################################################################################################

void ImageEditor::slotImport(void) {
	auto settings = otoolkit::api::getGlobalInterface()->createSettingsInstance();

	QString fileName = QFileDialog::getOpenFileName(m_root, "Open Image", settings->value("ImageEditor.LastImportFile", QString()).toString(), "Images (*.png *.jpg *.jpeg *.bmp *.gif *.tiff)");
	if (fileName.isEmpty()) return;

	settings->setValue("ImageEditor.LastImportFile", fileName);

	QImage img(fileName);
	m_original->setImage(img);
	m_original->setFocus();
}

void ImageEditor::slotCalculate(void) {

}

void ImageEditor::slotExport(void) {

}