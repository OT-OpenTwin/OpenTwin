//! @file ColorStyleEditor.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit header
#include "ColorStyleEditor.h"
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/PropertyStringList.h"
#include "OTWidgets/TabWidget.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/PropertyInputString.h"
#include "OTWidgets/Painter2DEditButton.h"
#include "OTWidgets/PropertyInputPainter2D.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qfiledialog.h>

#define CSE_GROUP_General "General"
#define CSE_GROUP_StyleValues "Style Values"
#define CSE_GROUP_StyleSheetFiles "Style Sheet Files"
#define CSE_GROUP_StyleSheetColors "Style Sheet Colors"

#define CSE_Name "Name"

#define CSE_TAB_Base "Base"
#define CSE_TAB_Generated "Generated"

#define CSE_COLOR_Widget_Background_1 "Widget Background 1"
#define CSE_COLOR_Widget_Background_2 "Widget Background 2"
#define CSE_COLOR_Widget_Background_3 "Widget Background 3"
#define CSE_COLOR_Widget_Background_4 "Widget Background 4"
#define CSE_COLOR_Widget_Background_5 "Widget Background 5"
#define CSE_COLOR_Widget_Background_6 "Widget Background 6"
#define CSE_COLOR_Widget_Background_7 "Widget Background 7"
#define CSE_COLOR_Widget_Background_8 "Widget Background 8"
#define CSE_COLOR_Widget_Background_9 "Widget Background 9"
#define CSE_COLOR_Accent_1 "Accent Color 1"
#define CSE_COLOR_Accent_2 "Accent Color 2"
#define CSE_COLOR_Accent_3 "Accent Color 3"
#define CSE_COLOR_Accent_4 "Accent Color 4"
#define CSE_COLOR_Accent_5 "Accent Color 5"
#define CSE_COLOR_Accent_6 "Accent Color 6"
#define CSE_COLOR_Accent_7 "Accent Color 7"
#define CSE_COLOR_Accent_8 "Accent Color 8"
#define CSE_COLOR_Accent_9 "Accent Color 9"

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
	QSplitter* rootSplitter = new QSplitter;
	m_root = rootSplitter;
	QWidget* rLayW = new QWidget;
	QVBoxLayout* rLay = new QVBoxLayout(rLayW);

	// Create controls
	m_propertyGrid = new ot::PropertyGrid;

	using namespace ot;

	// General
	PropertyGroup* generalGroup = new PropertyGroup(CSE_GROUP_General);
	m_nameProp = new PropertyString(CSE_Name, std::string());
	generalGroup->addProperty(m_nameProp);

	m_styleValuesGroup = new PropertyGroup(CSE_GROUP_StyleValues);
	m_colorsGroup = new PropertyGroup(CSE_GROUP_StyleSheetColors);
	m_fileGroup = new PropertyGroup(CSE_GROUP_StyleSheetFiles);

	m_propertyGridConfig.setRootGroups({ generalGroup, m_styleValuesGroup, m_colorsGroup, m_fileGroup });

	m_editor = new ot::TextEditor;
	m_editor->setReadOnly(true);
	m_baseEditor = new ot::TextEditor;
	m_baseEditor->setReadOnly(true);

	m_editorTab = new TabWidget;
	m_editorTab->addTab(m_baseEditor, CSE_TAB_Base);
	m_editorTab->addTab(m_editor, CSE_TAB_Generated);

	ot::PushButton* btnGenerate = new ot::PushButton("Generate");
	ot::PushButton* btnApply = new ot::PushButton("Apply");
	rLay->addWidget(m_editorTab, 1);
	rLay->addWidget(btnGenerate);
	rLay->addWidget(btnApply);

	// Setup controls

	// Setup layouts
	rootSplitter->addWidget(m_propertyGrid->getQWidget());
	rootSplitter->addWidget(rLayW);

	// Create menu
	QAction* actionImportConfig = _rootMenu->addAction("Save Configuration");
	QAction* actionExportConfig = _rootMenu->addAction("Load Configuration");
	_rootMenu->addSeparator();
	QAction* actionBright = _rootMenu->addAction("Set Bright");
	QAction* actionDark = _rootMenu->addAction("Set Dark");
	QAction* actionBlue = _rootMenu->addAction("Set Blue");
	_rootMenu->addSeparator();
	QAction* actionApplyAsCurrent = _rootMenu->addAction("Apply As current");
	QAction* actionExport = _rootMenu->addAction("Export");

	// Initialize data
	this->initializeBrightStyleValues();
	this->initializeStyleSheetBase();
	this->parseStyleSheetBaseFile();
	this->initializePropertyGrid();
	this->slotGenerate();

	// Connect signals
	this->connect(actionImportConfig, &QAction::triggered, this, &ColorStyleEditor::slotImportConfig);
	this->connect(actionExportConfig, &QAction::triggered, this, &ColorStyleEditor::slotExportConfig);
	this->connect(actionBright, &QAction::triggered, this, &ColorStyleEditor::slotBright);
	this->connect(actionDark, &QAction::triggered, this, &ColorStyleEditor::slotDark);
	this->connect(actionBlue, &QAction::triggered, this, &ColorStyleEditor::slotBlue);
	this->connect(btnGenerate, &QPushButton::clicked, this, &ColorStyleEditor::slotGenerate);
	this->connect(btnApply, &QPushButton::clicked, this, &ColorStyleEditor::slotApplyAsCurrent);
	this->connect(actionApplyAsCurrent, &QAction::triggered, this, &ColorStyleEditor::slotApplyAsCurrent);
	this->connect(actionExport, &QAction::triggered, this, &ColorStyleEditor::slotExport);

	return rootSplitter;
}

