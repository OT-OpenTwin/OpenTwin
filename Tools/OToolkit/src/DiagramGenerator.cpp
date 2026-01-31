// @otlicense
// File: DiagramGenerator.cpp
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


// OToolkit header
#include "DiagramGenerator.h"
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTGui/Graphics/GraphicsLineItemCfg.h"
#include "OTGui/Graphics/GraphicsTextItemCfg.h"
#include "OTGui/Graphics/GraphicsGroupItemCfg.h"
#include "OTGui/Graphics/GraphicsEllipseItemCfg.h"
#include "OTGui/Graphics/GraphicsRectangularItemCfg.h"
#include "OTGui/Graphics/GraphicsDecoratedLineItemCfg.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/WidgetViewDock.h"
#include "OTWidgets/TreeWidgetItem.h"
#include "OTWidgets/PathBrowseEdit.h"
#include "OTWidgets/ExpanderWidget.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/GlobalWidgetViewManager.h"

// Qt header
#include <QtCore/qdiriterator.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qprogressbar.h>

// std header
#include <thread>

#define DIAGEN_LOG(___message) OTOOLKIT_LOG("DiaGen", ___message)
#define DIAGEN_LOGW(___message) OTOOLKIT_LOGW("DiaGen", ___message)
#define DIAGEN_LOGE(___message) OTOOLKIT_LOGE("DiaGen", ___message)

#define DIAGEN_PLOGW(___file, ___line, ___message) DIAGEN_LOGW(___file + QString(" @") + QString::number(___line) + ": " + ___message)
#define DIAGEN_PLOGE(___file, ___line, ___message) DIAGEN_LOGE(___file + QString(" @") + QString::number(___line) + ": " + ___message)

bool DiagramGenerator::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;

	QWidget* centralWidget = new QWidget;

	WidgetView* view = createCentralWidgetView(centralWidget, "Diagram Generator");
	_content.addView(view);

	QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);
	QHBoxLayout* pathLayout = new QHBoxLayout;
	centralLayout->addLayout(pathLayout);

	// Path
	pathLayout->addWidget(new Label("Root Path:", centralWidget));
	m_rootPathEdit = new PathBrowseEdit(PathBrowseMode::Directory, centralWidget);
	pathLayout->addWidget(m_rootPathEdit->getQWidget(), 1);

	m_syntaxSelect = new ComboBox(centralWidget);
	m_syntaxSelect->addItem("C++", static_cast<int>(ParseSyntax::Cpp));
	pathLayout->addWidget(new Label("Syntax:", centralWidget));
	pathLayout->addWidget(m_syntaxSelect);

	m_runButton = new PushButton("Run", centralWidget);
	pathLayout->addWidget(m_runButton);
	connect(m_runButton, &PushButton::clicked, this, &DiagramGenerator::slotRunParser);

	// Ingore
	ExpanderWidget* ignoreExpander = new ExpanderWidget("Ignore Patterns", centralWidget);
	centralLayout->addWidget(ignoreExpander);
	m_ignoreEditor = new TextEditor(ignoreExpander);
	ignoreExpander->setWidget(m_ignoreEditor);

	// Results
	QGroupBox* resultGroup = new QGroupBox("Results", centralWidget);
	centralLayout->addWidget(resultGroup, 1);

	QVBoxLayout* resultLayout = new QVBoxLayout(resultGroup);
	QHBoxLayout* resultControlLayout = new QHBoxLayout;
	resultLayout->addLayout(resultControlLayout);

	m_resultGenerateButton = new PushButton("Generate", resultGroup);
	connect(m_resultGenerateButton, &PushButton::clicked, this, &DiagramGenerator::slotGenerateResult);
	resultControlLayout->addStretch(1);
	resultControlLayout->addWidget(m_resultGenerateButton);

	m_resultTree = new TreeWidgetFilter(resultGroup);
	TreeWidget* tree = m_resultTree->getTreeWidget();
	tree->setColumnCount(1);
	tree->setHeaderHidden(true);
	resultLayout->addWidget(m_resultTree->getQWidget(), 1);
	connect(tree, &TreeWidget::itemDoubleClicked, this, &DiagramGenerator::slotResultItemDoubleClicked);

	// Status
	m_progressBar = new QProgressBar(centralWidget);
	_content.addStatusWidget(m_progressBar);

	return true;
}

