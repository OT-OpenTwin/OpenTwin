//! @file ColorStyleEditor.cpp
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
#include "OTCore/StringHelper.h"
#include "OTCore/PropertyInt.h"
#include "OTCore/PropertyBool.h"
#include "OTCore/PropertyGroup.h"
#include "OTGui/FillPainter2D.h"
#include "OTCore/PropertyColor.h"
#include "OTCore/PropertyString.h"
#include "OTCore/PropertyDouble.h"
#include "OTCore/PropertyGridCfg.h"
#include "OTCore/PropertyStringList.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTWidgets/SpinBox.h"
#include "OTWidgets/Splitter.h"
#include "OTWidgets/TabWidget.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyInputInt.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/PropertyInputString.h"
#include "OTWidgets/PropertyInputDouble.h"
#include "OTWidgets/Painter2DEditButton.h"
#include "OTWidgets/PropertyInputPainter2D.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qfiledialog.h>

#define CSE_GROUP_General "General"
#define CSE_GROUP_StyleFiles "Style Files"
#define CSE_GROUP_StyleInt "Style Integer Numbers"
#define CSE_GROUP_StyleDouble "Style Decimal Numbers"
#define CSE_GROUP_StyleValues "Style Values"
#define CSE_GROUP_Int "Integer Numbers"
#define CSE_GROUP_Double "Decimal Numbers"
#define CSE_GROUP_StyleSheetFiles "Style Sheet Files"
#define CSE_GROUP_StyleSheetColors "Style Sheet Colors"

#define CSE_Name "Name"

#define CSE_TAB_Base "Base"
#define CSE_TAB_Generated "Generated"

#define CSE_COLOR_BorderColor "Border Color"
#define CSE_COLOR_LightBorderColor "Light Border Color"
#define CSE_COLOR_BorderDisabledColor "Border Disabled Color"
#define CSE_COLOR_BorderHoverColor "Border Hover Color"
#define CSE_COLOR_BorderSelectionColor "Border Selection Color"
#define CSE_COLOR_HeaderBackground "Header Background"
#define CSE_COLOR_HeaderHoverForeground "Header Hover Foreground"
#define CSE_COLOR_HeaderSelectionBackground "Header Selection Background"
#define CSE_COLOR_InputBackground "Input Background"
#define CSE_COLOR_WidgetAlternateBackground "Widget Alternate Background"
#define CSE_COLOR_WidgetForeground "Widget Foreground"
#define CSE_COLOR_WidgetBackground "Widget Background"
#define CSE_COLOR_WidgetDisabledBackground "Widget Disabled Background"
#define CSE_COLOR_WidgetDisabledForeground "Widget Disabled Foreground"
#define CSE_COLOR_WidgetHoverBackground "Widget Hover Background"
#define CSE_COLOR_WidgetHoverForeground "Widget Hover Foreground"
#define CSE_COLOR_WidgetSelectionBackground "Widget Selection Background"
#define CSE_COLOR_WidgetSelectionForeground "Widget Selection Foreground"
#define CSE_COLOR_WindowBackground "Window Background"
#define CSE_COLOR_TitleBackground "Title Background"
#define CSE_COLOR_TitleForeground "Title Foreground"
#define CSE_COLOR_TTBFirstTabBackground "TabToolBar First Tab Background"
#define CSE_COLOR_TTBFirstTabForeground "TabToolBar First Tab Foreground"
#define CSE_COLOR_ErrorForeground "Error Foreground"

#define CSE_FILE_LogInBackgroundImage "Log In Background Image"

#define CSE_INT_SplitterHandleWidth "Splitter Handle Width"
#define CSE_INT_SplitterBorderRadius "Splitter Border Radius"

#define CSE_NUMBER_BorderRadius_Big "Border Radius Big"
#define CSE_NUMBER_BorderRadius_Small "Border Radius Small"
#define CSE_NUMBER_OpacityTooltip "Opacity ToolTip"

ColorStyleEditor::ColorStyleEditor() {

}