void ColorStyleEditor::restoreToolSettings(QSettings& _settings) {

}

bool ColorStyleEditor::prepareToolShutdown(QSettings& _settings) {
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

void ColorStyleEditor::slotImportConfig(void) {
	// Select file
	QString fileName = QFileDialog::getOpenFileName(
		m_root,
		"Load Configuration", 
		otoolkit::api::getGlobalInterface()->createSettingsInstance().get()->value("ColorStlyeEditor.LastConfig").toString(),
		"Color Style Editor Configuration File (*.otcsc)"
	);

	if (fileName.isEmpty()) return;
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		OT_LOG_E("Failed to open file for reading");
		return;
	}

	// Read file
	QByteArray data(file.readAll());
	file.close();


	// Parse data
	using namespace ot;
	JsonDocument doc;
	std::string dataC = data.toStdString();

	if (!doc.fromJson(dataC)) {
		OT_LOG_E("Failed to parse configuration");
		return;
	}

	// Clean old data
	m_styleValues.clear();
	m_styleValuesGroup->clear(true);

	m_colors.clear();
	m_colorsGroup->clear(true);

	m_files.clear();
	m_fileGroup->clear(true);

	// Import data
	try {
		ConstJsonArray stylesArr = json::getArray(doc, "Styles");
		for (JsonSizeType i = 0; i < stylesArr.Size(); i++) {
			ConstJsonObject stylesObj = json::getObject(stylesArr, i);

		}

		ConstJsonArray colorArr = json::getArray(doc, "Colors");

		ConstJsonArray fileArr = json::getArray(doc, "Files");

	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
	}
	catch (...) {
		OT_LOG_E("Unknown error");
	}
	
}

