//! @file ColorStyleEditor.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit header
#include "ColorStyleEditor.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/PropertyStringList.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PropertyGrid.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qsplitter.h>

#define CSE_GROUP_General "General"
#define CSE_GROUP_StyleValues "Style Values"
#define CSE_GROUP_StyleSheetFiles "Style Sheet Files"
#define CSE_GROUP_StyleSheetColors "Style Sheet Colors"

#define CSE_Name "Name"

ColorStyleEditor::ColorStyleEditor() {

}

ColorStyleEditor::~ColorStyleEditor() {
	for (const auto& p : m_styleValues) {
		delete p.second;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

QWidget* ColorStyleEditor::runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets) {
	// Create layouts
	m_root = new QSplitter;
	QWidget* rLayW = new QWidget;
	QVBoxLayout* rLay = new QVBoxLayout(rLayW);

	// Create controls
	m_propertyGrid = new ot::PropertyGrid;
	m_editor = new ot::TextEditor;
	m_editor->setReadOnly(true);
	ot::PushButton* btn = new ot::PushButton("Generate");
	rLay->addWidget(m_editor, 1);
	rLay->addWidget(btn);

	// Setup controls

	// Setup layouts
	m_root->addWidget(m_propertyGrid->getQWidget());

	// Create menu
	QAction* actionReset = _rootMenu->addAction("Reset");
	QAction* actionGenerate = _rootMenu->addAction("Generate");
	QAction* actionExport = _rootMenu->addAction("Export");

	// Initialize data
	this->initializeStyleValues();
	this->initializePropertyGrid();

	// Connect signals
	this->connect(actionReset, &QAction::triggered, this, &ColorStyleEditor::slotReset);
	this->connect(actionGenerate, &QAction::triggered, this, &ColorStyleEditor::slotGenerate);
	this->connect(actionExport, &QAction::triggered, this, &ColorStyleEditor::slotExport);

	return m_root;
}

void ColorStyleEditor::restoreToolSettings(QSettings& _settings) {

}

bool ColorStyleEditor::prepareToolShutdown(QSettings& _settings) {
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

void ColorStyleEditor::slotReset(void) {
	this->initializeStyleValues();
}

void ColorStyleEditor::slotGenerate(void) {

}

void ColorStyleEditor::slotExport(void) {

}

// ###########################################################################################################################################################################################################################################################################################################################

void ColorStyleEditor::initializeStyleValues(void) {
	for (const auto& p : m_styleValues) {
		delete p.second;
	}
	m_styleValues.clear();

	for (const auto& p : m_colors) {
		delete p.second;
	}
	m_colors.clear();
	m_files.clear();

	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(255, 255, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(0, 0, 0))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsHoverBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(72, 72, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsHoverForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(255, 255, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsSelectedBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(72, 255, 72))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsSelectedForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(0, 0, 0))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_WindowBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(240, 240, 240))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_WindowForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(0, 0, 0))));
	
	QFile templateFile(":/data/StyleSheetBase.otssb");
	std::string tmp;
	if (!templateFile.exists()) return;
	if (!templateFile.open(QIODevice::ReadOnly)) return;
	tmp = templateFile.readAll().toStdString();
	templateFile.close();

	std::list<std::string> lst;
	size_t ix = tmp.find('\n');
	while (ix != std::string::npos) {
		lst.push_back(tmp.substr(0, ix));
		tmp = tmp.substr(ix + 1);
		ix = tmp.find('\n');
	}
	lst.push_back(tmp);

	size_t lineCt = 0;
	for (std::string l : lst) {
		lineCt++;
		ix = l.find('\r');
		while (ix != std::string::npos) {
			l.erase(ix);
			ix = l.find('\r');
		}
		ix = l.find('%');
		while (ix != std::string::npos) {
			size_t ix2 = l.find('%', ix + 1);
			if (ix2 == std::string::npos) {
				OT_LOG_EA("StyleSheetBase.otsb file broken");
				return;
			}
			std::string kv = l.substr(ix + 1, (ix2 - ix) - 1);
			l = l.substr(ix2 + 1);
			ix = l.find('%');

			ix2 = kv.find(':');
			if (ix2 == std::string::npos) {
				OT_LOG_EA("StyleSheetBase.otsb file broken");
				continue;
			}

			std::string k = kv.substr(0, ix2);
			std::string n = kv.substr(ix2 + 1);
			if (n.empty()) {
				OT_LOG_W("StyleSheetBase.otsb file broken: Key empty");
				continue;
			}
			if (k == "color") {
				const auto& it = m_colors.find(n);
				if (it == m_colors.end()) {
					m_colors.insert_or_assign(n, new ot::PropertyPainter2D(new ot::FillPainter2D));
				}
			}
			else if (k == "file") {
				const auto& it = m_files.find(n);
				if (it == m_files.end()) {
					m_files.insert_or_assign(n, new ot::PropertyString);
				}
			}
			else {
				OT_LOG_W("StyleSheetBase.otsb file broken: Invalid key \"" + k + "\"");
				return;
			}

		}
	}
}

void ColorStyleEditor::initializePropertyGrid(void) {
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
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		styleValuesGroup->addProperty(it.second);
	}

	// Stylesheet Colors
	PropertyGroup* styleSheetColorGroup = new PropertyGroup(CSE_GROUP_StyleSheetColors);
	for (const auto& it : m_colors) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		styleSheetColorGroup->addProperty(it.second);
	}

	// Stylesheet Files
	PropertyGroup* styleSheetFilePath = new PropertyGroup(CSE_GROUP_StyleSheetFiles);
	for (const auto& it : m_files) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		styleSheetFilePath->addProperty(it.second);
	}

	cfg.setRootGroups({ generalGroup, styleValuesGroup, styleSheetColorGroup, styleSheetFilePath });

	m_propertyGrid->setupFromConfig(cfg);
	m_propertyGrid->blockSignals(false);
}