ColorStyleEditor::~ColorStyleEditor() {
	for (const auto& f : m_styleFiles) {
		delete f.second;
	}
	for (const auto& p : m_styleValues) {
		delete p.second;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

QWidget* ColorStyleEditor::runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets) {
	// Create layouts
	ot::Splitter* rootSplitter = new ot::Splitter;
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
	generalGroup->addProperty(new PropertyStringList("xxx", "Test", std::list<std::string>({ "Test", "Other" })));
	m_styleFilesGroup = new PropertyGroup(CSE_GROUP_StyleFiles);
	m_styleIntGroup = new PropertyGroup(CSE_GROUP_StyleInt);
	m_styleDoubleGroup = new PropertyGroup(CSE_GROUP_StyleDouble);
	m_styleValuesGroup = new PropertyGroup(CSE_GROUP_StyleValues);
	m_intGroup = new PropertyGroup(CSE_GROUP_Int);
	m_doubleGroup = new PropertyGroup(CSE_GROUP_Double);
	m_colorsGroup = new PropertyGroup(CSE_GROUP_StyleSheetColors);
	m_fileGroup = new PropertyGroup(CSE_GROUP_StyleSheetFiles);

	m_propertyGridConfig.setRootGroups({ generalGroup, m_styleFilesGroup, m_styleIntGroup, m_styleDoubleGroup, m_styleValuesGroup, m_intGroup, m_doubleGroup, m_colorsGroup, m_fileGroup });

	m_editor = new ot::TextEditor;
	m_editor->setReadOnly(true);
	m_baseEditor = new ot::TextEditor;
	m_baseEditor->setReadOnly(false);
	m_baseEditor->setNewLineWithSamePrefix(true);
	m_baseEditor->setDuplicateLineShortcutEnabled(true);

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
	QAction* actionBright = _rootMenu->addAction("Set Bright");
	QAction* actionDark = _rootMenu->addAction("Set Dark");
	QAction* actionBlue = _rootMenu->addAction("Set Blue");
	_rootMenu->addSeparator();
	QAction* actionApplyAsCurrent = _rootMenu->addAction("Apply As current");
	QAction* actionExport = _rootMenu->addAction("Export Color Style");
	_rootMenu->addSeparator();
	QAction* actionImportBase = _rootMenu->addAction("Import Style Sheet Base");
	QAction* actionExportBase = _rootMenu->addAction("Export Style Sheet Base");

	// Initialize data
	{
		auto settings = otoolkit::api::getGlobalInterface()->createSettingsInstance();
		m_lastBaseFile = settings.get()->value("ColorStlyeEditor.LastBaseExport", QString()).toString();
		if (m_lastBaseFile.isEmpty()) {
			QByteArray arr = qgetenv("OPENTWIN_DEV_ROOT");
			if (!arr.isEmpty()) {
				m_lastBaseFile = QString::fromStdString(arr.toStdString()) + "/Tools/OToolkit/data/StyleSheetBase.otssb";
			}
			else {
				this->selectStyleSheetBase();
			}
		}
	}

	try {
		this->initializeBrightStyleValues();
		this->initializeStyleSheetBase();
		this->parseStyleSheetBaseFile();
		this->initializePropertyGrid();
		this->slotGenerate();
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
	}
	catch (...) {
		OT_LOG_E("Unknown error");
	}
	
	QShortcut* generateAndApplyTree = new QShortcut(QKeySequence("Ctrl+Shift+B"), m_propertyGrid, this, &ColorStyleEditor::slotGenerateAndApply);
	QShortcut* generateAndApplyEdit = new QShortcut(QKeySequence("Ctrl+Shift+B"), m_baseEditor, this, &ColorStyleEditor::slotGenerateAndApply);
	generateAndApplyTree->setContext(Qt::WidgetWithChildrenShortcut);
	generateAndApplyEdit->setContext(Qt::WidgetWithChildrenShortcut);

	// Connect signals
	this->connect(actionBright, &QAction::triggered, this, &ColorStyleEditor::slotBright);
	this->connect(actionDark, &QAction::triggered, this, &ColorStyleEditor::slotDark);
	this->connect(actionBlue, &QAction::triggered, this, &ColorStyleEditor::slotBlue);
	this->connect(btnGenerate, &QPushButton::clicked, this, &ColorStyleEditor::slotGenerate);
	this->connect(btnApply, &QPushButton::clicked, this, &ColorStyleEditor::slotApplyAsCurrent);
	this->connect(actionApplyAsCurrent, &QAction::triggered, this, &ColorStyleEditor::slotApplyAsCurrent);
	this->connect(actionExport, &QAction::triggered, this, &ColorStyleEditor::slotExport);
	this->connect(actionImportBase, &QAction::triggered, this, &ColorStyleEditor::slotImportBase);
	this->connect(actionExportBase, &QAction::triggered, this, &ColorStyleEditor::slotExportBase);
	this->connect(m_baseEditor, &TextEditor::saveRequested, this, &ColorStyleEditor::slotExportBase);
	this->connect(m_baseEditor, &TextEditor::textChanged, this, &ColorStyleEditor::slotBaseChanged);
	
	return rootSplitter;
}

void ColorStyleEditor::restoreToolSettings(QSettings& _settings) {

}

bool ColorStyleEditor::prepareToolShutdown(QSettings& _settings) {
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

void ColorStyleEditor::slotBright(void) {
	this->initializeBrightStyleValues();
	this->parseStyleSheetBaseFile();
	this->initializePropertyGrid();
}

void ColorStyleEditor::slotDark(void) {
	this->initializeDarkStyleValues();
	this->parseStyleSheetBaseFile();
	this->initializePropertyGrid();
}

void ColorStyleEditor::slotBlue(void) {
	this->initializeBlueStyleValues();
	this->parseStyleSheetBaseFile();
	this->initializePropertyGrid();
}

void ColorStyleEditor::slotGenerate(void) {
	std::string gen;
	if (!this->generateFile(gen)) return;
	m_editor->setPlainText(QString::fromStdString(gen));
	OT_LOG_I("Color Style generated");
}

void ColorStyleEditor::slotApplyAsCurrent(void) {
	std::string tmp = m_editor->toPlainText().toStdString();
	if (tmp.empty()) {
		OT_LOG_W("No data to apply");
		return;
	}

	ot::GlobalColorStyle::instance().addStyle(QByteArray::fromStdString(tmp), true, true);
}

void ColorStyleEditor::slotGenerateAndApply(void) {
	if (m_baseEditor->contentChanged()) {
		if (!this->slotExportBase()) return;
	}
	this->slotGenerate();
	this->slotApplyAsCurrent();
}

void ColorStyleEditor::slotExport(void) {
	std::string tmp = m_editor->toPlainText().toStdString();
	if (tmp.empty()) {
		OT_LOG_W("No data to apply");
		return;
	}

	std::string lastDir = otoolkit::api::getGlobalInterface()->createSettingsInstance().get()->value("LastColorStyleEditorExportFile", QString()).toString().toStdString();
	size_t ix = lastDir.rfind('\\');
	size_t ix2 = lastDir.rfind('/');
	if (ix2 == std::string::npos) ix2 = ix;
	if (ix != std::string::npos) {
		ix = std::max(ix, ix2);
		lastDir = lastDir.substr(0, ix);
	}
	
	QString fileName = QFileDialog::getSaveFileName(nullptr, "Export Color Style File", QString::fromStdString(lastDir), "Color Style Files (*.otcsf)");
	if (fileName.isEmpty()) return;

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		OT_LOG_E("Failed to open file for writing: \"" + fileName.toStdString() + "\"");
		return;
	}

	file.write(QByteArray::fromStdString(tmp));
	file.close();

	otoolkit::api::getGlobalInterface()->createSettingsInstance().get()->setValue("LastColorStyleEditorExportFile", fileName);
	OT_LOG_I("Color style exported \"" + fileName.toStdString() + "\"");
}

void ColorStyleEditor::slotImportBase(void) {
	this->selectStyleSheetBase();
	this->initializeStyleSheetBase();
}

bool ColorStyleEditor::slotExportBase(void) {
	auto settings = otoolkit::api::getGlobalInterface()->createSettingsInstance();
	if (m_lastBaseFile.isEmpty()) {
		m_lastBaseFile = QFileDialog::getSaveFileName(m_baseEditor, "Export Style Sheet Base", m_lastBaseFile, "OpenTwin Style Sheet Base (*.otssb)");
	}
	
	if (m_lastBaseFile.isEmpty()) return false;

	QFile file(m_lastBaseFile);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		OT_LOG_E("Failed to open file for writing. File: \"" + m_lastBaseFile.toStdString() + "\"");
		return false;
	}

	QString txt = m_baseEditor->toPlainText();
	file.write(QByteArray::fromStdString(txt.toStdString()));
	file.close();

	settings.get()->setValue("ColorStlyeEditor.LastBaseExport", m_lastBaseFile);
	m_editorTab->setTabText(0, "Base");
	m_baseEditor->setContentChanged(false);
	OT_LOG_I("StyleSheet Base exported to: \"" + m_lastBaseFile.toStdString() + "\"");

	return true;
}