void ColorStyleEditor::slotExportConfig(void) {
	QString fileName = QFileDialog::getSaveFileName(
		m_root,
		"Save Configuration",
		otoolkit::api::getGlobalInterface()->createSettingsInstance().get()->value("ColorStlyeEditor.LastConfig").toString(),
		"Color Style Editor Configuration File (*.otcsc)"
	);

	if (fileName.isEmpty()) return;
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly)) {
		OT_LOG_E("Failed to open file for writing");
		return;
	}

	using namespace ot;
	JsonDocument doc;
	JsonArray stylesArr;
	JsonArray colorsArr;
	JsonArray filesArr;

	doc.AddMember("Name", JsonString(m_nameProp->value(), doc.GetAllocator()), doc.GetAllocator());

	JsonArray styleArr;
	for (const auto& it : m_styleValues) {
		JsonObject styleObj;
		it.second->addToJsonObject(styleObj, doc.GetAllocator());
		styleArr.PushBack(styleObj, doc.GetAllocator());
	}
	doc.AddMember("Styles", styleArr, doc.GetAllocator());

	JsonArray colorArr;
	for (const auto& it : m_colors) {
		JsonObject colorObj;
		it.second->addToJsonObject(colorObj, doc.GetAllocator());
		styleArr.PushBack(colorObj, doc.GetAllocator());
	}
	doc.AddMember("Colors", colorArr, doc.GetAllocator());

	JsonArray fileArr;
	for (const auto& it : m_styleValues) {
		JsonObject fileObj;
		it.second->addToJsonObject(fileObj, doc.GetAllocator());
		fileArr.PushBack(fileObj, doc.GetAllocator());
	}
	doc.AddMember("Files", fileArr, doc.GetAllocator());

	file.write(QByteArray::fromStdString(doc.toJson()));
	file.close();

	otoolkit::api::getGlobalInterface()->createSettingsInstance().get()->setValue("ColorStlyeEditor.LastConfig", fileName);
}

void ColorStyleEditor::slotBright(void) {
	this->initializeBrightStyleValues();
}

void ColorStyleEditor::slotDark(void) {
	this->initializeDarkStyleValues();
}

void ColorStyleEditor::slotBlue(void) {
	this->initializeBlueStyleValues();
}

void ColorStyleEditor::slotGenerate(void) {
	std::string gen;
	if (!this->generateFile(gen)) return;
	m_editor->setPlainText(QString::fromStdString(gen));

	m_editorTab->setCurrentIndex(1);
}

void ColorStyleEditor::slotApplyAsCurrent(void) {

}

void ColorStyleEditor::slotExport(void) {

}

// ###########################################################################################################################################################################################################################################################################################################################

void ColorStyleEditor::initializeStyleSheetBase(void) {
	QFile templateFile(":/data/StyleSheetBase.otssb");
	if (!templateFile.exists()) {
		OT_LOG_E("Style sheet base file does not exist");
		return;
	}
	if (!templateFile.open(QIODevice::ReadOnly)) {
		OT_LOG_E("Failed to open style sheet base file for reading");
		return;
	}
	m_sheetBase = templateFile.readAll();
	templateFile.close();

	if (m_sheetBase.isEmpty()) {
		OT_LOG_E("Style sheet base file is empty");
		return;
	}

	m_baseEditor->setPlainText(QString::fromStdString(m_sheetBase.toStdString()));
}

void ColorStyleEditor::cleanUpData(void) {
	for (const auto& p : m_styleValues) {
		delete p.second;
	}
	m_styleValues.clear();

	for (const auto& p : m_colors) {
		delete p.second;
	}
	m_colors.clear();

	for (const auto& f : m_files) {
		delete f.second;
	}
	m_files.clear();
}

void ColorStyleEditor::initializeBrightStyleValues(void) {
	// Clean up data
	this->cleanUpData();

	// Initialize default style values
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(255, 255, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(0, 0, 0))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsHoverBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(72, 72, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsHoverForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(255, 255, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsSelectedBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(72, 255, 72))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsSelectedForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(0, 0, 0))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_WindowBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(240, 240, 240))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_WindowForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(0, 0, 0))));

	// Initialize default colors
	m_colors.insert_or_assign(CSE_COLOR_Accent_1, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color())));
	m_colors.insert_or_assign(CSE_COLOR_Accent_2, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color())));
	m_colors.insert_or_assign(CSE_COLOR_Accent_3, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color())));
	m_colors.insert_or_assign(CSE_COLOR_Accent_4, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color())));
	m_colors.insert_or_assign(CSE_COLOR_Widget_Background_1, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color())));
	m_colors.insert_or_assign(CSE_COLOR_Widget_Background_2, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color())));
	m_colors.insert_or_assign(CSE_COLOR_Widget_Background_3, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color())));
	m_colors.insert_or_assign(CSE_COLOR_Widget_Background_4, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color())));
	m_colors.insert_or_assign(CSE_COLOR_Widget_Background_5, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color())));
}

