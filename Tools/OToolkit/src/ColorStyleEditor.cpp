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
#include "OTCore/String.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/PropertyStringList.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTWidgets/SpinBox.h"
#include "OTWidgets/Splitter.h"
#include "OTWidgets/TabWidget.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTGui/ColorStyleTypes.h"
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
#define CSE_GROUP_Files "Files"
#define CSE_GROUP_Painter "Painter"
#define CSE_GROUP_Int "Integer Numbers"
#define CSE_GROUP_Double "Decimal Numbers"

#define CSE_Name "Name"

#define CSE_TAB_Base "Base"
#define CSE_TAB_Generated "Generated"

ColorStyleEditor::ColorStyleEditor() {

}

ColorStyleEditor::~ColorStyleEditor() {
	for (const auto& p : m_painters) {
		delete p.second;
	}
	for (const auto& f : m_files) {
		delete f.second;
	}
	for (const auto& i : m_integer) {
		delete i.second;
	}
	for (const auto& d : m_double) {
		delete d.second;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

bool ColorStyleEditor::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	// Create layouts
	ot::Splitter* rootSplitter = new ot::Splitter;
	m_root = this->createCentralWidgetView(rootSplitter, "ColorStyle Editor");
	_content.addView(m_root);

	QWidget* rLayW = new QWidget;
	QVBoxLayout* rLay = new QVBoxLayout(rLayW);

	// Create controls
	m_propertyGrid = new ot::PropertyGrid;

	using namespace ot;

	// General
	PropertyGroup* generalGroup = new PropertyGroup(CSE_GROUP_General);
	m_nameProp = new PropertyString(CSE_Name, std::string());
	generalGroup->addProperty(m_nameProp);
	m_painterGroup = new PropertyGroup(CSE_GROUP_Painter);
	m_intGroup = new PropertyGroup(CSE_GROUP_Int);
	m_doubleGroup = new PropertyGroup(CSE_GROUP_Double);
	m_fileGroup = new PropertyGroup(CSE_GROUP_Files);

	m_propertyGridConfig.setRootGroups({ generalGroup, m_painterGroup, m_intGroup, m_doubleGroup, m_fileGroup });

	m_editor = new ot::TextEditor;
	m_editor->setReadOnly(true);
	m_baseEditor = new ot::TextEditor;
	m_baseEditor->setReadOnly(false);
	m_baseEditor->setNewLineWithSamePrefix(true);
	m_baseEditor->setDuplicateLineShortcutEnabled(true);
	m_baseEditor->setEnableSameTextHighlighting(true);

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
		const std::string& currentGlobalStyleName = ot::GlobalColorStyle::instance().getCurrentStyleName();
		if (currentGlobalStyleName == ot::toString(ot::ColorStyleName::BlueStyle)) {
			this->initializeBlueStyleValues();
		}
		else if (currentGlobalStyleName == ot::toString(ot::ColorStyleName::DarkStyle)) {
			this->initializeDarkStyleValues();
		}
		else {
			if (!currentGlobalStyleName.empty() && ot::GlobalColorStyle::instance().getCurrentStyleName() != ot::toString(ot::ColorStyleName::BrightStyle)) {
				OT_LOG_E("Unknown color style name \"" + currentGlobalStyleName + "\"");
			}
			this->initializeBrightStyleValues();
		}

		this->initializeStyleSheetBase();
		this->parseStyleSheetBaseFile();
		this->initializePropertyGrid();
		this->slotGenerate();
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
	catch (...) {
		OT_LOG_EAS("Unknown error");
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
	
	return true;
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
	if (m_baseEditor->getContentChanged()) {
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
		OT_LOG_EAS("Failed to open file for writing: \"" + fileName.toStdString() + "\"");
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
		OT_LOG_EAS("Failed to open file for writing. File: \"" + m_lastBaseFile.toStdString() + "\"");
		return false;
	}

	QString txt = m_baseEditor->toPlainText();
	file.write(QByteArray::fromStdString(txt.toStdString()));
	file.close();

	settings.get()->setValue("ColorStlyeEditor.LastBaseExport", m_lastBaseFile);
	m_editorTab->setTabText(0, "Base");
	m_baseEditor->setContentSaved();
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
		OT_LOG_EAS("No style sheet base file selected");
		return;
	}
	QFile templateFile(m_lastBaseFile);
	if (!templateFile.exists()) {
		OT_LOG_EAS("Style sheet base file does not exist");
		return;
	}
	if (!templateFile.open(QIODevice::ReadOnly)) {
		OT_LOG_EAS("Failed to open style sheet base file for reading");
		return;
	}
	QByteArray sheetBase = templateFile.readAll();
	templateFile.close();

	if (sheetBase.isEmpty()) {
		OT_LOG_EAS("Style sheet base file is empty");
		return;
	}

	m_baseEditor->setPlainText(QString::fromStdString(sheetBase.toStdString()));
}

void ColorStyleEditor::cleanUpData(void) {
	m_painters.clear();
	m_integer.clear();
	m_double.clear();
	m_files.clear();
}

void ColorStyleEditor::initializeBrightStyleValues(void) {
	using namespace ot;
	// Clean up data
	this->cleanUpData();

	m_nameProp->setValue(ot::toString(ot::ColorStyleName::BrightStyle));

	// Initialize painters
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Transparent), new PropertyPainter2D(new FillPainter2D(ot::Transparent)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Border), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::BorderLight), new PropertyPainter2D(new FillPainter2D(ot::DarkGray)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::BorderDisabled), new PropertyPainter2D(new FillPainter2D(ot::Gray)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::BorderHover), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::BorderSelection), new PropertyPainter2D(new FillPainter2D(ot::Lime)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderBackground), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderHoverBackground), new PropertyPainter2D(new FillPainter2D(Color(White))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderHoverForeground), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderSelectionBackground), new PropertyPainter2D(new FillPainter2D(ot::Lime)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderSelectionForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::InputBackground), new PropertyPainter2D(new FillPainter2D(ot::LightGray)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::InputForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetAlternateBackground), new PropertyPainter2D(new FillPainter2D(ot::LightGray)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetBackground), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetDisabledBackground), new PropertyPainter2D(new FillPainter2D(Color(230, 230, 230))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetDisabledForeground), new PropertyPainter2D(new FillPainter2D(ot::DarkGray)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetHoverBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetHoverForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetSelectionBackground), new PropertyPainter2D(new FillPainter2D(ot::Lime)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetSelectionForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WindowBackground), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WindowForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::DialogBackground), new PropertyPainter2D(new FillPainter2D(Color(240, 240, 240))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::DialogForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TitleBorder), new PropertyPainter2D(new FillPainter2D(ot:: Black)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TitleBackground), new PropertyPainter2D(new FillPainter2D(ot::LightGray)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TitleForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow0), new PropertyPainter2D(new FillPainter2D(Color(230, 57, 70))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow1), new PropertyPainter2D(new FillPainter2D(Color(255, 140, 66))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow2), new PropertyPainter2D(new FillPainter2D(Color(255, 215, 0))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow3), new PropertyPainter2D(new FillPainter2D(Color(80, 200, 120))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow4), new PropertyPainter2D(new FillPainter2D(Color(0, 143, 90))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow5), new PropertyPainter2D(new FillPainter2D(Color(0, 174, 239))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow6), new PropertyPainter2D(new FillPainter2D(Color(70, 130, 180))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow7), new PropertyPainter2D(new FillPainter2D(Color(90, 79, 207))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow8), new PropertyPainter2D(new FillPainter2D(Color(155, 48, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow9), new PropertyPainter2D(new FillPainter2D(Color(255, 20, 147))));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::ToolBarFirstTabBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::ToolBarFirstTabForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemBorder), new PropertyPainter2D(new FillPainter2D(Color(50, 50, 50))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemConnection), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemBackground), new PropertyPainter2D(new FillPainter2D(Color(230, 230, 230))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemSelectionBorder), new PropertyPainter2D(new FillPainter2D(ot::Lime)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemHoverBorder), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemConnectableBackground), new PropertyPainter2D(new FillPainter2D(Color(200, 200, 200))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemLineColor), new PropertyPainter2D(new FillPainter2D(ot::Black)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextHighlight), new PropertyPainter2D(new FillPainter2D(Color(78, 201, 176))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextLightHighlight), new PropertyPainter2D(new FillPainter2D(Color(220, 220, 170))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextComment), new PropertyPainter2D(new FillPainter2D(Color(87, 166, 74))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextWarning), new PropertyPainter2D(new FillPainter2D(Color(240, 96, 0))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextError), new PropertyPainter2D(new FillPainter2D(Color(235, 0, 0))));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonClass), new PropertyPainter2D(new FillPainter2D(Color(78, 201, 176))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonComment), new PropertyPainter2D(new FillPainter2D(Color(87, 166, 74))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonFunction), new PropertyPainter2D(new FillPainter2D(Color(220, 220, 170))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonKeyword), new PropertyPainter2D(new FillPainter2D(Color(86, 156, 214))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonString), new PropertyPainter2D(new FillPainter2D(Color(214, 157, 133))));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PlotCurve), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PlotCurveDimmed), new PropertyPainter2D(new FillPainter2D(100, 100, 100, 100)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PlotCurveHighlight), new PropertyPainter2D(new FillPainter2D(255, 255, 128)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::ErrorForeground), new PropertyPainter2D(new FillPainter2D(ot::Red)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TextEditorHighlightBackground), new PropertyPainter2D(new FillPainter2D(ot::LightGray)));

	LinearGradientPainter2D* tehb = new LinearGradientPainter2D;
	tehb->setStart(Point2DD(0.5, 0.));
	tehb->setFinalStop(Point2DD(0.5, 1.));
	tehb->addStop(GradientPainterStop2D(0., Color(ot::Gray)));
	tehb->addStop(GradientPainterStop2D(0.04, Color(ot::Gray)));
	tehb->addStop(GradientPainterStop2D(0.05, Color(ot::White)));
	tehb->addStop(GradientPainterStop2D(0.98, Color(ot::White)));
	tehb->addStop(GradientPainterStop2D(0.99, Color(ot::Gray)));
	tehb->addStop(GradientPainterStop2D(1., Color(ot::Gray)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TextEditorLineBorder), new PropertyPainter2D(tehb));

	// Double
	m_double.insert_or_assign(toString(ColorStyleDoubleEntry::BorderWidth), new PropertyDouble(1.));
	m_double.insert_or_assign(toString(ColorStyleDoubleEntry::BorderRadiusBig), new PropertyDouble(4.));
	m_double.insert_or_assign(toString(ColorStyleDoubleEntry::BorderRadiusSmall), new PropertyDouble(4.));
	m_double.insert_or_assign(toString(ColorStyleDoubleEntry::ToolTipOpacity), new PropertyDouble(10.));

	// Int
	m_integer.insert_or_assign(toString(ColorStyleIntegerEntry::SplitterBorderRadius), new PropertyInt(2));
	m_integer.insert_or_assign(toString(ColorStyleIntegerEntry::SplitterHandleWidth), new PropertyInt(2));

	// File
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TransparentIcon), new PropertyString("/icons/transparent.png"));
	
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowUpIcon), new PropertyString("/icons/arrow_up.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowUpDisabledIcon), new PropertyString("/icons/arrow_up_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowUpFocusIcon), new PropertyString("/icons/arrow_up_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowUpPressed), new PropertyString("/icons/arrow_up_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowDownIcon), new PropertyString("/icons/arrow_down.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowDownDisabledIcon), new PropertyString("/icons/arrow_down_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowDownFocusIcon), new PropertyString("/icons/arrow_down_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowDownPressed), new PropertyString("/icons/arrow_down_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowLeftIcon), new PropertyString("/icons/arrow_left.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowLeftDisabledIcon), new PropertyString("/icons/arrow_left_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowLeftFocusIcon), new PropertyString("/icons/arrow_left_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowLeftPressed), new PropertyString("/icons/arrow_left_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowRightIcon), new PropertyString("/icons/arrow_right.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowRightDisabledIcon), new PropertyString("/icons/arrow_right_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowRightFocusIcon), new PropertyString("/icons/arrow_right_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowRightPressed), new PropertyString("/icons/arrow_right_pressed.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchClosedIcon), new PropertyString("/icons/branch_closed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchClosedFocusIcon), new PropertyString("/icons/branch_closed_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchEndIcon), new PropertyString("/icons/branch_end.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchLineIcon), new PropertyString("/icons/branch_line.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchMoreIcon), new PropertyString("/icons/branch_more.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchOpenIcon), new PropertyString("/icons/branch_open.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchOpenFocusIcon), new PropertyString("/icons/branch_open_focus.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxCheckedIcon), new PropertyString("/icons/checkbox_checked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxCheckedDisabledIcon), new PropertyString("/icons/checkbox_checked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxCheckedFocusIcon), new PropertyString("/icons/checkbox_checked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxCheckedPressedIcon), new PropertyString("/icons/checkbox_checked_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxIndeterminateIcon), new PropertyString("/icons/checkbox_indeterminate.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxIndeterminateDisabledIcon), new PropertyString("/icons/checkbox_indeterminate_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxIndeterminateFocusIcon), new PropertyString("/icons/checkbox_indeterminate_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxIndeterminatePressedIcon), new PropertyString("/icons/checkbox_indeterminate_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxUncheckedIcon), new PropertyString("/icons/checkbox_unchecked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxUncheckedDisabledIcon), new PropertyString("/icons/checkbox_unchecked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxUncheckedFocusIcon), new PropertyString("/icons/checkbox_unchecked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxUncheckedPressedIcon), new PropertyString("/icons/checkbox_unchecked_pressed.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxCheckedIcon), new PropertyString("/icons/log_in_checkbox_checked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxCheckedDisabledIcon), new PropertyString("/icons/log_in_checkbox_checked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxCheckedFocusIcon), new PropertyString("/icons/log_in_checkbox_checked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxCheckedPressedIcon), new PropertyString("/icons/log_in_checkbox_checked_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxIndeterminateIcon), new PropertyString("/icons/log_in_checkbox_indeterminate.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxIndeterminateDisabledIcon), new PropertyString("/icons/log_in_checkbox_indeterminate_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxIndeterminateFocusIcon), new PropertyString("/icons/log_in_checkbox_indeterminate_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxIndeterminatePressedIcon), new PropertyString("/icons/log_in_checkbox_indeterminate_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxUncheckedIcon), new PropertyString("/icons/log_in_checkbox_unchecked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxUncheckedDisabledIcon), new PropertyString("/icons/log_in_checkbox_unchecked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxUncheckedFocusIcon), new PropertyString("/icons/log_in_checkbox_unchecked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxUncheckedPressedIcon), new PropertyString("/icons/log_in_checkbox_unchecked_pressed.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonCheckedIcon), new PropertyString("/icons/radio_checked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonCheckedDisabledIcon), new PropertyString("/icons/radio_checked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonCheckedFocusIcon), new PropertyString("/icons/radio_checked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonUncheckedIcon), new PropertyString("/icons/radio_unchecked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonUncheckedDisabledIcon), new PropertyString("/icons/radio_unchecked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonUncheckedFocusIcon), new PropertyString("/icons/radio_unchecked_focus.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::ToolBarMoveHorizontalIcon), new PropertyString("/icons/toolbar_move_horizontal.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ToolBarMoveVerticalIcon), new PropertyString("/icons/toolbar_move_vertical.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ToolBarSeparatorHorizontalIcon), new PropertyString("/icons/toolbar_separator_horizontal.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ToolBarSeparatorVerticalIcon), new PropertyString("/icons/toolbar_separator_vertical.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowCloseIcon), new PropertyString("/icons/window_close.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowCloseDisabledIcon), new PropertyString("/icons/window_close_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowCloseFocusIcon), new PropertyString("/icons/window_close_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowClosePressedIcon), new PropertyString("/icons/window_close_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowPinIcon), new PropertyString("/icons/window_pin.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnpinIcon), new PropertyString("/icons/window_unpin.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowPinDisabledIcon), new PropertyString("/icons/window_pin_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnpinDisabledIcon), new PropertyString("/icons/window_unpin_disbaled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowPinFocusIcon), new PropertyString("/icons/window_pin_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnpinFocusIcon), new PropertyString("/icons/window_unpin_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowPinPressedIcon), new PropertyString("/icons/window_pin_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnpinPressedIcon), new PropertyString("/icons/window_unpin_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowLockIcon), new PropertyString("/icons/window_lock.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnlockIcon), new PropertyString("/icons/window_unlock.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowLockDisabledIcon), new PropertyString("/icons/window_lock_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnlockDisabledIcon), new PropertyString("/icons/window_unlock_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowLockFocusIcon), new PropertyString("/icons/window_lock_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnlockFocusIcon), new PropertyString("/icons/window_unlock_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowLockPressedIcon), new PropertyString("/icons/window_lock_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnlockPressedIcon), new PropertyString("/icons/window_unlock_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowGripIcon), new PropertyString("/icons/window_grip.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUndockIcon), new PropertyString("/icons/window_undock.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUndockDisabledIcon), new PropertyString("/icons/window_undock_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUndockFocusIcon), new PropertyString("/icons/window_undock_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUndockPressedIcon), new PropertyString("/icons/window_undock_pressed.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInBackgroundImage), new PropertyString("/images/OpenTwin.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::PropertyItemDeleteIcon), new PropertyString("/properties/Delete.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::PropertyGroupExpandedIcon), new PropertyString("/properties/ArrowGreenDown.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::PropertyGroupCollapsedIcon), new PropertyString("/properties/ArrowBlueRight.png"));
	
}

