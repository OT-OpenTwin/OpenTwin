// @otlicense


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
	m_parseResult = ParserResult();
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

	for (const auto& it : m_parseResult.sequenceDiagrams) {
		const SequenceDiagram& dia = it.second;
		TreeWidgetItem* diaItem = new TreeWidgetItem;
		diaItem->setData(0, Qt::UserRole, QVariant(static_cast<int>(ResultType::SequenceDiagram)));
		diaItem->setData(0, Qt::UserRole + 1, dia.name);
		diaItem->setText(0, dia.name + " (Function: " + dia.functionName + ")");
		rootDiaItem->addChild(diaItem);
	}

	TreeWidgetItem* rootFuncItem = new TreeWidgetItem;
	rootFuncItem->setText(0, "Sequence Functions");
	m_resultTree->getTreeWidget()->addTopLevelItem(rootFuncItem);

	for (const auto& it : m_parseResult.sequenceFunctions) {
		const SequenceFunction& func = it.second;
		TreeWidgetItem* funcItem = new TreeWidgetItem;
		funcItem->setData(0, Qt::UserRole, QVariant(static_cast<int>(ResultType::SequenceFunction)));
		funcItem->setData(0, Qt::UserRole + 1, func.name);
		funcItem->setText(0, func.name);
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
	RectD titleRect = calculateLifeLineTextBoxRect("Title", viewData, Point2DD());
	viewData.callVStart = titleRect.getBottom() + viewData.firstCallOffset;

	return viewData;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Worker

void DiagramGenerator::parserWorker(ParserData&& _data) {
	auto startTime = ot::DateTime::msSinceEpoch();

	DIAGEN_LOG("Starting parser worker...");
	DIAGEN_LOG("Root path: " + _data.rootPath);

	// Parse
	parseDirectory(_data.rootPath, _data);

	DIAGEN_LOG("Parsing complete. Validating...");

	// Validate
	validateFunctions();
	validateDiagrams();

	// Done
	std::string intervalStr = ot::DateTime::intervalToString(ot::DateTime::msSinceEpoch() - startTime);
	DIAGEN_LOG("Parse Summary:\n"
		"\tDirectories Parsed: " + QString::number(m_parseResult.directoriesParsed) + "\n"
		"\tFiles Parsed: " + QString::number(m_parseResult.filesParsed) + "\n"
		"\tInvalid Diagrams: " + QString::number(m_parseResult.invalidDiagrams) + "\n"
		"\tValid Diagrams: " + QString::number(m_parseResult.sequenceDiagrams.size()) + "\n"
		"\tInvalid Functions: " + QString::number(m_parseResult.invalidFunctions) + "\n"
		"\tValid Functions: " + QString::number(m_parseResult.sequenceFunctions.size()) + "\n"
		"\tTime Taken: " + QString::fromStdString(intervalStr)
	);

	for (const auto& it : m_parseResult.sequenceDiagrams) {
		const SequenceDiagram& dia = it.second;
		DIAGEN_LOG("Diagram: \"" + dia.name + "\"; Function: \"" + dia.functionName + "\"");
	}

	for (const auto& it : m_parseResult.sequenceFunctions) {
		const SequenceFunction& func = it.second;
		DIAGEN_LOG("Function: \"" + func.name + "\"; Lifeline: \"" + func.lifeLine + "\"; Default: \"" + func.defaultText + "\"");
	}

	QMetaObject::invokeMethod(this, &DiagramGenerator::slotParserFinished, Qt::QueuedConnection);
}

void DiagramGenerator::parseDirectory(const QString& _path, const ParserData& _data) {
	m_parseResult.directoriesParsed++;
	
	QDirIterator dirIt(_path, QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	while (dirIt.hasNext()) {
		QFileInfo fileInfo = dirIt.nextFileInfo();
		if (fileInfo.isDir()) {
			// Ensure directory is not ignored
			bool skipDirectory = false;
			for (const QRegularExpression& regex : _data.ignorePatterns) {
				if (regex.match(fileInfo.filePath()).hasMatch()) {
					skipDirectory = true;
					break;
				}
			}

			if (!skipDirectory) {
				parseDirectory(fileInfo.filePath(), _data);
			}
		}
		else if (_data.fileExtensions.contains(fileInfo.suffix(), Qt::CaseInsensitive)) {
			parseFile(fileInfo.filePath(), _data);
		}
	}
}

void DiagramGenerator::parseFile(const QString& _filePath, const ParserData& _data) {
	m_parseResult.filesParsed++;

	QFile file(_filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		DIAGEN_LOGE("Could not open file for reading: " + _filePath);
		return;
	}

	QTextStream in(&file);

	SequenceFunction currentFunction;
	size_t lineCt = 0;
	while (!in.atEnd()) {
		QString line = in.readLine().trimmed();
		lineCt++;
		int prefLen = 0;

		if (line.startsWith("//@seq")) {
			prefLen = 6;
		}
		else if (line.startsWith("// @seq")) {
			prefLen = 7;
		}

		if (prefLen > 0) {
			QString commandLine = line.mid(prefLen).trimmed();
			if (commandLine.startsWith("func ")) {
				finalizeFunction(_filePath, lineCt, std::move(currentFunction));
				parseFunctionLine(_filePath, lineCt, commandLine, 5, currentFunction);
			}
			else if (commandLine.startsWith("function ")) {
				finalizeFunction(_filePath, lineCt, std::move(currentFunction));
				parseFunctionLine(_filePath, lineCt, commandLine, 9, currentFunction);
			}
			else if (commandLine.startsWith("diagram ")) {
				parseDiagramLine(_filePath, lineCt, commandLine, 8);
			}
			else if (commandLine.startsWith("dia ")) {
				parseDiagramLine(_filePath, lineCt, commandLine, 4);
			}
			else if (commandLine.startsWith("start ")) {
				parseDiagramLine(_filePath, lineCt, commandLine, 6);
			}
			else if (commandLine.startsWith("call ")) {
				if (!currentFunction.isValid()) {
					DIAGEN_PLOGW(_filePath, lineCt, "Ignoring 'seq call' command outside of function");
					continue;
				}
				parseCallLine(_filePath, lineCt, commandLine, currentFunction);
			}
			else if (commandLine.startsWith("return ")) {
				if (!currentFunction.isValid()) {
					DIAGEN_PLOGW(_filePath, lineCt, "Ignoring 'seq call' command outside of function");
					continue;
				}

				if (!currentFunction.returnValue.text.isEmpty()) {
					DIAGEN_PLOGW(_filePath, lineCt, "Multiple 'seq return' commands. Ignoring duplicate...");
					continue;
				}

				QString cmd = commandLine.mid(QString("return ").length()).trimmed();
				if (cmd.isEmpty()) {
					cmd = " ";
				}
				currentFunction.returnValue.text = cmd;
			}
		}
	}

	file.close();

	finalizeFunction(_filePath, lineCt, std::move(currentFunction));
}

void DiagramGenerator::finalizeFunction(const QString& _filePath, size_t _lineCounter, SequenceFunction&& _function) {
	if (!_function.isValid()) {
		return;
	}

	auto it = m_parseResult.sequenceFunctions.find(_function.name);
	if (it != m_parseResult.sequenceFunctions.end()) {
		DIAGEN_PLOGW(_filePath, _lineCounter, "Function \"" + _function.name + "\" already defined in file " + it->second.filePath);
		return;
	}

	_function.filePath = _filePath;
	m_parseResult.sequenceFunctions.emplace(_function.name, std::move(_function));
}

void DiagramGenerator::parseFunctionLine(const QString& _filePath, size_t _lineCounter, const QString& _line, int _prefixLen, SequenceFunction& _function) {
	_function = SequenceFunction();

	QString cmd = _line.mid(_prefixLen).trimmed();

	if (cmd.isEmpty()) {
		DIAGEN_PLOGW(_filePath, _lineCounter, "Missing attributes while parsing \"seq func\" command");
		return;
	}

	QString key;
	QString value;

	while (filterKeyValue(_filePath, _lineCounter, "seq func", cmd, key, value)) {
		if (key == "n" || key == "name") {
			_function.name = value;
		}
		else if (key == "l" || key == "life" || key == "lifeline") {
			_function.lifeLine = value;
		}
		else if (key == "t" || key == "d" || key == "text" || key == "txt" || key == "default" || key == "def") {
			_function.defaultText = value;
		}
		else {
			DIAGEN_PLOGW(_filePath, _lineCounter, "Unknown key \"" + key + "\" while parsing \"seq func\" command");
		}
	}

	if (!_function.isValid()) {
		DIAGEN_PLOGW(_filePath, _lineCounter, "Invalid function syntax (name and lifeline must be specified)");
	}
}

void DiagramGenerator::parseCallLine(const QString& _filePath, size_t _lineCounter, const QString& _line, SequenceFunction& _function) {
	SequenceCall call;
	QString cmd = _line.mid(QString("call ").length()).trimmed();
	if (cmd.isEmpty()) {
		DIAGEN_PLOGW(_filePath, _lineCounter, "Missing attributes while parsing \"seq call\" command");
		return;
	}

	if (cmd.contains('=')) {
		QString key;
		QString value;

		while (filterKeyValue(_filePath, _lineCounter, "seq call", cmd, key, value)) {
			if (key == "f" || key == "func" || key == "function") {
				call.function = value;
			}
			else if (key == "t" || key == "txt" || key == "text") {
				call.text = value;
			}
			else {
				DIAGEN_PLOGW(_filePath, _lineCounter, "Unknown key \"" + key + "\" while parsing \"seq call\" command");
			}
		}
	}
	else {
		// Simple call syntax: function name only
		call.function = cmd;
	}

	if (call.function.isEmpty()) {
		DIAGEN_PLOGW(_filePath, _lineCounter, "Function not specified while parsing \"seq call\" command");
		return;
	}

	_function.calls.push_back(std::move(call));
}

void DiagramGenerator::parseDiagramLine(const QString& _filePath, size_t _lineCounter, const QString& _line, int _prefixLen) {
	SequenceDiagram diagram;
	QString cmd = _line.mid(_prefixLen).trimmed();
	if (cmd.isEmpty()) {
		DIAGEN_PLOGW(_filePath, _lineCounter, "Missing attributes while parsing \"seq diagram\" command");
		return;
	}

	QString key;
	QString value;

	while (filterKeyValue(_filePath, _lineCounter, "seq diagram", cmd, key, value)) {
		if (key == "f" || key == "func" || key == "function") {
			diagram.functionName = value;
		}
		else if (key == "n" || key == "name") {
			diagram.name = value;
		}
		else {
			DIAGEN_PLOGW(_filePath, _lineCounter, "Unknown key \"" + key + "\" while parsing \"seq diagram\" command");
		}
	}

	if (diagram.functionName.isEmpty()) {
		DIAGEN_PLOGW(_filePath, _lineCounter, "Function name not specified while parsing \"seq diagram\" command");
		return;
	}
	
	if (diagram.name.isEmpty()) {
		diagram.name = diagram.functionName;
	}

	if (m_parseResult.sequenceDiagrams.find(diagram.name) != m_parseResult.sequenceDiagrams.end()) {
		DIAGEN_PLOGW(_filePath, _lineCounter, "Diagram \"" + diagram.name + "\" already defined. Ignoring duplicate...");
		return;
	}

	m_parseResult.sequenceDiagrams.emplace(diagram.name, std::move(diagram));
}

void DiagramGenerator::validateFunctions() {
	bool hasError = true;
	while (hasError) {
		hasError = false;

		for (auto it = m_parseResult.sequenceFunctions.begin(); it != m_parseResult.sequenceFunctions.end(); it++) {
			SequenceFunction& func = it->second;
			std::set<QString> usedFunctions;
			if (!validateFunction(func.name, func, usedFunctions)) {
				m_parseResult.invalidFunctions++;
				m_parseResult.sequenceFunctions.erase(it);
				hasError = true;
				break;
			}
		}
	}
}

bool DiagramGenerator::validateFunction(const QString& _rootFunction, SequenceFunction& _function, std::set<QString>& _usedFunctions) {
	if (_usedFunctions.find(_function.name) != _usedFunctions.end()) {
		DIAGEN_LOGW("Detected recursive function call for function \"" + _function.name + "\" while parsing root function \"" + _rootFunction + "\". Recursion is not supported.");
		return false;
	}

	for (SequenceCall& call : _function.calls) {
		auto it = m_parseResult.sequenceFunctions.find(call.function);
		if (it == m_parseResult.sequenceFunctions.end()) {
			DIAGEN_LOGW("Function \"" + _function.name + "\" calls undefined function \"" + call.function + "\" while parsing root function \"" + _rootFunction + "\".");
			return false;
		}

		// Apply default text if not specified
		if (call.text.isEmpty()) {
			call.text = it->second.defaultText;
		}

		_usedFunctions.insert(_function.name);
		if (!validateFunction(_rootFunction, it->second, _usedFunctions)) {
			return false;
		}
		_usedFunctions.erase(_function.name);
	}

	return true;
}

void DiagramGenerator::validateDiagrams() {
	for (auto it = m_parseResult.sequenceDiagrams.begin(); it != m_parseResult.sequenceDiagrams.end(); ) {
		if (m_parseResult.sequenceFunctions.find(it->second.functionName) == m_parseResult.sequenceFunctions.end()) {
			DIAGEN_LOGW("Diagram \"" + it->second.name + "\" references undefined function \"" + it->second.functionName + "\". Removing diagram...");
			m_parseResult.invalidDiagrams++;
			it = m_parseResult.sequenceDiagrams.erase(it);
		}
		else {
			it++;
		}
	}
}

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
		
		// Create call item
		if (callToRight) {
			generateNewCallItem(fromLifeLineIt->second, toLifeLineIt->second, _viewData, call.text, CallFlag::None);
		}
		else {
			generateNewCallItem(toLifeLineIt->second, fromLifeLineIt->second, _viewData, call.text, CallFlag::ReverseCall);
		}

		// Process subsequent calls
		generateSequenceDiagramCalls(funcIt->second, _viewData);

		// Return call
		if (callToRight) {
			generateNewCallItem(fromLifeLineIt->second, toLifeLineIt->second, _viewData, funcIt->second.returnValue.text, CallFlag::ReturnCall);
		}
		else {
			generateNewCallItem(toLifeLineIt->second, fromLifeLineIt->second, _viewData, funcIt->second.returnValue.text, CallFlag::ReturnCall | CallFlag::ReverseCall);
		}
	}
}