void ColorStyleEditor::initializeDarkStyleValues(void) {
	// Clean up data
	this->cleanUpData();

	// Initialize default style values
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(255, 255, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(0, 0, 0))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsHoverBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(72, 72, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsHoverForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(255, 255, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsSelectedBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(72, 255, 72))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsSelectedForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(0, 0, 0))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_WindowBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(240, 240, 240))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_WindowForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(0, 0, 0))));
}

void ColorStyleEditor::initializeBlueStyleValues(void) {
	// Clean up data
	this->cleanUpData();

	// Initialize default style values
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(255, 255, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(0, 0, 0))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsHoverBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(72, 72, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsHoverForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(255, 255, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsSelectedBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(72, 255, 72))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsSelectedForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(0, 0, 0))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_WindowBackground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(240, 240, 240))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_WindowForeground, new ot::PropertyPainter2D(new ot::FillPainter2D(ot::Color(0, 0, 0))));
}

void ColorStyleEditor::parseStyleSheetBaseFile(void) {
	// Create copy of the default sheet base file
	std::string otssb = m_sheetBase.toStdString();

	// Split the file into lines
	std::list<std::string> lst;
	size_t ix = otssb.find('\n');
	while (ix != std::string::npos) {
		lst.push_back(otssb.substr(0, ix));
		otssb = otssb.substr(ix + 1);
		ix = otssb.find('\n');
	}
	lst.push_back(otssb);

	// Parse each line
	size_t lineCt = 0;
	for (std::string l : lst) {
		lineCt++;
		// Find opening %
		ix = l.find('%');
		while (ix != std::string::npos) {
			// Find closing %
			size_t ix2 = l.find('%', ix + 1);
			if (ix2 == std::string::npos) {
				OT_LOG_EA("StyleSheetBase.otsb file broken");
				return;
			}

			// Get key name pair
			std::string kn = l.substr(ix + 1, (ix2 - ix) - 1);

			// Cut string and find next opening %
			l = l.substr(ix2 + 1);
			ix = l.find('%');

			// Find key name splitter ':'
			ix2 = kn.find(':');
			if (ix2 == std::string::npos) {
				OT_LOG_EA("StyleSheetBase.otsb file broken");
				continue;
			}

			// Get key and name
			std::string k = kn.substr(0, ix2);
			std::string n = kn.substr(ix2 + 1);
			if (n.empty()) {
				OT_LOG_W("StyleSheetBase.otsb file broken: Key empty");
				continue;
			}
			// Check key
			if (k == "color") {
				// Create color entry if doesnt exist
				const auto& it = m_colors.find(n);
				if (it == m_colors.end()) {
					m_colors.insert_or_assign(n, new ot::PropertyPainter2D(new ot::FillPainter2D));
				}
			}
			else if (k == "file") {
				// Create file entry
				const auto& it = m_files.find(n);
				if (it == m_files.end()) {
					m_files.insert_or_assign(n, new ot::PropertyString);
				}
			}
			else {
				// Unknown entry
				OT_LOG_W("StyleSheetBase.otsb file broken: Invalid key \"" + k + "\"");
				return;
			}

		}
	}
}

void ColorStyleEditor::initializePropertyGrid(void) {
	m_propertyGrid->blockSignals(true);

	using namespace ot;
	
	// Style values
	m_styleValuesGroup->clear();
	for (const auto& it : m_styleValues) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		m_styleValuesGroup->addProperty(it.second);
	}

	// Stylesheet Colors
	m_colorsGroup->clear();
	for (const auto& it : m_colors) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		m_colorsGroup->addProperty(it.second);
	}

	// Stylesheet Files
	m_fileGroup->clear();
	for (const auto& it : m_files) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		m_fileGroup->addProperty(it.second);
	}

	m_propertyGrid->setupFromConfig(m_propertyGridConfig);
	m_propertyGrid->blockSignals(false);
}