void ColorStyleEditor::initializeDarkStyleValues(void) {
	using namespace ot;
	// Clean up data
	this->cleanUpData();

	m_nameProp->setValue(ot::toString(ot::ColorStyleName::DarkStyle));

	// Initialize painters
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Transparent), new PropertyPainter2D(new FillPainter2D(ot::Transparent)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Border), new PropertyPainter2D(new FillPainter2D(Color(70, 70, 70))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::BorderLight), new PropertyPainter2D(new FillPainter2D(Color(50, 50, 50))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::BorderDisabled), new PropertyPainter2D(new FillPainter2D(Color(30, 30, 30))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::BorderHover), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::BorderSelection), new PropertyPainter2D(new FillPainter2D(ot::Lime)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderBackground), new PropertyPainter2D(new FillPainter2D(Color(30, 30, 30))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderForeground), new PropertyPainter2D(new FillPainter2D(Color(White))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderHoverBackground), new PropertyPainter2D(new FillPainter2D(Color(30, 30, 30))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderHoverForeground), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderSelectionBackground), new PropertyPainter2D(new FillPainter2D(ot::Lime)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderSelectionForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::InputBackground), new PropertyPainter2D(new FillPainter2D(Color(50, 50, 50))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::InputForeground), new PropertyPainter2D(new FillPainter2D(Color(White))));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetAlternateBackground), new PropertyPainter2D(new FillPainter2D(Color(40, 40, 40))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetForeground), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetBackground), new PropertyPainter2D(new FillPainter2D(Color(30, 30, 30))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetDisabledBackground), new PropertyPainter2D(new FillPainter2D(Color(20, 20, 20))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetDisabledForeground), new PropertyPainter2D(new FillPainter2D(Color(200, 200, 200))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetHoverBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetHoverForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetSelectionBackground), new PropertyPainter2D(new FillPainter2D(ot::Lime)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetSelectionForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WindowBackground), new PropertyPainter2D(new FillPainter2D(Color(30, 30, 30))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WindowForeground), new PropertyPainter2D(new FillPainter2D(ot::White)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::DialogBackground), new PropertyPainter2D(new FillPainter2D(Color(50, 50, 50))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::DialogForeground), new PropertyPainter2D(new FillPainter2D(ot::White)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TitleBorder), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TitleBackground), new PropertyPainter2D(new FillPainter2D(Color(50, 50, 50))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TitleForeground), new PropertyPainter2D(new FillPainter2D(ot::White)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow0), new PropertyPainter2D(new FillPainter2D(Color(230, 57, 70))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow1), new PropertyPainter2D(new FillPainter2D(Color(255, 140, 66))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow2), new PropertyPainter2D(new FillPainter2D(Color(255, 215, 0))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow3), new PropertyPainter2D(new FillPainter2D(Color(80, 200, 120))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow4), new PropertyPainter2D(new FillPainter2D(Color(0, 143, 90))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow5), new PropertyPainter2D(new FillPainter2D(Color(0, 174, 239))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow6), new PropertyPainter2D(new FillPainter2D(Color(70, 130, 180))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow7), new PropertyPainter2D(new FillPainter2D(Color(90, 79, 207))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow8), new PropertyPainter2D(new FillPainter2D(Color(155, 48, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow9), new PropertyPainter2D(new FillPainter2D(Color(255, 20, 147))));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::ToolBarFirstTabBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::ToolBarFirstTabForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemBorder), new PropertyPainter2D(new FillPainter2D(Color(100, 100, 100))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemConnection), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemBackground), new PropertyPainter2D(new FillPainter2D(Color(50, 50, 50))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemForeground), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemSelectionBorder), new PropertyPainter2D(new FillPainter2D(ot::Lime)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemHoverBorder), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemConnectableBackground), new PropertyPainter2D(new FillPainter2D(Color(100, 100, 100))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemLineColor), new PropertyPainter2D(new FillPainter2D(ot::White)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextHighlight), new PropertyPainter2D(new FillPainter2D(Color(78, 201, 176))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextLightHighlight), new PropertyPainter2D(new FillPainter2D(Color(220, 220, 170))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextComment), new PropertyPainter2D(new FillPainter2D(Color(87, 166, 74))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextWarning), new PropertyPainter2D(new FillPainter2D(Color(240, 96, 0))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextError), new PropertyPainter2D(new FillPainter2D(Color(235, 0, 0))));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonClass), new PropertyPainter2D(new FillPainter2D(Color(78, 201, 176))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonComment), new PropertyPainter2D(new FillPainter2D(Color(87, 166, 74))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonFunction), new PropertyPainter2D(new FillPainter2D(Color(220, 220, 170))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonKeyword), new PropertyPainter2D(new FillPainter2D(Color(86, 156, 214))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonString), new PropertyPainter2D(new FillPainter2D(Color(214, 157, 133))));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PlotCurve), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PlotCurveDimmed), new PropertyPainter2D(new FillPainter2D(100, 100, 100, 100)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PlotCurveHighlight), new PropertyPainter2D(new FillPainter2D(255, 255, 128)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::ErrorForeground), new PropertyPainter2D(new FillPainter2D(ot::Red)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TextEditorHighlightBackground), new PropertyPainter2D(new FillPainter2D(Color(70, 100, 70))));

	LinearGradientPainter2D* tehb = new LinearGradientPainter2D;
	tehb->setStart(Point2DD(0.5, 0.));
	tehb->setFinalStop(Point2DD(0.5, 1.));
	tehb->addStop(GradientPainterStop2D(0., Color(ot::Gray)));
	tehb->addStop(GradientPainterStop2D(0.04, Color(ot::Gray)));
	tehb->addStop(GradientPainterStop2D(0.05, Color(Color(30, 30, 30))));
	tehb->addStop(GradientPainterStop2D(0.98, Color(Color(30, 30, 30))));
	tehb->addStop(GradientPainterStop2D(0.99, Color(ot::Gray)));
	tehb->addStop(GradientPainterStop2D(1., Color(ot::Gray)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TextEditorLineBorder), new PropertyPainter2D(tehb));

	// Double
	m_double.insert_or_assign(toString(ColorStyleDoubleEntry::BorderWidth), new PropertyDouble(1.));
	m_double.insert_or_assign(toString(ColorStyleDoubleEntry::BorderRadiusBig), new PropertyDouble(4.));
	m_double.insert_or_assign(toString(ColorStyleDoubleEntry::BorderRadiusSmall), new PropertyDouble(4.));
	m_double.insert_or_assign(toString(ColorStyleDoubleEntry::ToolTipOpacity), new PropertyDouble(10.));

	// Int
	m_integer.insert_or_assign(toString(ColorStyleIntegerEntry::SplitterBorderRadius), new PropertyInt(2));
	m_integer.insert_or_assign(toString(ColorStyleIntegerEntry::SplitterHandleWidth), new PropertyInt(2));

	// File
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TransparentIcon), new PropertyString("/icons/transparent.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowUpIcon), new PropertyString("/icons/arrow_up.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowUpDisabledIcon), new PropertyString("/icons/arrow_up_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowUpFocusIcon), new PropertyString("/icons/arrow_up_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowUpPressed), new PropertyString("/icons/arrow_up_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowDownIcon), new PropertyString("/icons/arrow_down.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowDownDisabledIcon), new PropertyString("/icons/arrow_down_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowDownFocusIcon), new PropertyString("/icons/arrow_down_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowDownPressed), new PropertyString("/icons/arrow_down_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowLeftIcon), new PropertyString("/icons/arrow_left.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowLeftDisabledIcon), new PropertyString("/icons/arrow_left_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowLeftFocusIcon), new PropertyString("/icons/arrow_left_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowLeftPressed), new PropertyString("/icons/arrow_left_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowRightIcon), new PropertyString("/icons/arrow_right.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowRightDisabledIcon), new PropertyString("/icons/arrow_right_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowRightFocusIcon), new PropertyString("/icons/arrow_right_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowRightPressed), new PropertyString("/icons/arrow_right_pressed.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchClosedIcon), new PropertyString("/icons/branch_closed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchClosedFocusIcon), new PropertyString("/icons/branch_closed_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchEndIcon), new PropertyString("/icons/branch_end.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchLineIcon), new PropertyString("/icons/branch_line.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchMoreIcon), new PropertyString("/icons/branch_more.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchOpenIcon), new PropertyString("/icons/branch_open.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchOpenFocusIcon), new PropertyString("/icons/branch_open_focus.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxCheckedIcon), new PropertyString("/icons/checkbox_checked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxCheckedDisabledIcon), new PropertyString("/icons/checkbox_checked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxCheckedFocusIcon), new PropertyString("/icons/checkbox_checked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxCheckedPressedIcon), new PropertyString("/icons/checkbox_checked_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxIndeterminateIcon), new PropertyString("/icons/checkbox_indeterminate.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxIndeterminateDisabledIcon), new PropertyString("/icons/checkbox_indeterminate_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxIndeterminateFocusIcon), new PropertyString("/icons/checkbox_indeterminate_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxIndeterminatePressedIcon), new PropertyString("/icons/checkbox_indeterminate_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxUncheckedIcon), new PropertyString("/icons/checkbox_unchecked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxUncheckedDisabledIcon), new PropertyString("/icons/checkbox_unchecked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxUncheckedFocusIcon), new PropertyString("/icons/checkbox_unchecked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxUncheckedPressedIcon), new PropertyString("/icons/checkbox_unchecked_pressed.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxCheckedIcon), new PropertyString("/icons/log_in_checkbox_checked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxCheckedDisabledIcon), new PropertyString("/icons/log_in_checkbox_checked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxCheckedFocusIcon), new PropertyString("/icons/log_in_checkbox_checked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxCheckedPressedIcon), new PropertyString("/icons/log_in_checkbox_checked_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxIndeterminateIcon), new PropertyString("/icons/log_in_checkbox_indeterminate.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxIndeterminateDisabledIcon), new PropertyString("/icons/log_in_checkbox_indeterminate_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxIndeterminateFocusIcon), new PropertyString("/icons/log_in_checkbox_indeterminate_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxIndeterminatePressedIcon), new PropertyString("/icons/log_in_checkbox_indeterminate_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxUncheckedIcon), new PropertyString("/icons/log_in_checkbox_unchecked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxUncheckedDisabledIcon), new PropertyString("/icons/log_in_checkbox_unchecked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxUncheckedFocusIcon), new PropertyString("/icons/log_in_checkbox_unchecked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxUncheckedPressedIcon), new PropertyString("/icons/log_in_checkbox_unchecked_pressed.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonCheckedIcon), new PropertyString("/icons/radio_checked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonCheckedDisabledIcon), new PropertyString("/icons/radio_checked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonCheckedFocusIcon), new PropertyString("/icons/radio_checked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonUncheckedIcon), new PropertyString("/icons/radio_unchecked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonUncheckedDisabledIcon), new PropertyString("/icons/radio_unchecked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonUncheckedFocusIcon), new PropertyString("/icons/radio_unchecked_focus.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::ToolBarMoveHorizontalIcon), new PropertyString("/icons/toolbar_move_horizontal.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ToolBarMoveVerticalIcon), new PropertyString("/icons/toolbar_move_vertical.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ToolBarSeparatorHorizontalIcon), new PropertyString("/icons/toolbar_separator_horizontal.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ToolBarSeparatorVerticalIcon), new PropertyString("/icons/toolbar_separator_vertical.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowCloseIcon), new PropertyString("/icons/window_close.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowCloseDisabledIcon), new PropertyString("/icons/window_close_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowCloseFocusIcon), new PropertyString("/icons/window_close_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowClosePressedIcon), new PropertyString("/icons/window_close_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowPinIcon), new PropertyString("/icons/window_pin.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnpinIcon), new PropertyString("/icons/window_unpin.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowPinDisabledIcon), new PropertyString("/icons/window_pin_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnpinDisabledIcon), new PropertyString("/icons/window_unpin_disbaled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowPinFocusIcon), new PropertyString("/icons/window_pin_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnpinFocusIcon), new PropertyString("/icons/window_unpin_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowPinPressedIcon), new PropertyString("/icons/window_pin_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnpinPressedIcon), new PropertyString("/icons/window_unpin_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowLockIcon), new PropertyString("/icons/window_lock.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnlockIcon), new PropertyString("/icons/window_unlock.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowLockDisabledIcon), new PropertyString("/icons/window_lock_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnlockDisabledIcon), new PropertyString("/icons/window_unlock_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowLockFocusIcon), new PropertyString("/icons/window_lock_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnlockFocusIcon), new PropertyString("/icons/window_unlock_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowLockPressedIcon), new PropertyString("/icons/window_lock_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnlockPressedIcon), new PropertyString("/icons/window_unlock_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowGripIcon), new PropertyString("/icons/window_grip.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUndockIcon), new PropertyString("/icons/window_undock.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUndockDisabledIcon), new PropertyString("/icons/window_undock_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUndockFocusIcon), new PropertyString("/icons/window_undock_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUndockPressedIcon), new PropertyString("/icons/window_undock_pressed.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInBackgroundImage), new PropertyString("/images/OpenTwin.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::PropertyItemDeleteIcon), new PropertyString("/properties/Delete.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::PropertyGroupExpandedIcon), new PropertyString("/properties/ArrowGreenDown.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::PropertyGroupCollapsedIcon), new PropertyString("/properties/ArrowBlueRight.png"));
}