void ColorStyleEditor::slotBaseChanged(void) {
	m_editorTab->setTabText(0, "Base*");
}

// ###########################################################################################################################################################################################################################################################################################################################

void ColorStyleEditor::selectStyleSheetBase(void) {
	m_lastBaseFile = QFileDialog::getOpenFileName(m_baseEditor, "Import Style Sheet Base", m_lastBaseFile, "OpenTwin Style Sheet Base (*.otssb)");
	if (!m_lastBaseFile.isEmpty()) {
		otoolkit::api::getGlobalInterface()->createSettingsInstance().get()->setValue("ColorStlyeEditor.LastBaseExport", m_lastBaseFile);
	}
}

void ColorStyleEditor::initializeStyleSheetBase(void) {
	if (m_lastBaseFile.isEmpty()) {
		OT_LOG_E("No style sheet base file selected");
		return;
	}
	QFile templateFile(m_lastBaseFile);
	if (!templateFile.exists()) {
		OT_LOG_E("Style sheet base file does not exist");
		return;
	}
	if (!templateFile.open(QIODevice::ReadOnly)) {
		OT_LOG_E("Failed to open style sheet base file for reading");
		return;
	}
	QByteArray sheetBase = templateFile.readAll();
	templateFile.close();

	if (sheetBase.isEmpty()) {
		OT_LOG_E("Style sheet base file is empty");
		return;
	}

	m_baseEditor->setPlainText(QString::fromStdString(sheetBase.toStdString()));
}