bool ColorStyleEditor::generateFile(std::string& _result) {
	OTAssertNullptr(m_propertyGrid);
	using namespace ot;
	_result.clear();

	// Get name property
	const PropertyGridItem* nameItm = m_propertyGrid->findItem(CSE_Name);
	if (!nameItm) {
		OT_LOG_E("Name property not found");
		return false;
	}

	// Get name value
	const PropertyInputString* iName = dynamic_cast<const PropertyInputString*>(nameItm->getInput());
	OTAssertNullptr(iName);
	std::string cName = iName->getCurrentValue().toString().toStdString();

	// Get style values
	JsonDocument styleValuesDoc(rapidjson::kArrayType);
	const PropertyGridGroup* gStyleValues = m_propertyGrid->findGroup(CSE_GROUP_StyleValues);
	OTAssertNullptr(gStyleValues);
	for (const PropertyGridItem* itm : gStyleValues->childProperties()) {
		const PropertyInputPainter2D* inp = dynamic_cast<const PropertyInputPainter2D*>(itm->getInput());
		if (!inp) {
			OT_LOG_E("Input cast failed");
			return false;
		}
		JsonObject pObj;
		inp->getButton()->getPainter()->addToJsonObject(pObj, styleValuesDoc.GetAllocator());
		styleValuesDoc.PushBack(pObj, styleValuesDoc.GetAllocator());
	}
	std::string cStyleValue = styleValuesDoc.toJson();

	// Clean up new line if exists
	size_t ix = cStyleValue.find('\n');
	while (ix != std::string::npos) {
		cStyleValue.erase(ix);
		ix = cStyleValue.find('\n');
	}

	// Build up color replace map
	std::map<QString, QString> replacementMap;
	{
		const PropertyGridGroup* gColors = m_propertyGrid->findGroup(CSE_GROUP_StyleSheetColors);
		OTAssertNullptr(gColors);
		for (const PropertyGridItem* itm : gColors->childProperties()) {
			const PropertyInputPainter2D* inp = dynamic_cast<const PropertyInputPainter2D*>(itm->getInput());
			if (!inp) {
				OT_LOG_E("Input cast failed");
				return false;
			}
			QString k = "%color:" + QString::fromStdString(inp->propertyName()) + "%";
			if (replacementMap.count(k)) {
				OT_LOG_W("Duplicate key \"" + k.toStdString() + "\"");
				return false;
			}
			replacementMap.insert_or_assign(k, QString::fromStdString(inp->getButton()->getPainter()->generateQss()));
		}
	}
	{
		const PropertyGridGroup* gFiles = m_propertyGrid->findGroup(CSE_GROUP_StyleSheetFiles);
		OTAssertNullptr(gFiles);
		for (const PropertyGridItem* itm : gFiles->childProperties()) {
			const PropertyInputString* inp = dynamic_cast<const PropertyInputString*>(itm->getInput());
			if (!inp) {
				OT_LOG_E("Input cast failed");
				return false;
			}
			QString k = "%file:" + QString::fromStdString(inp->propertyName()) + "%";
			if (replacementMap.count(k)) {
				OT_LOG_W("Duplicate key \"" + k.toStdString() + "\"");
				return false;
			}
			replacementMap.insert_or_assign(k, "%root%" + inp->getCurrentText());
		}
	}

	QString base = QString::fromStdString(m_sheetBase.toStdString());
	for (const auto& it : replacementMap) {
		base.replace(it.first, it.second);
	}

	_result = "name:" + cName + "\n" +
		"values:" + cStyleValue + "\n" +
		"sheet:\n";
	_result.append(base.toStdString());

	return true;
}