void ColorStyleEditor::initializeBlueStyleValues(void) {
	using namespace ot;
	// Clean up data
	this->cleanUpData();

	m_nameProp->setValue(ot::toString(ot::ColorStyleName::BlueStyle));

	// Initialize default style integers

	// Initialize painters
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Transparent), new PropertyPainter2D(new FillPainter2D(ot::Transparent)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Border), new PropertyPainter2D(new FillPainter2D(Color(0, 100, 180))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::BorderLight), new PropertyPainter2D(new FillPainter2D(ot::Navy)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::BorderDisabled), new PropertyPainter2D(new FillPainter2D(ot::Gray)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::BorderHover), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::BorderSelection), new PropertyPainter2D(new FillPainter2D(Color(210, 90, 10))));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 64, 128))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderForeground), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderHoverBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 64, 128))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderHoverForeground), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderSelectionBackground), new PropertyPainter2D(new FillPainter2D(Color(210, 90, 10))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::HeaderSelectionForeground), new PropertyPainter2D(new FillPainter2D(Color(White))));
	
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::InputBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 90, 156))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::InputForeground), new PropertyPainter2D(new FillPainter2D(Color(White))));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetAlternateBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 80, 140))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetForeground), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 64, 128))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetDisabledBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 44, 108))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetDisabledForeground), new PropertyPainter2D(new FillPainter2D(ot::Gray)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetHoverBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetHoverForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetSelectionBackground), new PropertyPainter2D(new FillPainter2D(Color(210, 90, 10))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WidgetSelectionForeground), new PropertyPainter2D(new FillPainter2D(ot::White)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WindowBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 64, 128))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::WindowForeground), new PropertyPainter2D(new FillPainter2D(ot::White)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::DialogBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 90, 156))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::DialogForeground), new PropertyPainter2D(new FillPainter2D(ot::White)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TitleBorder), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TitleBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 90, 156))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TitleForeground), new PropertyPainter2D(new FillPainter2D(ot::White)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow0), new PropertyPainter2D(new FillPainter2D(Color(230, 57, 70))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow1), new PropertyPainter2D(new FillPainter2D(Color(255, 140, 66))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow2), new PropertyPainter2D(new FillPainter2D(Color(255, 215, 0))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow3), new PropertyPainter2D(new FillPainter2D(Color(80, 200, 120))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow4), new PropertyPainter2D(new FillPainter2D(Color(0, 143, 90))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow5), new PropertyPainter2D(new FillPainter2D(Color(0, 174, 239))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow6), new PropertyPainter2D(new FillPainter2D(Color(70, 130, 180))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow7), new PropertyPainter2D(new FillPainter2D(Color(90, 79, 207))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow8), new PropertyPainter2D(new FillPainter2D(Color(155, 48, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::Rainbow9), new PropertyPainter2D(new FillPainter2D(Color(255, 20, 147))));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::ToolBarFirstTabBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 215, 255))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::ToolBarFirstTabForeground), new PropertyPainter2D(new FillPainter2D(ot::Black)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemBorder), new PropertyPainter2D(new FillPainter2D(Color(200, 200, 200))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemConnection), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 80, 140))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemForeground), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemSelectionBorder), new PropertyPainter2D(new FillPainter2D(Color(210, 90, 10))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemHoverBorder), new PropertyPainter2D(new FillPainter2D(Color(210, 90, 10))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemConnectableBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 80, 140))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::GraphicsItemLineColor), new PropertyPainter2D(new FillPainter2D(ot::White)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextHighlight), new PropertyPainter2D(new FillPainter2D(Color(78, 201, 176))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextLightHighlight), new PropertyPainter2D(new FillPainter2D(Color(220, 220, 170))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextComment), new PropertyPainter2D(new FillPainter2D(Color(87, 166, 74))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextWarning), new PropertyPainter2D(new FillPainter2D(Color(240, 96, 0))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::StyledTextError), new PropertyPainter2D(new FillPainter2D(Color(235, 0, 0))));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonClass), new PropertyPainter2D(new FillPainter2D(Color(78, 201, 176))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonComment), new PropertyPainter2D(new FillPainter2D(Color(87, 166, 74))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonFunction), new PropertyPainter2D(new FillPainter2D(Color(220, 220, 170))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonKeyword), new PropertyPainter2D(new FillPainter2D(Color(86, 156, 214))));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PythonString), new PropertyPainter2D(new FillPainter2D(Color(214, 157, 133))));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PlotCurve), new PropertyPainter2D(new FillPainter2D(ot::White)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PlotCurveDimmed), new PropertyPainter2D(new FillPainter2D(100, 100, 100, 100)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::PlotCurveHighlight), new PropertyPainter2D(new FillPainter2D(255, 255, 128)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::ErrorForeground), new PropertyPainter2D(new FillPainter2D(ot::Red)));

	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TextEditorHighlightBackground), new PropertyPainter2D(new FillPainter2D(Color(0, 100, 180))));

	LinearGradientPainter2D* tehb = new LinearGradientPainter2D;
	tehb->setStart(Point2DD(0.5, 0.));
	tehb->setFinalStop(Point2DD(0.5, 1.));
	tehb->addStop(GradientPainterStop2D(0., Color(ot::Gray)));
	tehb->addStop(GradientPainterStop2D(0.04, Color(ot::Gray)));
	tehb->addStop(GradientPainterStop2D(0.05, Color(0, 64, 128)));
	tehb->addStop(GradientPainterStop2D(0.98, Color(0, 64, 128)));
	tehb->addStop(GradientPainterStop2D(0.99, Color(ot::Gray)));
	tehb->addStop(GradientPainterStop2D(1., Color(ot::Gray)));
	m_painters.insert_or_assign(toString(ColorStyleValueEntry::TextEditorLineBorder), new PropertyPainter2D(tehb));

	// Double
	m_double.insert_or_assign(toString(ColorStyleDoubleEntry::BorderWidth), new PropertyDouble(1.));
	m_double.insert_or_assign(toString(ColorStyleDoubleEntry::BorderRadiusBig), new PropertyDouble(4.));
	m_double.insert_or_assign(toString(ColorStyleDoubleEntry::BorderRadiusSmall), new PropertyDouble(4.));
	m_double.insert_or_assign(toString(ColorStyleDoubleEntry::ToolTipOpacity), new PropertyDouble(10.));

	// Integer
	m_integer.insert_or_assign(toString(ColorStyleIntegerEntry::SplitterHandleWidth), new PropertyInt(2));
	m_integer.insert_or_assign(toString(ColorStyleIntegerEntry::SplitterBorderRadius), new PropertyInt(2));

	// File
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TransparentIcon), new PropertyString("/icons/transparent.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowUpIcon), new PropertyString("/icons/arrow_up.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowUpDisabledIcon), new PropertyString("/icons/arrow_up_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowUpFocusIcon), new PropertyString("/icons/arrow_up_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowUpPressed), new PropertyString("/icons/arrow_up_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowDownIcon), new PropertyString("/icons/arrow_down.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowDownDisabledIcon), new PropertyString("/icons/arrow_down_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowDownFocusIcon), new PropertyString("/icons/arrow_down_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowDownPressed), new PropertyString("/icons/arrow_down_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowLeftIcon), new PropertyString("/icons/arrow_left.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowLeftDisabledIcon), new PropertyString("/icons/arrow_left_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowLeftFocusIcon), new PropertyString("/icons/arrow_left_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowLeftPressed), new PropertyString("/icons/arrow_left_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowRightIcon), new PropertyString("/icons/arrow_right.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowRightDisabledIcon), new PropertyString("/icons/arrow_right_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowRightFocusIcon), new PropertyString("/icons/arrow_right_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ArrowRightPressed), new PropertyString("/icons/arrow_right_pressed.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchClosedIcon), new PropertyString("/icons/branch_closed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchClosedFocusIcon), new PropertyString("/icons/branch_closed_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchEndIcon), new PropertyString("/icons/branch_end.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchLineIcon), new PropertyString("/icons/branch_line.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchMoreIcon), new PropertyString("/icons/branch_more.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchOpenIcon), new PropertyString("/icons/branch_open.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::TreeBranchOpenFocusIcon), new PropertyString("/icons/branch_open_focus.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxCheckedIcon), new PropertyString("/icons/checkbox_checked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxCheckedDisabledIcon), new PropertyString("/icons/checkbox_checked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxCheckedFocusIcon), new PropertyString("/icons/checkbox_checked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxCheckedPressedIcon), new PropertyString("/icons/checkbox_checked_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxIndeterminateIcon), new PropertyString("/icons/checkbox_indeterminate.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxIndeterminateDisabledIcon), new PropertyString("/icons/checkbox_indeterminate_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxIndeterminateFocusIcon), new PropertyString("/icons/checkbox_indeterminate_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxIndeterminatePressedIcon), new PropertyString("/icons/checkbox_indeterminate_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxUncheckedIcon), new PropertyString("/icons/checkbox_unchecked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxUncheckedDisabledIcon), new PropertyString("/icons/checkbox_unchecked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxUncheckedFocusIcon), new PropertyString("/icons/checkbox_unchecked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::CheckBoxUncheckedPressedIcon), new PropertyString("/icons/checkbox_unchecked_pressed.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxCheckedIcon), new PropertyString("/icons/log_in_checkbox_checked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxCheckedDisabledIcon), new PropertyString("/icons/log_in_checkbox_checked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxCheckedFocusIcon), new PropertyString("/icons/log_in_checkbox_checked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxCheckedPressedIcon), new PropertyString("/icons/log_in_checkbox_checked_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxIndeterminateIcon), new PropertyString("/icons/log_in_checkbox_indeterminate.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxIndeterminateDisabledIcon), new PropertyString("/icons/log_in_checkbox_indeterminate_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxIndeterminateFocusIcon), new PropertyString("/icons/log_in_checkbox_indeterminate_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxIndeterminatePressedIcon), new PropertyString("/icons/log_in_checkbox_indeterminate_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxUncheckedIcon), new PropertyString("/icons/log_in_checkbox_unchecked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxUncheckedDisabledIcon), new PropertyString("/icons/log_in_checkbox_unchecked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxUncheckedFocusIcon), new PropertyString("/icons/log_in_checkbox_unchecked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInCheckBoxUncheckedPressedIcon), new PropertyString("/icons/log_in_checkbox_unchecked_pressed.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonCheckedIcon), new PropertyString("/icons/radio_checked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonCheckedDisabledIcon), new PropertyString("/icons/radio_checked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonCheckedFocusIcon), new PropertyString("/icons/radio_checked_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonUncheckedIcon), new PropertyString("/icons/radio_unchecked.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonUncheckedDisabledIcon), new PropertyString("/icons/radio_unchecked_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::RadioButtonUncheckedFocusIcon), new PropertyString("/icons/radio_unchecked_focus.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::ToolBarMoveHorizontalIcon), new PropertyString("/icons/toolbar_move_horizontal.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ToolBarMoveVerticalIcon), new PropertyString("/icons/toolbar_move_vertical.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ToolBarSeparatorHorizontalIcon), new PropertyString("/icons/toolbar_separator_horizontal.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::ToolBarSeparatorVerticalIcon), new PropertyString("/icons/toolbar_separator_vertical.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowCloseIcon), new PropertyString("/icons/window_close.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowCloseDisabledIcon), new PropertyString("/icons/window_close_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowCloseFocusIcon), new PropertyString("/icons/window_close_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowClosePressedIcon), new PropertyString("/icons/window_close_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowPinIcon), new PropertyString("/icons/window_pin.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnpinIcon), new PropertyString("/icons/window_unpin.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowPinDisabledIcon), new PropertyString("/icons/window_pin_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnpinDisabledIcon), new PropertyString("/icons/window_unpin_disbaled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowPinFocusIcon), new PropertyString("/icons/window_pin_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnpinFocusIcon), new PropertyString("/icons/window_unpin_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowPinPressedIcon), new PropertyString("/icons/window_pin_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnpinPressedIcon), new PropertyString("/icons/window_unpin_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowLockIcon), new PropertyString("/icons/window_lock.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnlockIcon), new PropertyString("/icons/window_unlock.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowLockDisabledIcon), new PropertyString("/icons/window_lock_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnlockDisabledIcon), new PropertyString("/icons/window_unlock_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowLockFocusIcon), new PropertyString("/icons/window_lock_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnlockFocusIcon), new PropertyString("/icons/window_unlock_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowLockPressedIcon), new PropertyString("/icons/window_lock_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUnlockPressedIcon), new PropertyString("/icons/window_unlock_pressed.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowGripIcon), new PropertyString("/icons/window_grip.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUndockIcon), new PropertyString("/icons/window_undock.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUndockDisabledIcon), new PropertyString("/icons/window_undock_disabled.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUndockFocusIcon), new PropertyString("/icons/window_undock_focus.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::WindowUndockPressedIcon), new PropertyString("/icons/window_undock_pressed.png"));

	m_files.insert_or_assign(toString(ColorStyleFileEntry::LogInBackgroundImage), new PropertyString("/images/OpenTwin.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::PropertyItemDeleteIcon), new PropertyString("/properties/Delete.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::PropertyGroupExpandedIcon), new PropertyString("/properties/ArrowGreenDown.png"));
	m_files.insert_or_assign(toString(ColorStyleFileEntry::PropertyGroupCollapsedIcon), new PropertyString("/properties/ArrowBlueRight.png"));
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
			if (k == ot::toString(ot::ColorStyleBaseFileMacro::PainterMacro)) {
				// Create color entry if doesnt exist
				const auto& it = m_painters.find(n);
				if (it == m_painters.end()) {
					m_painters.insert_or_assign(n, new ot::PropertyPainter2D(new ot::FillPainter2D));
				}
			}
			else if (k == ot::toString(ot::ColorStyleBaseFileMacro::FileMacro)) {
				// Create file entry
				const auto& it = m_files.find(n);
				if (it == m_files.end()) {
					m_files.insert_or_assign(n, new ot::PropertyString(n));
				}
			}
			else if (k == ot::toString(ot::ColorStyleBaseFileMacro::IntMacro)) {
				// Create file entry
				const auto& it = m_integer.find(n);
				if (it == m_integer.end()) {
					m_integer.insert_or_assign(n, new ot::PropertyInt);
				}
			}
			else if (k == ot::toString(ot::ColorStyleBaseFileMacro::DoubleMacro)) {
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
	
	// Files
	m_fileGroup->clear();
	for (const auto& it : m_files) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		m_fileGroup->addProperty(it.second);
	}

	// Integers
	m_intGroup->clear();
	for (const auto& it : m_integer) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		m_intGroup->addProperty(it.second);
	}

	// Doubles
	m_doubleGroup->clear();
	for (const auto& it : m_double) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		m_doubleGroup->addProperty(it.second);
	}

	// Values
	m_painterGroup->clear();
	for (const auto& it : m_painters) {
		it.second->setPropertyName(it.first);
		it.second->setPropertyTitle(it.first);
		m_painterGroup->addProperty(it.second);
	}

	m_propertyGrid->setupGridFromConfig(m_propertyGridConfig);
	m_propertyGrid->blockSignals(false);
}