void DiagramGenerator::repositionLifelines(SequenceViewData& _viewData) {
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
					ot::Point2DD pos = sortedLifeLines[mv]->group->getPosition();
					pos.setX(pos.x() + delta);
					sortedLifeLines[mv]->group->setPosition(pos);
				}
			}
		}
	}
}

void DiagramGenerator::repositionCalls(SequenceViewData& _viewData) {
	for (auto& lifeline : _viewData.lifelines) {
		const double startX = lifeline.second.group->getPosition().x() + lifeline.second.lifeLine->getPosition().x() + (_viewData.lifeLineProcessWidth / 2.);

		for (auto& callItem : lifeline.second.calls) {
			auto toLifeLineIt = _viewData.lifelines.find(callItem.toLifeLine);
			if (toLifeLineIt == _viewData.lifelines.end()) {
				DIAGEN_LOGE("Could not find lifeline for call to \"" + callItem.toLifeLine + "\"");
				continue;
			}

			// Recalculate line x positions
			const double destX = toLifeLineIt->second.group->getPosition().x() + toLifeLineIt->second.lifeLine->getPosition().x() - (_viewData.lifeLineProcessWidth / 2.);
			
			callItem.line->setPosition(ot::Point2DD(startX, callItem.line->getPosition().y()));
			callItem.line->setTo(ot::Point2DD(destX - startX, 0.));
			callItem.text->setPosition(ot::Point2DD(
				startX + _viewData.callTextMargin + (_viewData.callArrowHeadSize.width() / 2.), 
				callItem.text->getPosition().y()
			));
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

void DiagramGenerator::generateNewCallItem(SequenceLifeLineItem& _from, SequenceLifeLineItem& _to, SequenceViewData& _viewData, const QString& _callText, const CallFlags& _flags) {
	using namespace ot;

	// Calculate y position
	double callY = _viewData.callVStart + (_viewData.callVSeparation * static_cast<double>(_viewData.currentCallIndex));
	_viewData.currentCallIndex++;

	std::string callName = "Call_" + std::to_string(_viewData.currentCallIndex);

	// Create call line
	SequenceCallItem callItem;
	callItem.flags = _flags;

	callItem.text = new GraphicsTextItemCfg;
	callItem.text->setName(callName + "_Text");
	callItem.text->setTextFont(_viewData.callFont);
	callItem.text->setText(std::to_string(_viewData.currentCallIndex) + ": " + (_callText.isEmpty() ? "-" : _callText.toStdString()));
	callItem.text->setPosition(Point2DD(0., callY));
	callItem.text->setZValue(_viewData.callZValue);
	callItem.text->setTextPainter(new StyleRefPainter2D(ColorStyleValueEntry::SequenceMessageText));

	callY += (_viewData.callTextLineSpacing + _viewData.callTextHeight);

	callItem.line = new GraphicsDecoratedLineItemCfg;
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

bool DiagramGenerator::filterKeyValue(const QString& _filePath, size_t _lineCounter, const QString& _command, QString& _cmd, QString& _key, QString& _value) {
	qsizetype ix = _cmd.indexOf('=');
	if (ix < 0) {
		return false;
	}
	_key = _cmd.left(ix).trimmed().toLower();
	_cmd = _cmd.mid(ix + 1).trimmed();
	if (_cmd.isEmpty()) {
		DIAGEN_PLOGW(_filePath, _lineCounter, "No value for key \"" + _key + "\" while parsing \"" + _command + "\" command");
		return false;
	}
	if (_cmd.front() != '\"') {
		DIAGEN_PLOGW(_filePath, _lineCounter, "Quotation marks missing for value of key \"" + _key + "\" while parsing \"" + _command + "\" command");
		return false;
	}
	_cmd.remove(0, 1); // remove leading quote
	ix = _cmd.indexOf('\"');
	if (ix < 0) {
		DIAGEN_PLOGW(_filePath, _lineCounter, "Closing quotation mark missing for value of key \"" + _key + "\" while parsing \"" + _command + "\" command");
		return false;
	}
	_value = _cmd.left(ix);
	_cmd = _cmd.mid(ix + 1).trimmed();

	return true;
}