void ColorStyleEditor::cleanUpData(void) {
	m_styleFiles.clear();
	m_styleInts.clear();
	m_styleDoubles.clear();
	m_styleValues.clear();
	m_colors.clear();
	m_files.clear();
	m_integer.clear();
	m_double.clear();
}

void ColorStyleEditor::initializeBrightStyleValues(void) {
	using namespace ot;
	// Clean up data
	this->cleanUpData();

	m_nameProp->setValue(OT_COLORSTYLE_NAME_Bright);

	// Initialize default style files
	m_styleFiles.insert_or_assign(OT_COLORSTYLE_FILE_PropertyItemDelete, new PropertyString("/properties/Delete.png"));
	m_styleFiles.insert_or_assign(OT_COLORSTYLE_FILE_PropertyGroupExpanded, new PropertyString("/properties/ArrowGreenDown.png"));
	m_styleFiles.insert_or_assign(OT_COLORSTYLE_FILE_PropertyGroupCollapsed, new PropertyString("/properties/ArrowBlueRight.png"));

	// Initialize default style integers
	m_styleInts.insert_or_assign(OT_COLORSTYLE_INT_SplitterHandleWidth, new PropertyInt(2));

	// Initialize default style values
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsBackground, new PropertyPainter2D(new FillPainter2D(Color::White)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsForeground, new PropertyPainter2D(new FillPainter2D(Color::Black)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsHoverBackground, new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsHoverForeground, new PropertyPainter2D(new FillPainter2D(Color::Black)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsSelectedBackground, new PropertyPainter2D(new FillPainter2D(Color::Lime)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsSelectedForeground, new PropertyPainter2D(new FillPainter2D(Color::Black)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_WindowBackground, new PropertyPainter2D(new FillPainter2D(Color::White)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_WindowForeground, new PropertyPainter2D(new FillPainter2D(Color::Black)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ControlsBorderColor, new PropertyPainter2D(new FillPainter2D(Color::DarkGray)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_TitleBackground, new PropertyPainter2D(new FillPainter2D(Color(215, 215, 215))));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_TitleForeground, new PropertyPainter2D(new FillPainter2D(Color::Black)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_ErrorForeground, new PropertyPainter2D(new FillPainter2D(Color::Red)));

	LinearGradientPainter2D* tehb = new LinearGradientPainter2D;
	tehb->setStart(Point2DD(0.5, 0.));
	tehb->setFinalStop(Point2DD(0.5, 1.));
	tehb->addStop(GradientPainterStop2D(0., Color(Color::Gray)));
	tehb->addStop(GradientPainterStop2D(0.04, Color(Color::Gray)));
	tehb->addStop(GradientPainterStop2D(0.05, Color(Color::White)));
	tehb->addStop(GradientPainterStop2D(0.98, Color(Color::White)));
	tehb->addStop(GradientPainterStop2D(0.99, Color(Color::Gray)));
	tehb->addStop(GradientPainterStop2D(1., Color(Color::Gray)));
	m_styleValues.insert_or_assign(OT_COLORSTYLE_VALUE_TextEditHighlightBorder, new PropertyPainter2D(tehb));

	// Initialize default colors
	m_colors.insert_or_assign(CSE_COLOR_BorderColor, new PropertyPainter2D(new FillPainter2D(Color::Black)));
	m_colors.insert_or_assign(CSE_COLOR_LightBorderColor, new PropertyPainter2D(new FillPainter2D(Color::DarkGray)));
	m_colors.insert_or_assign(CSE_COLOR_BorderDisabledColor, new PropertyPainter2D(new FillPainter2D(Color::Gray)));
	m_colors.insert_or_assign(CSE_COLOR_BorderHoverColor, new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_colors.insert_or_assign(CSE_COLOR_BorderSelectionColor, new PropertyPainter2D(new FillPainter2D(Color::Lime)));
	m_colors.insert_or_assign(CSE_COLOR_HeaderBackground, new PropertyPainter2D(new FillPainter2D(Color::White)));
	m_colors.insert_or_assign(CSE_COLOR_HeaderHoverForeground, new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_colors.insert_or_assign(CSE_COLOR_HeaderSelectionBackground, new PropertyPainter2D(new FillPainter2D(Color::Lime)));
	m_colors.insert_or_assign(CSE_COLOR_InputBackground, new PropertyPainter2D(new FillPainter2D(Color::LightGray)));
	m_colors.insert_or_assign(CSE_COLOR_WidgetAlternateBackground, new PropertyPainter2D(new FillPainter2D(Color::LightGray)));
	m_colors.insert_or_assign(CSE_COLOR_WidgetForeground, new PropertyPainter2D(new FillPainter2D(Color::Black)));
	m_colors.insert_or_assign(CSE_COLOR_WidgetBackground, new PropertyPainter2D(new FillPainter2D(Color::White)));
	m_colors.insert_or_assign(CSE_COLOR_WidgetDisabledBackground, new PropertyPainter2D(new FillPainter2D(Color::Gray)));
	m_colors.insert_or_assign(CSE_COLOR_WidgetDisabledForeground, new PropertyPainter2D(new FillPainter2D(Color::DarkGray)));
	m_colors.insert_or_assign(CSE_COLOR_WidgetHoverBackground, new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_colors.insert_or_assign(CSE_COLOR_WidgetHoverForeground, new PropertyPainter2D(new FillPainter2D(Color::Black)));
	m_colors.insert_or_assign(CSE_COLOR_WidgetSelectionBackground, new PropertyPainter2D(new FillPainter2D(Color::Lime)));
	m_colors.insert_or_assign(CSE_COLOR_WidgetSelectionForeground, new PropertyPainter2D(new FillPainter2D(Color::Black)));
	m_colors.insert_or_assign(CSE_COLOR_WindowBackground, new PropertyPainter2D(new FillPainter2D(Color::White)));
	m_colors.insert_or_assign(CSE_COLOR_TitleBackground, new PropertyPainter2D(new FillPainter2D(Color::LightGray)));
	m_colors.insert_or_assign(CSE_COLOR_TitleForeground, new PropertyPainter2D(new FillPainter2D(Color::Black)));
	m_colors.insert_or_assign(CSE_COLOR_TTBFirstTabBackground, new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_colors.insert_or_assign(CSE_COLOR_TTBFirstTabForeground, new PropertyPainter2D(new FillPainter2D(Color::Black)));
	m_colors.insert_or_assign(CSE_COLOR_ErrorForeground, new PropertyPainter2D(new FillPainter2D(Color::Red)));
	m_colors.insert_or_assign("Test 1", new PropertyPainter2D(new FillPainter2D(Color::Red)));

	m_double.insert_or_assign(CSE_NUMBER_BorderRadius_Big, new PropertyDouble(10.));
	m_double.insert_or_assign(CSE_NUMBER_BorderRadius_Small, new PropertyDouble(6.));
	m_double.insert_or_assign(CSE_NUMBER_OpacityTooltip, new PropertyDouble(10.));

	m_integer.insert_or_assign(CSE_INT_SplitterHandleWidth, new PropertyInt(2));
	m_integer.insert_or_assign(CSE_INT_SplitterBorderRadius, new PropertyInt(2));

	m_files.insert_or_assign(CSE_FILE_LogInBackgroundImage, new PropertyString("/images/OpenTwin.png"));
}

void ColorStyleEditor::initializeDarkStyleValues(void) {
	OT_LOG_E("Not implemented yet");
}

void ColorStyleEditor::initializeBlueStyleValues(void) {
	OT_LOG_E("Not implemented yet");
}

void ColorStyleEditor::parseStyleSheetBaseFile(void) {
	// Create copy of the default sheet base file
	std::string otssb = m_baseEditor->toPlainText().toStdString();

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
			if (k == OT_COLORSTYLE_BASEFILE_MACRO_Color) {
				// Create color entry if doesnt exist
				const auto& it = m_colors.find(n);
				if (it == m_colors.end()) {
					m_colors.insert_or_assign(n, new ot::PropertyPainter2D(new ot::FillPainter2D));
				}
			}
			else if (k == OT_COLORSTYLE_BASEFILE_MACRO_File) {
				// Create file entry
				const auto& it = m_files.find(n);
				if (it == m_files.end()) {
					m_files.insert_or_assign(n, new ot::PropertyString(n));
				}
			}
			else if (k == OT_COLORSTYLE_BASEFILE_MACRO_Int) {
				// Create file entry
				const auto& it = m_integer.find(n);
				if (it == m_integer.end()) {
					m_integer.insert_or_assign(n, new ot::PropertyInt);
				}
			}
			else if (k == OT_COLORSTYLE_BASEFILE_MACRO_Double) {
				// Create file entry
				const auto& it = m_double.find(n);
				if (it == m_double.end()) {
					m_double.insert_or_assign(n, new ot::PropertyDouble);
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
	
	// Style files
	m_styleFilesGroup->clear();
	for (const auto& it : m_styleFiles) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		m_styleFilesGroup->addProperty(it.second);
	}

	// Style integers
	m_styleIntGroup->clear();
	for (const auto& it : m_styleInts) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		m_styleIntGroup->addProperty(it.second);
	}

	// Style doubles
	m_styleDoubleGroup->clear();
	for (const auto& it : m_styleDoubles) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		m_styleDoubleGroup->addProperty(it.second);
	}

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

	// Stylesheet Integers
	m_intGroup->clear();
	for (const auto& it : m_integer) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		m_intGroup->addProperty(it.second);
	}

	// Stylesheet Doubles
	m_doubleGroup->clear();
	for (const auto& it : m_double) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		m_doubleGroup->addProperty(it.second);
	}

	m_propertyGrid->setupGridFromConfig(m_propertyGridConfig);
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

	// Get style files
	JsonDocument styleFilesDoc(rapidjson::kArrayType);
	const PropertyGridGroup* gStyleFiles = m_propertyGrid->findGroup(CSE_GROUP_StyleFiles);
	OTAssertNullptr(gStyleFiles);
	for (const PropertyGridItem* itm : gStyleFiles->childProperties()) {
		const PropertyInputString* inp = dynamic_cast<const PropertyInputString*>(itm->getInput());
		if (!inp) {
			OT_LOG_E("Input cast failed");
			return false;
		}

		JsonObject fObj;
		fObj.AddMember(OT_COLORSTYLE_FILE_VALUE_Name, JsonString(itm->getPropertyData().propertyName(), styleFilesDoc.GetAllocator()), styleFilesDoc.GetAllocator());
		fObj.AddMember(OT_COLORSTYLE_FILE_VALUE_Path, JsonString(inp->getCurrentText().toStdString(), styleFilesDoc.GetAllocator()), styleFilesDoc.GetAllocator());
		styleFilesDoc.PushBack(fObj, styleFilesDoc.GetAllocator());
	}
	std::string cStyleFiles = styleFilesDoc.toJson();

	// Clean up new line if exists
	{
		size_t ix = cStyleFiles.find('\n');
		while (ix != std::string::npos) {
			cStyleFiles.erase(ix);
			ix = cStyleFiles.find('\n');
		}
	}

	// Get style integers
	JsonDocument styleIntDoc(rapidjson::kArrayType);
	const PropertyGridGroup* gStyleInts = m_propertyGrid->findGroup(CSE_GROUP_StyleInt);
	OTAssertNullptr(gStyleInts);
	for (const PropertyGridItem* itm : gStyleInts->childProperties()) {
		const PropertyInputInt* inp = dynamic_cast<const PropertyInputInt*>(itm->getInput());
		if (!inp) {
			OT_LOG_E("Input cast failed");
			return false;
		}

		JsonObject fObj;
		fObj.AddMember(OT_COLORSTYLE_FILE_VALUE_Name, JsonString(itm->getPropertyData().propertyName(), styleIntDoc.GetAllocator()), styleIntDoc.GetAllocator());
		fObj.AddMember(OT_COLORSTYLE_FILE_VALUE_Value, inp->getValue(), styleIntDoc.GetAllocator());
		styleIntDoc.PushBack(fObj, styleIntDoc.GetAllocator());
	}
	std::string cStyleInts = styleIntDoc.toJson();

	// Clean up new line if exists
	{
		size_t ix = cStyleInts.find('\n');
		while (ix != std::string::npos) {
			cStyleInts.erase(ix);
			ix = cStyleInts.find('\n');
		}
	}

	// Get style doubles
	JsonDocument styleDoubleDoc(rapidjson::kArrayType);
	const PropertyGridGroup* gStyleDoubles = m_propertyGrid->findGroup(CSE_GROUP_StyleDouble);
	OTAssertNullptr(gStyleDoubles);
	for (const PropertyGridItem* itm : gStyleDoubles->childProperties()) {
		const PropertyInputDouble* inp = dynamic_cast<const PropertyInputDouble*>(itm->getInput());
		if (!inp) {
			OT_LOG_E("Input cast failed");
			return false;
		}

		JsonObject fObj;
		fObj.AddMember(OT_COLORSTYLE_FILE_VALUE_Name, JsonString(itm->getPropertyData().propertyName(), styleDoubleDoc.GetAllocator()), styleDoubleDoc.GetAllocator());
		fObj.AddMember(OT_COLORSTYLE_FILE_VALUE_Value, inp->getValue(), styleDoubleDoc.GetAllocator());
		styleDoubleDoc.PushBack(fObj, styleDoubleDoc.GetAllocator());
	}
	std::string cStyleDoubles = styleDoubleDoc.toJson();

	// Clean up new line if exists
	{
		size_t ix = cStyleDoubles.find('\n');
		while (ix != std::string::npos) {
			cStyleDoubles.erase(ix);
			ix = cStyleDoubles.find('\n');
		}
	}

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
		if (!inp->getButton()->getPainter()) {
			OT_LOG_E("No painter set");
			return false;
		}

		ColorStyleValue newValue;
		newValue.setName(itm->getPropertyData().propertyName());
		newValue.setPainter(inp->getButton()->getPainter()->createCopy());

		JsonObject pObj;
		newValue.addToJsonObject(pObj, styleValuesDoc.GetAllocator());
		styleValuesDoc.PushBack(pObj, styleValuesDoc.GetAllocator());
	}
	std::string cStyleValue = styleValuesDoc.toJson();

	// Clean up new line if exists
	{
		size_t ix = cStyleValue.find('\n');
		while (ix != std::string::npos) {
			cStyleValue.erase(ix);
			ix = cStyleValue.find('\n');
		}
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
			QString k = "%color:" + QString::fromStdString(inp->data().propertyName()) + "%";
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
			QString k = "%file:" + QString::fromStdString(inp->data().propertyName()) + "%";
			if (replacementMap.count(k)) {
				OT_LOG_W("Duplicate key \"" + k.toStdString() + "\"");
				return false;
			}
			replacementMap.insert_or_assign(k, "%root%" + inp->getCurrentText());
		}
	}
	{
		const PropertyGridGroup* gInt = m_propertyGrid->findGroup(CSE_GROUP_Int);
		OTAssertNullptr(gInt);
		for (const PropertyGridItem* itm : gInt->childProperties()) {
			const PropertyInputInt* inp = dynamic_cast<const PropertyInputInt*>(itm->getInput());
			if (!inp) {
				OT_LOG_E("Input cast failed");
				return false;
			}
			QString k = "%int:" + QString::fromStdString(inp->data().propertyName()) + "%";
			if (replacementMap.count(k)) {
				OT_LOG_W("Duplicate key \"" + k.toStdString() + "\"");
				return false;
			}
			replacementMap.insert_or_assign(k, QString::number(inp->getValue()));
		}
	}
	{
		const PropertyGridGroup* gDouble = m_propertyGrid->findGroup(CSE_GROUP_Double);
		OTAssertNullptr(gDouble);
		for (const PropertyGridItem* itm : gDouble->childProperties()) {
			const PropertyInputDouble* inp = dynamic_cast<const PropertyInputDouble*>(itm->getInput());
			if (!inp) {
				OT_LOG_E("Input cast failed");
				return false;
			}
			QString k = "%number:" + QString::fromStdString(inp->data().propertyName()) + "%";
			if (replacementMap.count(k)) {
				OT_LOG_W("Duplicate key \"" + k.toStdString() + "\"");
				return false;
			}
			replacementMap.insert_or_assign(k, QString::number(inp->getValue()));
		}
	}

	QString base = m_baseEditor->toPlainText();
	for (const auto& it : replacementMap) {
		base.replace(it.first, it.second);
	}

	_result = OT_COLORSTYLE_FILE_KEY_Name + cName + "\n" +
		OT_COLORSTYLE_FILE_KEY_Files + cStyleFiles + "\n" +
		OT_COLORSTYLE_FILE_KEY_Integers + cStyleInts + "\n" +
		OT_COLORSTYLE_FILE_KEY_Doubles + cStyleDoubles + "\n" +
		OT_COLORSTYLE_FILE_KEY_Values + cStyleValue + "\n" +
		OT_COLORSTYLE_FILE_KEY_StyleSheet "\n";
	_result.append(base.toStdString());

	return true;
}