bool ColorStyleEditor::generateFile(std::string& _result) {
	OTAssertNullptr(m_propertyGrid);
	using namespace ot;
	_result.clear();

	// Get name property
	const PropertyGridItem* nameItm = m_propertyGrid->findItem(CSE_GROUP_General, CSE_Name);
	if (!nameItm) {
		OT_LOG_EAS("Name property not found");
		return false;
	}

	// Get name value
	const PropertyInputString* iName = dynamic_cast<const PropertyInputString*>(nameItm->getInput());
	OTAssertNullptr(iName);
	std::string cName = iName->getCurrentValue().toString().toStdString();

	// Get files
	JsonDocument styleFilesDoc(rapidjson::kArrayType);
	const PropertyGridGroup* gStyleFiles = m_propertyGrid->findGroup(CSE_GROUP_Files);
	OTAssertNullptr(gStyleFiles);
	for (const PropertyGridItem* itm : gStyleFiles->childProperties()) {
		const PropertyInputString* inp = dynamic_cast<const PropertyInputString*>(itm->getInput());
		if (!inp) {
			OT_LOG_EAS("Property cast failed");
			return false;
		}

		JsonObject fObj;
		fObj.AddMember(JsonString(toString(ColorStyleFileValue::NameValue), styleFilesDoc.GetAllocator()), JsonString(itm->getPropertyData().getPropertyName(), styleFilesDoc.GetAllocator()), styleFilesDoc.GetAllocator());
		fObj.AddMember(JsonString(toString(ColorStyleFileValue::PathValue), styleFilesDoc.GetAllocator()), JsonString(inp->getCurrentText().toStdString(), styleFilesDoc.GetAllocator()), styleFilesDoc.GetAllocator());
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
	const PropertyGridGroup* gStyleInts = m_propertyGrid->findGroup(CSE_GROUP_Int);
	OTAssertNullptr(gStyleInts);
	for (const PropertyGridItem* itm : gStyleInts->childProperties()) {
		const PropertyInputInt* inp = dynamic_cast<const PropertyInputInt*>(itm->getInput());
		if (!inp) {
			OT_LOG_EAS("Property cast failed");
			return false;
		}

		JsonObject fObj;
		fObj.AddMember(JsonString(toString(ColorStyleFileValue::NameValue), styleIntDoc.GetAllocator()), JsonString(itm->getPropertyData().getPropertyName(), styleIntDoc.GetAllocator()), styleIntDoc.GetAllocator());
		fObj.AddMember(JsonString(toString(ColorStyleFileValue::ValueValue), styleIntDoc.GetAllocator()), JsonValue(inp->getValue()), styleIntDoc.GetAllocator());
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
	const PropertyGridGroup* gStyleDoubles = m_propertyGrid->findGroup(CSE_GROUP_Double);
	OTAssertNullptr(gStyleDoubles);
	for (const PropertyGridItem* itm : gStyleDoubles->childProperties()) {
		const PropertyInputDouble* inp = dynamic_cast<const PropertyInputDouble*>(itm->getInput());
		if (!inp) {
			OT_LOG_EAS("Property cast failed");
			return false;
		}

		JsonObject fObj;
		fObj.AddMember(JsonString(toString(ColorStyleFileValue::NameValue), styleDoubleDoc.GetAllocator()), JsonString(itm->getPropertyData().getPropertyName(), styleDoubleDoc.GetAllocator()), styleDoubleDoc.GetAllocator());
		fObj.AddMember(JsonString(toString(ColorStyleFileValue::ValueValue), styleDoubleDoc.GetAllocator()), JsonValue(inp->getValue()), styleDoubleDoc.GetAllocator());
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
	const PropertyGridGroup* gStyleValues = m_propertyGrid->findGroup(CSE_GROUP_Painter);
	OTAssertNullptr(gStyleValues);
	for (const PropertyGridItem* itm : gStyleValues->childProperties()) {
		const PropertyInputPainter2D* inp = dynamic_cast<const PropertyInputPainter2D*>(itm->getInput());
		if (!inp) {
			OT_LOG_EAS("Property cast failed");
			return false;
		}
		if (!inp->getButton()->getPainter()) {
			OT_LOG_EAS("No painter set");
			return false;
		}

		ColorStyleValue newValue;
		newValue.setEntryKey(stringToColorStyleValueEntry(itm->getPropertyData().getPropertyName()));
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
		const PropertyGridGroup* gColors = m_propertyGrid->findGroup(CSE_GROUP_Painter);
		OTAssertNullptr(gColors);
		for (const PropertyGridItem* itm : gColors->childProperties()) {
			const PropertyInputPainter2D* inp = dynamic_cast<const PropertyInputPainter2D*>(itm->getInput());
			if (!inp) {
				OT_LOG_EAS("Property cast failed");
				return false;
			}
			QString k = "%" + QString::fromStdString(ot::toString(ot::ColorStyleBaseFileMacro::PainterMacro)) + ":" + QString::fromStdString(inp->data().getPropertyName()) + "%";
			if (replacementMap.count(k)) {
				OT_LOG_W("Duplicate key \"" + k.toStdString() + "\"");
				return false;
			}
			replacementMap.insert_or_assign(k, QString::fromStdString(inp->getButton()->getPainter()->generateQss()));
		}
	}
	{
		const PropertyGridGroup* gFiles = m_propertyGrid->findGroup(CSE_GROUP_Files);
		OTAssertNullptr(gFiles);
		for (const PropertyGridItem* itm : gFiles->childProperties()) {
			const PropertyInputString* inp = dynamic_cast<const PropertyInputString*>(itm->getInput());
			if (!inp) {
				OT_LOG_EAS("Property cast failed");
				return false;
			}
			QString k = "%" + QString::fromStdString(ot::toString(ot::ColorStyleBaseFileMacro::FileMacro)) + ":" + QString::fromStdString(inp->data().getPropertyName()) + "%";
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
				OT_LOG_EAS("Property cast failed");
				return false;
			}
			QString k = "%" + QString::fromStdString(ot::toString(ot::ColorStyleBaseFileMacro::IntMacro)) + ":" + QString::fromStdString(inp->data().getPropertyName()) + "%";
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
				OT_LOG_EAS("Property cast failed");
				return false;
			}
			QString k = "%" + QString::fromStdString(ot::toString(ot::ColorStyleBaseFileMacro::DoubleMacro)) + ":" + QString::fromStdString(inp->data().getPropertyName()) + "%";
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

	_result = toString(ColorStyleFileKey::NameKey) + cName + "\n" +
		toString(ColorStyleFileKey::FileKey) + cStyleFiles + "\n" +
		toString(ColorStyleFileKey::IntegerKey) + cStyleInts + "\n" +
		toString(ColorStyleFileKey::DoubleKey) + cStyleDoubles + "\n" +
		toString(ColorStyleFileKey::PainterKey) + cStyleValue + "\n" +
		toString(ColorStyleFileKey::SheetKey) + "\n";
	_result.append(base.toStdString());

	return true;
}