void DiagramGenerator::restoreToolSettings(QSettings& _settings) {
	QString pth = _settings.value("DiagramGenerator.RootPath", "").toString();
	if (pth.isEmpty()) {
		pth = qgetenv("OPENTWIN_DEV_ROOT");
	}
	m_rootPathEdit->setPath(pth);

	QString ignoreData = _settings.value("DiagramGenerator.IgnoreData", "").toString();
	if (ignoreData.isEmpty()) {
		ignoreData =
			"# =============================================================\n"
			"# Default OpenTwin Ignore Patterns\n"
			"# =============================================================\n"
			"\n"
			"# === OpenTwin directories ===\n"
			"Certificates/\n"
			"Deployment/\n"
			"Deployment_Documentation/\n"
			"Deployment_Frontend/\n"
			"DeploymentDocumentation/\n"
			"Documentation/\n"
			"Framework/\n"
			"InstallationImages/\n"
			"Scripts/\n"
			"Templates/\n"
			"ThirdParty/\n"
			"Testdata/\n"
			"\n"
			"# === IDE / Editor directories ===\n"
			".vs/\n"
			".vscode/\n"
			".idea/\n"
			".settings/\n"
			".classpath\n"
			".project\n"
			"\n"
			"# === Build output ===\n"
			"bin/\n"
			"obj/\n"
			"build/\n"
			"dist/\n"
			"out/\n"
			"Debug/\n"
			"Release/\n"
			"x64/\n"
			"x86/\n"
			"CMakeFiles/\n"
			"\n"
			"# === Python ===\n"
			"__pycache__/\n"
			".pytest_cache/\n"
			".tox/\n"
			".mypy_cache/\n"
			"env/\n"
			"venv/\n"
			"# === General ===\n"
			".git/\n"
			".github/\n"
			"\n";
	}
	m_ignoreEditor->setPlainText(ignoreData);
}

