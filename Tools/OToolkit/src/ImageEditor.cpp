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
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qfiledialog.h>

ImageEditor::ImageEditor() {

}

ImageEditor::~ImageEditor() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

QWidget* ImageEditor::runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets) {
	using namespace ot;
	
	// Create layouts
	Splitter* rootSplitter = new Splitter;
	m_root = rootSplitter;
	QWidget* rLayW = new QWidget;
	QVBoxLayout* rLay = new QVBoxLayout(rLayW);

	// Create controls
	
	// General
	
	// Setup controls

	// Setup layouts
	
	// Create menu
	QAction* actionImport = _rootMenu->addAction("Import Image");
	QAction* actionCalculate = _rootMenu->addAction("Calculate");
	QAction* actionExport = _rootMenu->addAction("Export Image");
	
	// Initialize data
	
	// Connect signals
	this->connect(actionImport, &QAction::triggered, this, &ImageEditor::slotImport);
	this->connect(actionCalculate, &QAction::triggered, this, &ImageEditor::slotCalculate);
	this->connect(actionExport, &QAction::triggered, this, &ImageEditor::slotExport);
	
	return rootSplitter;
}

void ImageEditor::restoreToolSettings(QSettings& _settings) {

}

bool ImageEditor::prepareToolShutdown(QSettings& _settings) {
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

void ImageEditor::slotImport(void) {

}

void ImageEditor::slotCalculate(void) {

}

void ImageEditor::slotExport(void) {

}