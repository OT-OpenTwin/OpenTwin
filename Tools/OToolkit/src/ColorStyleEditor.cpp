//! @file ColorStyleEditor.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit header
#include "ColorStyleEditor.h"

// OpenTwin header
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/PropertyStringList.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/PropertyGrid.h"

// Qt header
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qsplitter.h>

#define CSE_GROUP_General "General"
#define CSE_GROUP_StyleValues "Style Values"

#define CSE_Name "Name"

ColorStyleEditor::ColorStyleEditor() {

}

ColorStyleEditor::~ColorStyleEditor() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

QWidget* ColorStyleEditor::runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets) {
	// Create layouts
	m_root = new QSplitter;

	// Create controls
	m_propertyGrid = new ot::PropertyGrid;

	// Setup controls

	// Setup layouts
	m_root->addWidget(m_propertyGrid->getQWidget());

	// Create menu
	QAction* actionImport = _rootMenu->addAction("Import");
	QAction* actionPreview = _rootMenu->addAction("Show Preview");
	QAction* actionExport = _rootMenu->addAction("Export");

	// Initialize data
	this->initializeStyleValues();
	this->rebuildProperties();

	// Connect signals
	this->connect(actionImport, &QAction::triggered, this, &ColorStyleEditor::slotImport);
	this->connect(actionExport, &QAction::triggered, this, &ColorStyleEditor::slotExport);
	this->connect(actionPreview, &QAction::triggered, this, &ColorStyleEditor::slotShowPreview);

	return m_root;
}

void ColorStyleEditor::restoreToolSettings(QSettings& _settings) {

}

bool ColorStyleEditor::prepareToolShutdown(QSettings& _settings) {
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

void ColorStyleEditor::slotImport(void) {

}

void ColorStyleEditor::slotExport(void) {

}

void ColorStyleEditor::slotShowPreview(void) {

}

// ###########################################################################################################################################################################################################################################################################################################################

void ColorStyleEditor::initializeStyleValues(void) {
	m_styleValues.clear();
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsBackground, new ot::FillPainter2D(ot::Color(255, 255, 255)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsForeground, new ot::FillPainter2D(ot::Color(0, 0, 0)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsHoverBackground, new ot::FillPainter2D(ot::Color(72, 72, 255)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsHoverForeground, new ot::FillPainter2D(ot::Color(255, 255, 255)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsSelectedBackground, new ot::FillPainter2D(ot::Color(72, 255, 72)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsSelectedForeground, new ot::FillPainter2D(ot::Color(0, 0, 0)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_WindowBackground, new ot::FillPainter2D(ot::Color(240, 240, 240)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_WindowForeground, new ot::FillPainter2D(ot::Color(0, 0, 0)));
}

void ColorStyleEditor::rebuildProperties(void) {
	m_propertyGrid->blockSignals(true);

	using namespace ot;
	PropertyGridCfg cfg;
	
	// General
	PropertyGroup* generalGroup = new PropertyGroup(CSE_GROUP_General);
	PropertyString* nameProperty = new PropertyString(CSE_Name, m_currentName.toStdString());

	generalGroup->addProperty(nameProperty);

	// Style values
	PropertyGroup* styleValuesGroup = new PropertyGroup(CSE_GROUP_StyleValues);
	for (const auto& it : m_styleValues) {
		PropertyPainter2D* newStyleValue = new PropertyPainter2D(it.first, it.second->createCopy());
		styleValuesGroup->addProperty(newStyleValue);
	}

	cfg.setRootGroups({ generalGroup, styleValuesGroup });

	m_propertyGrid->setupFromConfig(cfg);
	m_propertyGrid->blockSignals(false);
}