bool DiagramGenerator::prepareToolShutdown(QSettings& _settings) {
	_settings.setValue("DiagramGenerator.RootPath", m_rootPathEdit->getPath());
	_settings.setValue("DiagramGenerator.IgnoreData", m_ignoreEditor->toPlainText());

	return otoolkit::Tool::prepareToolShutdown(_settings);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void DiagramGenerator::slotRunParser() {
	ParserData data;
	if (!initializeParserData(data)) {
		return;
	}

	// Clear data
	m_parser.clear();
	m_resultTree->getTreeWidget()->clear();

	// Close views
	for (ot::WidgetView* view : m_sequenceDiagramViews) {
		ot::GlobalWidgetViewManager::instance().closeView(view);
	}
	m_sequenceDiagramViews.clear();

	lockControls(true);

	std::thread worker(&DiagramGenerator::parserWorker, this, std::move(data));
	worker.detach();
}

void DiagramGenerator::slotParserFinished() {
	using namespace ot;
	TreeWidgetItem* rootDiaItem = new TreeWidgetItem;
	rootDiaItem->setText(0, "Sequence Diagrams");
	rootDiaItem->setData(0, Qt::UserRole, QVariant(static_cast<int>(ResultType::Container)));
	m_resultTree->getTreeWidget()->addTopLevelItem(rootDiaItem);

	for (const auto& it : m_parser.getSequenceDiagrams()) {
		const auto& dia = it.second;
		TreeWidgetItem* diaItem = new TreeWidgetItem;
		diaItem->setData(0, Qt::UserRole, QVariant(static_cast<int>(ResultType::SequenceDiagram)));
		diaItem->setData(0, Qt::UserRole + 1, QString::fromStdString(dia.getName()));
		diaItem->setText(0, QString::fromStdString(dia.getName()) + " (Function: " + QString::fromStdString(dia.getFunctionName()) + ")");
		rootDiaItem->addChild(diaItem);
	}

	TreeWidgetItem* rootFuncItem = new TreeWidgetItem;
	rootFuncItem->setText(0, "Sequence Functions");
	m_resultTree->getTreeWidget()->addTopLevelItem(rootFuncItem);

	for (const auto& it : m_parser.getSequenceFunctions()) {
		const auto& func = it.second;
		TreeWidgetItem* funcItem = new TreeWidgetItem;
		funcItem->setData(0, Qt::UserRole, QVariant(static_cast<int>(ResultType::SequenceFunction)));
		funcItem->setData(0, Qt::UserRole + 1, QString::fromStdString(func.getName()));
		funcItem->setText(0, QString::fromStdString(func.getName()));
		rootFuncItem->addChild(funcItem);
	}

	lockControls(false);
}

void DiagramGenerator::slotGenerateResult() {
	auto sel = m_resultTree->getTreeWidget()->selectedItems();
	if (sel.count() == 0) {
		return;
	}
	else if (sel.count() != 1) {
		DIAGEN_LOGE("Invalid selection");
		return;
	}
	else {
		slotResultItemDoubleClicked(sel.front(), 0);
	}
}

void DiagramGenerator::slotResultItemDoubleClicked(QTreeWidgetItem* _item, int _column) {
	return;
	/*
	if (!(_item->data(0, Qt::UserRole).canConvert<int>())) {
		DIAGEN_LOGE("Invalid item data");
		return;
	}

	ResultType type = static_cast<ResultType>(_item->data(0, Qt::UserRole).toInt());
	switch (type) {
	case ResultType::Container:
		// Do nothing
		break;
	case ResultType::SequenceDiagram:
	{
		QString diaName = _item->data(0, static_cast<int>(DiagramRoleIx::Name)).toString();
		auto diaIt = m_parseResult.sequenceDiagrams.find(diaName);
		if (diaIt == m_parseResult.sequenceDiagrams.end()) {
			DIAGEN_LOGE("Could not find sequence diagram: " + diaName);
			return;
		}
		generateSequenceDiagram(diaIt->second);
	}
		break;

	case ResultType::SequenceFunction:
	{
		QString funcName = _item->data(0, static_cast<int>(FunctionRoleIx::Name)).toString();
		auto funcIt = m_parseResult.sequenceFunctions.find(funcName);
		if (funcIt == m_parseResult.sequenceFunctions.end()) {
			DIAGEN_LOGE("Could not find sequence function: " + funcName);
			return;
		}
		generateSequenceDiagram(funcIt->second);
	}
		break;

	default:
		DIAGEN_LOGE("Unknown item type");
		break;
	}
	*/
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void DiagramGenerator::lockControls(bool _lock) {
	m_rootPathEdit->getLineEdit()->setEnabled(!_lock);
	m_rootPathEdit->getPushButton()->setEnabled(!_lock);
	m_syntaxSelect->setEnabled(!_lock);

	m_runButton->setEnabled(!_lock);

	m_ignoreEditor->setReadOnly(_lock);

	m_resultGenerateButton->setEnabled(!_lock);
	m_resultTree->getQWidget()->setEnabled(!_lock);

	m_progressBar->setRange(0, _lock ? 0 : 1);
	m_progressBar->setVisible(_lock);
}

bool DiagramGenerator::initializeParserData(ParserData& _data) {
	if (m_rootPathEdit->getPath().isEmpty()) {
		DIAGEN_LOGW("Please specify a valid root path");
		return false;
	}
	_data.rootPath = m_rootPathEdit->getPath();

	_data.syntax = static_cast<ParseSyntax>(m_syntaxSelect->currentData().toInt());

	switch (_data.syntax) {
	case ParseSyntax::Cpp:
		_data.fileExtensions << "cpp" << "cxx" << "c" << "h" << "hpp" << "hxx";
		break;

	default:
		break;
	}

	if (_data.fileExtensions.isEmpty()) {
		DIAGEN_LOGE("No file extensions specified for the selected syntax");
		return false;
	}

	// Parse ignore patterns
	QStringList ignoreLines = m_ignoreEditor->toPlainText().split('\n', Qt::SkipEmptyParts);
	for (const QString& line : ignoreLines) {
		QString pattern = line.trimmed();
		if (pattern.isEmpty() || pattern.front() == '#') {
			continue;
		}

		QString regexStr = globToRegex(pattern);
		if (regexStr.isEmpty()) {
			continue;
		}
		QRegularExpression regex(regexStr, QRegularExpression::PatternOption::CaseInsensitiveOption);
		if (!regex.isValid()) {
			DIAGEN_LOGW("Ignoring invalid ignore pattern: " + pattern);
			continue;
		}
		_data.ignorePatterns.push_back(std::move(regex));
	}

	return true;
}

QString DiagramGenerator::globToRegex(const QString& _pattern) {
	if (_pattern.isEmpty()) {
		return QString();
	}

	QString normalized = _pattern;
	normalized.replace("\\", "/");

	bool dirOnly = false;
	if (normalized.back() == '/') {
		dirOnly = true;
		normalized.removeLast(); // remove trailing slash for regex construction
	}

	QString regexStr;
	regexStr.reserve(normalized.size() * 2);

	// Anchor to start if pattern starts with '/'
	if (!normalized.isEmpty() && normalized.front() == '/') {
		regexStr.append("^");
		normalized.erase(normalized.begin());
	}
	else {
		regexStr.append("(^|.*/)");
	}

	for (QChar c : normalized) {
		switch (c.unicode()) {
		case '*': regexStr += ".*"; break;
		case '?': regexStr += '.'; break;
		case '.': case '(': case ')': case '+': case '|': case '^':
		case '$': case '{': case '}': case '[': case ']': case '\\':
			regexStr += '\\'; regexStr += c; break;
		default:
			regexStr += c; break;
		}
	}

	if (dirOnly) {
		regexStr += "(/.*)?$"; // match directory and everything inside
	}
	else {
		regexStr += "($|/.*)"; // match file or directory contents
	}

	return regexStr;
}

DiagramGenerator::SequenceViewData DiagramGenerator::initializeSequenceViewData(const QString& _name) {
	using namespace ot;

	SequenceViewData viewData;
	viewData.view = new ot::GraphicsView(nullptr);
	WidgetView* view = createCentralWidgetView(viewData.view, _name);

	WidgetViewBase vd = view->getViewData();
	vd.setViewFlag(WidgetViewBase::ViewIsCloseable, true);
	vd.setViewFlag(WidgetViewBase::ViewDefaultCloseHandling, false);
	view->setViewData(vd);
	
	GlobalWidgetViewManager::instance().addView(BasicServiceInformation(), view);

	viewData.lifeLinePen.setStyle(LineStyle::DashLine);

	QFontMetrics cfm(QtFactory::toQFont(viewData.callFont));
	viewData.callTextHeight = static_cast<double>(cfm.height());
	viewData.callVSeparation = viewData.callTextHeight + viewData.callSpacing + (viewData.callArrowHeadSize.height()) + viewData.callTextLineSpacing;

	// Finally calculate call starting position
	RectD titleRect;// = calculateLifeLineTextBoxRect("Title", viewData, Point2DD());
	viewData.callVStart = titleRect.getBottom() + viewData.firstCallOffset;

	return viewData;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Worker

void DiagramGenerator::parserWorker(ParserData&& _data) {
	DIAGEN_LOG("Starting parser worker...");

	ot::SequenceDiaCfgParser parser;
	std::list<std::string> fileExtensions;
	for (const QString& ext : _data.fileExtensions) {
		fileExtensions.push_back("." + ext.toStdString());
	}
	parser.parseDirectory(_data.rootPath.toStdString(), false, fileExtensions, ot::IgnoreRules::parseFromText(m_ignoreEditor->toPlainText().toStdString()), parser.NoLogging);

	// Done
	DIAGEN_LOG("Parse Summary:\n"
		"\tFiles Parsed: " + QString::number(parser.getFilesParsed()) + "\n"
		"\tInvalid Diagrams: " + QString::number(parser.getInvalidDiagrams()) + "\n"
		"\tValid Diagrams: " + QString::number(parser.getSequenceDiagrams().size()) + "\n"
		"\tInvalid Functions: " + QString::number(parser.getInvalidFunctions()) + "\n"
		"\tValid Functions: " + QString::number(parser.getSequenceFunctions().size()) + "\n \n"
		"\tParse time: " + QString::fromStdString(ot::DateTime::intervalToString(parser.getParseTime())) + "\n"
		"\tValidation time: " + QString::fromStdString(ot::DateTime::intervalToString(parser.getValidationTime())) + "\n"
		"\tTotal time: " + QString::fromStdString(ot::DateTime::intervalToString(parser.getTotalRuntime()))
	);

	m_parser = std::move(parser);

	QMetaObject::invokeMethod(this, &DiagramGenerator::slotParserFinished, Qt::QueuedConnection);
}

/*
void DiagramGenerator::generateSequenceDiagram(const SequenceDiagram& _diagram) {
	DIAGEN_LOG("Generating sequence diagram: " + _diagram.name);
	
	SequenceViewData viewData = initializeSequenceViewData(_diagram.name);

	auto funcIt = m_parseResult.sequenceFunctions.find(_diagram.functionName);
	if (funcIt == m_parseResult.sequenceFunctions.end()) {
		DIAGEN_LOGE("Could not find function for diagram: " + _diagram.name);
		return;
	}

	generateSequenceDiagram(viewData, funcIt->second);
}

void DiagramGenerator::generateSequenceDiagram(const SequenceFunction& _function) {
	DIAGEN_LOG("Generating sequence diagram for function: " + _function.name);

	SequenceViewData viewData = initializeSequenceViewData(_function.name);
	generateSequenceDiagram(viewData, _function);
}

void DiagramGenerator::generateSequenceDiagram(SequenceViewData& _viewData, const SequenceFunction& _function) {
	generateSequenceDiagramLifeLines(_function, _viewData);
	generateSequenceDiagramCalls(_function, _viewData);
	mergeProcessBoxes(_viewData);
	repositionLifelines(_viewData);
	repositionCalls(_viewData);
	generateItems(_viewData);
	clear(_viewData);

	_viewData.view->resetView();
}

void DiagramGenerator::generateSequenceDiagramLifeLines(const SequenceFunction& _function, SequenceViewData& _viewData) {
	auto lifeLineIt = _viewData.lifelines.find(_function.lifeLine);

	if (lifeLineIt == _viewData.lifelines.end()) {
		SequenceLifeLineItem lifeLine = generateNewLifeLineItem(_function.lifeLine, _viewData);
		lifeLineIt = _viewData.lifelines.emplace(_function.lifeLine, std::move(lifeLine)).first;
	}

	for (const SequenceCall& call : _function.calls) {
		auto funcIt = m_parseResult.sequenceFunctions.find(call.function);
		if (funcIt == m_parseResult.sequenceFunctions.end()) {
			DIAGEN_LOGE("Could not find function for call: " + call.function);
			continue;
		}

		generateSequenceDiagramLifeLines(funcIt->second, _viewData);
	}
}

void DiagramGenerator::generateSequenceDiagramCalls(const SequenceFunction& _function, SequenceViewData& _viewData) {
	auto fromLifeLineIt = _viewData.lifelines.find(_function.lifeLine);
	if (fromLifeLineIt == _viewData.lifelines.end()) {
		DIAGEN_LOGE("Could not find lifeline for function \"" + _function.name + "\"");
		return;
	}

	for (const SequenceCall& call : _function.calls) {
		auto funcIt = m_parseResult.sequenceFunctions.find(call.function);
		if (funcIt == m_parseResult.sequenceFunctions.end()) {
			DIAGEN_LOGE("Could not find function for call \"" + call.function + "\"");
			continue;
		}

		auto toLifeLineIt = _viewData.lifelines.find(funcIt->second.lifeLine);
		if (toLifeLineIt == _viewData.lifelines.end()) {
			DIAGEN_LOGE("Could not find lifeline for function \"" + funcIt->second.name + "\"");
			continue;
		}

		// Determine direction

		bool callToRight = true;

		if (fromLifeLineIt->second.group->getPosition().x() > toLifeLineIt->second.group->getPosition().x()) {
			callToRight = false;
		}
		
		double processStartY = 0.;
		double processEndY = 0.;

		// Create call item
		if (callToRight) {
			processStartY = generateNewCallItem(fromLifeLineIt->second, toLifeLineIt->second, _viewData, call.text, CallFlag::None);
		}
		else {
			processStartY = generateNewCallItem(toLifeLineIt->second, fromLifeLineIt->second, _viewData, call.text, CallFlag::ReverseCall);
		}

		// Process subsequent calls
		generateSequenceDiagramCalls(funcIt->second, _viewData);

		// Return call
		if (callToRight) {
			processEndY = generateNewCallItem(fromLifeLineIt->second, toLifeLineIt->second, _viewData, funcIt->second.returnValue.text, CallFlag::ReturnCall);
		}
		else {
			processEndY = generateNewCallItem(toLifeLineIt->second, fromLifeLineIt->second, _viewData, funcIt->second.returnValue.text, CallFlag::ReturnCall | CallFlag::ReverseCall);
		}

		// Create process boxes
		createProcessBox(fromLifeLineIt->second, processStartY, processEndY, _viewData);
		createProcessBox(toLifeLineIt->second, processStartY, processEndY, _viewData);
	}
}

void DiagramGenerator::repositionLifelines(SequenceViewData& _viewData) {
	using namespace ot;

	// Sort life lines from left to right

	std::list<SequenceLifeLineItem*> tmpLifeLines;
	for (auto& it : _viewData.lifelines) {
		tmpLifeLines.push_back(&it.second);
	}

	std::vector<SequenceLifeLineItem*> sortedLifeLines;
	sortedLifeLines.reserve(_viewData.lifelines.size());

	while (!tmpLifeLines.empty()) {
		double minX = std::numeric_limits<double>::max();
		
		// Find leftmost life line
		SequenceLifeLineItem* leftMost = nullptr;
		for (SequenceLifeLineItem* lifeLine : tmpLifeLines) {
			if (lifeLine->group->getPosition().x() < minX) {
				minX = lifeLine->group->getPosition().x();
				leftMost = lifeLine;
			}
		}

		sortedLifeLines.push_back(leftMost);
		tmpLifeLines.remove(leftMost);
	}

	for (size_t x = 0; x < (sortedLifeLines.size() - 1); x++) {
		SequenceLifeLineItem* lifeLine = sortedLifeLines[x];

		// Calculate desired position of next life line(s)
		for (size_t nextX = x + 1; nextX < sortedLifeLines.size(); nextX++) {
			SequenceLifeLineItem* nextLifeLine = sortedLifeLines[nextX];
			
			double maxTextSize = 0.;

			// Calculate max text size of calls to the next lifeline
			for (SequenceCallItem& callItem : lifeLine->calls) {
				if (callItem.toLifeLine == nextLifeLine->name) {
					QFontMetrics fm(ot::QtFactory::toQFont(callItem.text->getTextFont()));
					maxTextSize = std::max(maxTextSize, static_cast<double>(fm.size(Qt::TextSingleLine, QString::fromStdString(callItem.text->getText())).width()));
				}
			}

			// Apply margins
			maxTextSize += (_viewData.callTextMargin * 2.);

			// If text is longer than distance, move right
			double distance = calculateLifelineDistance(*lifeLine, *nextLifeLine, _viewData);
			if (distance < maxTextSize) {
				double delta = maxTextSize - distance;

				// Move all right lifelines
				for (int mv = nextX; mv < sortedLifeLines.size(); mv++) {
					Point2DD pos = sortedLifeLines[mv]->group->getPosition();
					pos.setX(pos.x() + delta);
					sortedLifeLines[mv]->group->setPosition(pos);

					for (GraphicsRectangularItemCfg* process : sortedLifeLines[mv]->processes) {
						Point2DD pPos = process->getPosition();
						pPos.setX(pPos.x() + delta);
						process->setPosition(pPos);
					}
				}
			}
		}
	}
}

void DiagramGenerator::repositionCalls(SequenceViewData& _viewData) {
	for (auto& lifeline : _viewData.lifelines) {
		const double startX = lifeline.second.group->getPosition().x() + lifeline.second.lifeLine->getPosition().x() + (_viewData.lifeLineProcessWidth / 2.) + 1;

		for (auto& callItem : lifeline.second.calls) {
			auto toLifeLineIt = _viewData.lifelines.find(callItem.toLifeLine);
			if (toLifeLineIt == _viewData.lifelines.end()) {
				DIAGEN_LOGE("Could not find lifeline for call to \"" + callItem.toLifeLine + "\"");
				continue;
			}

			// Recalculate line x positions
			const double destX = ((toLifeLineIt->second.group->getPosition().x() + toLifeLineIt->second.lifeLine->getPosition().x()) - (_viewData.lifeLineProcessWidth / 2.)) - 1;
			
			callItem.line->setPosition(ot::Point2DD(startX, callItem.line->getPosition().y()));
			callItem.line->setTo(ot::Point2DD(destX - startX, 0.));
			callItem.text->setPosition(ot::Point2DD(
				startX + _viewData.callTextMargin + (_viewData.callArrowHeadSize.width() / 2.), 
				callItem.text->getPosition().y()
			));
		}
	}
}

void DiagramGenerator::mergeProcessBoxes(SequenceViewData& _viewData) {
	for (auto& lifeline : _viewData.lifelines) {
		for (auto process = lifeline.second.processes.begin(); process != lifeline.second.processes.end(); process++) {
			const double startY = (*process)->getPosition().y();
			const double endY = startY + (*process)->getSize().height();

			for (auto check = lifeline.second.processes.begin(); check != lifeline.second.processes.end(); ) {
				if (check == process) {
					check++;
					continue;
				}

				const double cStartY = (*check)->getPosition().y();
				const double cEndY = cStartY + (*check)->getSize().height();
				
				if ((cStartY <= endY && cStartY >= startY) || (cEndY <= endY && cEndY >= startY)) {
					// Merge
					const double newStartY = std::min(startY, cStartY);
					const double newEndY = std::max(endY, cEndY);
					(*process)->setPosition(ot::Point2DD(
						(*process)->getPosition().x(),
						newStartY
					));
					(*process)->setSize(ot::Size2DD(
						(*process)->getSize().width(),
						newEndY - newStartY
					));

					check = lifeline.second.processes.erase(check);
				}
				else {
					check++;
				}
			}
		}
	}
}

void DiagramGenerator::generateItems(SequenceViewData& _viewData) {
	using namespace ot;

	ot::UID uid = 0;

	for (const auto& it : _viewData.lifelines) {
		// Generate life lines
		GraphicsItem* itm = GraphicsItemFactory::itemFromConfig(it.second.group);
		if (!itm) {
			DIAGEN_LOGE("Could not create graphics item for lifeline: " + it.first);
			continue;
		}

		itm->setGraphicsItemUid(uid++);
		_viewData.view->addItem(itm);

		// Generate process boxes
		for (GraphicsRectangularItemCfg* processBoxCfg : it.second.processes) {
			GraphicsItem* processItm = GraphicsItemFactory::itemFromConfig(processBoxCfg);
			if (!processItm) {
				DIAGEN_LOGE("Could not create graphics item for process box in lifeline: " + it.first);
				continue;
			}
			processItm->setGraphicsItemUid(uid++);
			_viewData.view->addItem(processItm);
		}

		// Generate calls
		for (const auto& callIt : it.second.calls) {
			// Text
			GraphicsItem* textItm = GraphicsItemFactory::itemFromConfig(callIt.text);
			if (!textItm) {
				DIAGEN_LOGE("Could not create graphics item for call text in lifeline: " + it.first);
				continue;
			}
			textItm->setGraphicsItemUid(uid++);
			_viewData.view->addItem(textItm);

			// Line
			GraphicsItem* lineItm = GraphicsItemFactory::itemFromConfig(callIt.line);
			if (!lineItm) {
				DIAGEN_LOGE("Could not create graphics item for call line in lifeline: " + it.first);
				continue;
			}
			lineItm->setGraphicsItemUid(uid++);
			_viewData.view->addItem(lineItm);
		}
	}
}

void DiagramGenerator::clear(SequenceViewData& _viewData) {
	for (const auto& it : _viewData.lifelines) {
		delete it.second.group;

		for (const auto& callIt : it.second.calls) {
			clear(callIt);
		}
	}

	_viewData.lifelines.clear();
}

void DiagramGenerator::clear(const SequenceCallItem& _callItem) {
	delete _callItem.line;
	delete _callItem.text;
}

DiagramGenerator::SequenceLifeLineItem DiagramGenerator::generateNewLifeLineItem(const QString& _name, const SequenceViewData& _viewData) {
	using namespace ot;
	std::string name = _name.toStdString();

	// Calculate sizes and positions
	Point2DD newPos = calculateNextLifeLinePosition(_viewData);
	RectD textRect = calculateLifeLineTextBoxRect(_name, _viewData, newPos);

	// Empty item
	DiagramGenerator::SequenceLifeLineItem lifeLine;
	lifeLine.name = _name;

	// Group
	lifeLine.group = new GraphicsGroupItemCfg;
	lifeLine.group->setGraphicsItemFlags(GraphicsItemCfg::ItemIsSelectable | GraphicsItemCfg::ItemIsMoveable);
	lifeLine.group->setName(name);
	lifeLine.group->setZValue(_viewData.lifeLineZValue);
	lifeLine.group->setPosition(textRect.getTopLeft());

	// Text box
	lifeLine.textBox = new GraphicsRectangularItemCfg;
	lifeLine.textBox->setName(name + "_TextBox");
	lifeLine.textBox->setSize(textRect.getSize());
	lifeLine.textBox->setFixedSize(textRect.getSize());
	lifeLine.textBox->setZValue(_viewData.lifeLineTextBoxZValue);
	ot::PenFCfg boxPen = lifeLine.textBox->getOutline();
	boxPen.setPainter(new ot::StyleRefPainter2D(ColorStyleValueEntry::SequenceLifelineTextBoxBorder));
	lifeLine.textBox->setOutline(boxPen);
	lifeLine.textBox->setBackgroundPainer(new ot::StyleRefPainter2D(ColorStyleValueEntry::SequenceLifelineTextBox));
	lifeLine.group->addItem(lifeLine.textBox);

	// Text
	lifeLine.text = new GraphicsTextItemCfg;
	lifeLine.text->setName(name + "_Text");
	lifeLine.text->setTextFont(_viewData.lifeLineFont);
	lifeLine.text->setFixedSize(textRect.getSize());
	lifeLine.text->setAlignment(Alignment::Center);
	lifeLine.text->setText(name);
	lifeLine.text->setZValue(_viewData.lifeLineTextZValue);
	lifeLine.text->setTextPainter(new ot::StyleRefPainter2D(ColorStyleValueEntry::SequenceLifelineText));
	lifeLine.group->addItem(lifeLine.text);

	// Line
	lifeLine.lifeLine = new GraphicsLineItemCfg;
	lifeLine.lifeLine->setName(name + "_LifeLine");
	lifeLine.lifeLine->setLineStyle(_viewData.lifeLinePen);
	lifeLine.lifeLine->setZValue(_viewData.lifeLineLineZValue);
	lifeLine.lifeLine->setPosition(Point2DD(
		textRect.getSize().width() / 2.,
		textRect.getBottom()
	));
	lifeLine.lifeLine->setFrom(Point2DD(0., 0.));
	lifeLine.lifeLine->setTo(Point2DD(0., 10.));
	lifeLine.lifeLine->setPainter(new ot::StyleRefPainter2D(ColorStyleValueEntry::SequenceLifelineLine));
	lifeLine.group->addItem(lifeLine.lifeLine);

	return lifeLine;
}

double DiagramGenerator::generateNewCallItem(SequenceLifeLineItem& _from, SequenceLifeLineItem& _to, SequenceViewData& _viewData, const QString& _callText, const CallFlags& _flags) {
	using namespace ot;

	// Calculate y position
	double callY = _viewData.callVStart + (_viewData.callVSeparation * static_cast<double>(_viewData.currentCallIndex));
	_viewData.currentCallIndex++;

	std::string callName = "Call_" + std::to_string(_viewData.currentCallIndex);

	// Create call line
	SequenceCallItem callItem;
	callItem.flags = _flags;

	callItem.text = new GraphicsTextItemCfg;
	callItem.text->setGraphicsItemFlags(GraphicsItemCfg::ItemIsSelectable | GraphicsItemCfg::ItemIsMoveable);
	callItem.text->setName(callName + "_Text");
	callItem.text->setTextFont(_viewData.callFont);
	callItem.text->setText(std::to_string(_viewData.currentCallIndex) + ": " + (_callText.isEmpty() ? "-" : _callText.toStdString()));
	callItem.text->setPosition(Point2DD(0., callY));
	callItem.text->setZValue(_viewData.callZValue);
	callItem.text->setTextPainter(new StyleRefPainter2D(ColorStyleValueEntry::SequenceMessageText));

	callY += (_viewData.callTextLineSpacing + _viewData.callTextHeight);

	callItem.line = new GraphicsDecoratedLineItemCfg;
	callItem.line->setGraphicsItemFlags(GraphicsItemCfg::ItemIsSelectable | GraphicsItemCfg::ItemIsMoveable);
	callItem.line->setName(callName + "_Line");
	callItem.line->setPosition(Point2DD(0., callY));
	callItem.line->setFrom(Point2DD(0., 0));
	callItem.line->setTo(Point2DD(10., 0));
	callItem.line->setZValue(_viewData.callZValue);
	callItem.line->setPainter(new StyleRefPainter2D(ColorStyleValueEntry::SequenceMessageArrow));

	GraphicsDecorationCfg arrowHead(GraphicsDecorationCfg::VArrow);
	arrowHead.setSize(_viewData.callArrowHeadSize);
	arrowHead.setFillPainter(new StyleRefPainter2D(ColorStyleValueEntry::SequenceMessageArrow));

	PenFCfg arrowPen = arrowHead.getOutlinePen();
	arrowPen.setPainter(new StyleRefPainter2D(ColorStyleValueEntry::SequenceMessageArrow));
	arrowHead.setOutlinePen(arrowPen);

	if (_flags & CallFlag::ReturnCall) {
		PenFCfg linePen = callItem.line->getLineStyle();
		linePen.setStyle(LineStyle::DashLine);
		callItem.line->setLineStyle(linePen);

		arrowHead.setSymbol(GraphicsDecorationCfg::VArrow);

		if (_flags & CallFlag::ReverseCall) {
			callItem.line->setToDecoration(arrowHead);
		}
		else {
			callItem.line->setFromDecoration(arrowHead);
		}
	}
	else {
		arrowHead.setSymbol(GraphicsDecorationCfg::Arrow);	

		if (_flags & CallFlag::ReverseCall) {
			callItem.line->setFromDecoration(arrowHead);
		}
		else {
			callItem.line->setToDecoration(arrowHead);
		}
	}

	callItem.fromLifeLine = _from.name;
	callItem.toLifeLine = _to.name;

	_from.calls.push_back(std::move(callItem));

	double lifeLineEndY = callY + (_viewData.callArrowHeadSize.height() / 2.) + _viewData.firstCallOffset;

	_from.lifeLine->setTo(ot::Point2DD(0., lifeLineEndY - _from.lifeLine->getPosition().y()));
	_to.lifeLine->setTo(ot::Point2DD(0., lifeLineEndY - _from.lifeLine->getPosition().y()));

	return callY;
}

ot::Point2DD DiagramGenerator::calculateNextLifeLinePosition(const SequenceViewData& _viewData) {
	double x = 0.;
	double y = 0.;

	for (const auto& it : _viewData.lifelines) {
		double rightEdge = it.second.group->getPosition().x() + it.second.textBox->getSize().width();
		if (rightEdge > x) {
			x = rightEdge;
		}
	}

	if (x > 0.) {
		x += _viewData.lifeLineSpacing;
	}

	return ot::Point2DD(x, y);
}

ot::RectD DiagramGenerator::calculateLifeLineTextBoxRect(const QString& _text, const SequenceViewData& _viewData, const ot::Point2DD& _position) {
	ot::RectD rect;
	
	QFontMetrics fm(ot::QtFactory::toQFont(_viewData.lifeLineFont));
	QSize textSize = fm.size(Qt::TextSingleLine, _text);
	rect.setSize(
		ot::Size2DD(
			static_cast<double>(textSize.width()) + _viewData.lifeLineTextMargins.left() + _viewData.lifeLineTextMargins.right(),
			static_cast<double>(textSize.height()) + _viewData.lifeLineTextMargins.top() + _viewData.lifeLineTextMargins.bottom()
			));
	rect.moveTo(_position);

	return rect;
}

double DiagramGenerator::calculateLifelineDistance(const SequenceLifeLineItem& _left, const SequenceLifeLineItem& _right, const SequenceViewData& _viewData) {
	double leftPos = _left.group->getPosition().x() + _left.lifeLine->getPosition().x();
	double rightPos = _right.group->getPosition().x() + _right.lifeLine->getPosition().x();

	if (leftPos < 0.) {
		rightPos += (-leftPos);
		leftPos = 0.;
	}

	return (rightPos - (_viewData.lifeLineProcessWidth / 2.)) - (leftPos + (_viewData.lifeLineProcessWidth / 2.));
}

ot::GraphicsRectangularItemCfg* DiagramGenerator::createProcessBox(SequenceLifeLineItem& _lifeLine, double _startYPos, double _endYPos, const SequenceViewData& _viewData) {
	using namespace ot;


	// Create new process box
	GraphicsRectangularItemCfg* itm = new GraphicsRectangularItemCfg;
	itm->setGraphicsItemFlags(GraphicsItemCfg::ItemIsSelectable | GraphicsItemCfg::ItemIsMoveable);
	itm->setName(_lifeLine.name.toStdString() + "_Process_" + std::to_string(_lifeLine.processes.size() + 1));
	itm->setPosition(Point2DD(
		(_lifeLine.lifeLine->getPosition().x() + _lifeLine.group->getPosition().x()) - (_viewData.lifeLineProcessWidth / 2.),
		_startYPos
	));
	itm->setSize(Size2DD(
		_viewData.lifeLineProcessWidth,
		_endYPos - _startYPos
	));
	itm->setZValue(_viewData.lifeLineProcessZValue);
	PenFCfg processPen = itm->getOutline();
	processPen.setPainter(new StyleRefPainter2D(ColorStyleValueEntry::SequenceLifelineProcessBorder));
	itm->setOutline(processPen);
	itm->setBackgroundPainer(new StyleRefPainter2D(ColorStyleValueEntry::SequenceLifelineProcess));
	_lifeLine.processes.push_back(itm);

	return itm;
}
*/