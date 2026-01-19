// @otlicense
// File: DiagramGenerator.h
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

#pragma once

// Toolkit API header
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/Rect.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/Font.h"
#include "OTGui/PenCfg.h"
#include "OTGui/Margins.h"
#include "OTGui/VersionGraphVersionCfg.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qregularexpression.h>

// std header
#include <list>

class QProgressBar;
class QTreeWidgetItem;

namespace ot { 
	class ComboBox;
	class TextEditor;
	class PushButton;
	class GraphicsView;
	class PathBrowseEdit;
	class TreeWidgetFilter;
	class GraphicsTextItemCfg;
	class GraphicsLineItemCfg;
	class GraphicsGroupItemCfg;
	class GraphicsRectangularItemCfg;
	class GraphicsDecoratedLineItemCfg;
}

class DiagramGenerator : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	DiagramGenerator() = default;
	virtual ~DiagramGenerator() = default;

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	virtual QString getToolName(void) const override { return QString("Diagram Generator"); };
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	virtual void restoreToolSettings(QSettings& _settings) override;
	virtual bool prepareToolShutdown(QSettings& _settings) override;

	// ###########################################################################################################################################################################################################################################################################################################################
	
	// Private: Slots

private Q_SLOTS:
	void slotRunParser();
	void slotParserFinished();
	void slotGenerateResult();
	void slotResultItemDoubleClicked(QTreeWidgetItem* _item, int _column);

private:
	enum class ParseSyntax {
		Cpp
	};

	enum class CallFlag {
		None        = 0,
		ReturnCall  = 1 << 0,
		ReverseCall = 1 << 1
	};
	typedef ot::Flags<CallFlag> CallFlags;
	OT_ADD_FRIEND_FLAG_FUNCTIONS(CallFlag, CallFlags)

	struct ParserData {
		OT_DECL_DEFCOPY(ParserData)
		OT_DECL_DEFMOVE(ParserData)
		ParserData() = default;
		~ParserData() = default;

		QString rootPath;
		ParseSyntax syntax = ParseSyntax::Cpp;
		QStringList fileExtensions;
		std::list<QRegularExpression> ignorePatterns;
	};

	enum class ParseFileState {
		Idle,
		InFunction
	};

	struct ParseFileData {
		OT_DECL_NOCOPY(ParseFileData)
		OT_DECL_NOMOVE(ParseFileData)
		ParseFileData() = default;
		~ParseFileData() = default;

		QString currentFunction;
		ParseFileState state = ParseFileState::Idle;
	};

	struct SequenceCall {
		OT_DECL_DEFCOPY(SequenceCall)
		OT_DECL_DEFMOVE(SequenceCall)
		SequenceCall() = default;
		~SequenceCall() = default;

		QString text;
		QString function;
	};

	struct SequenceReturn {
		OT_DECL_DEFCOPY(SequenceReturn)
		OT_DECL_DEFMOVE(SequenceReturn)
		SequenceReturn() = default;
		~SequenceReturn() = default;

		QString text;
	};

	struct SequenceFunction {
		OT_DECL_DEFCOPY(SequenceFunction)
		OT_DECL_DEFMOVE(SequenceFunction)
		SequenceFunction() = default;
		~SequenceFunction() = default;

		bool isValid() const { return !name.isEmpty() && !lifeLine.isEmpty(); }

		QString lifeLine;
		QString name;
		QString defaultText;
		QString filePath;
		
		std::list<SequenceCall> calls;
		SequenceReturn returnValue;
	};

	struct SequenceDiagram {
		OT_DECL_DEFCOPY(SequenceDiagram)
		OT_DECL_DEFMOVE(SequenceDiagram)
		SequenceDiagram() = default;
		~SequenceDiagram() = default;
		
		QString name;
		QString functionName;
	};

	struct ParserResult {
		OT_DECL_NOCOPY(ParserResult)
		OT_DECL_DEFMOVE(ParserResult)
		ParserResult() = default;
		~ParserResult() = default;

		size_t filesParsed = 0;
		size_t directoriesParsed = 0;
		size_t invalidFunctions = 0;
		size_t invalidDiagrams = 0;

		std::map<QString, SequenceFunction> sequenceFunctions;
		std::map<QString, SequenceDiagram> sequenceDiagrams;
	};

	struct SequenceCallItem {
		OT_DECL_NOCOPY(SequenceCallItem)
		OT_DECL_DEFMOVE(SequenceCallItem)
		SequenceCallItem() = default;
		~SequenceCallItem() = default;

		ot::GraphicsTextItemCfg* text = nullptr;
		ot::GraphicsDecoratedLineItemCfg* line = nullptr;

		QString fromLifeLine;
		QString toLifeLine;

		CallFlags flags = CallFlag::None;
	};

	struct SequenceLifeLineItem {
		OT_DECL_NOCOPY(SequenceLifeLineItem)
		OT_DECL_DEFMOVE(SequenceLifeLineItem)
		SequenceLifeLineItem() = default;
		~SequenceLifeLineItem() = default;

		QString name;
		ot::GraphicsGroupItemCfg* group = nullptr;
		ot::GraphicsTextItemCfg* text = nullptr;
		ot::GraphicsRectangularItemCfg* textBox = nullptr;
		ot::GraphicsLineItemCfg* lifeLine = nullptr;
		std::list<SequenceCallItem> calls;
		std::list<ot::GraphicsRectangularItemCfg*> processes;
	};

	struct SequenceViewData {
		OT_DECL_NOCOPY(SequenceViewData)
		OT_DECL_DEFMOVE(SequenceViewData)
		SequenceViewData() = default;
		~SequenceViewData() = default;

		ot::GraphicsView* view = nullptr;

		int32_t currentCallIndex = 0;
		double firstCallOffset = 20.;
		double callTextLineSpacing = 2.;
		double callTextMargin = 5.;
		ot::Size2DD callArrowHeadSize = ot::Size2DD(5., 8.);
		double callSpacing = 5.;
		double callVStart = 0.; // Calculated start position
		double callTextHeight = 0.; // Calculated height depending on font
		double callVSeparation = 0.; // Calculated vertical separation between calls

		double lifeLineSpacing = 30.;
		double lifeLineProcessWidth = 8.;
		ot::Font lifeLineFont = ot::Font("Consolas", 12);
		ot::MarginsD lifeLineTextMargins = ot::MarginsD(10., 5., 10., 5.);
		ot::PenFCfg lifeLinePen = ot::PenFCfg();
		std::map<QString, SequenceLifeLineItem> lifelines;

		ot::Font callFont = ot::Font("Consolas", 10);

		int32_t lifeLineZValue = 1;
		int32_t lifeLineTextBoxZValue = 2;
		int32_t lifeLineLineZValue = 2;
		int32_t lifeLineProcessZValue = 3;
		int32_t lifeLineAsyncProcessZValue = 4;
		int32_t lifeLineTextZValue = 5;

		int32_t callZValue = 10;
	};

	enum class ResultType : int {
		Container,
		SequenceDiagram,
		SequenceFunction
	};

	enum class DiagramRoleIx : int {
		Name = Qt::UserRole + 1
	};

	enum class FunctionRoleIx : int {
		Name = Qt::UserRole + 1
	};

	ParserResult m_parseResult;

	QProgressBar* m_progressBar = nullptr;

	ot::PathBrowseEdit* m_rootPathEdit = nullptr;
	ot::ComboBox* m_syntaxSelect = nullptr;
	ot::PushButton* m_runButton = nullptr;

	ot::TextEditor* m_ignoreEditor = nullptr;

	ot::PushButton* m_resultGenerateButton = nullptr;
	ot::TreeWidgetFilter* m_resultTree = nullptr;

	std::list<ot::WidgetView*> m_sequenceDiagramViews;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

	void lockControls(bool _lock);
	bool initializeParserData(ParserData& _data);
	QString globToRegex(const QString& _pattern);

	SequenceViewData initializeSequenceViewData(const QString& _name);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Worker

	void parserWorker(ParserData&& _data);
	void parseDirectory(const QString& _path, const ParserData& _data);
	void parseFile(const QString& _filePath, const ParserData& _data);
	
	void finalizeFunction(const QString& _filePath, size_t _lineCounter, SequenceFunction&& _function);
	void parseFunctionLine(const QString& _filePath, size_t _lineCounter, const QString& _line, int _prefixLen, SequenceFunction& _function);
	void parseCallLine(const QString& _filePath, size_t _lineCounter, const QString& _line, SequenceFunction& _function);
	void parseDiagramLine(const QString& _filePath, size_t _lineCounter, const QString& _line, int _prefixLen);

	void validateFunctions();
	bool validateFunction(const QString& _rootFunction, SequenceFunction& _function, std::set<QString>& _usedFunctions);
	void validateDiagrams();

	void generateSequenceDiagram(const SequenceDiagram& _diagram);
	void generateSequenceDiagram(const SequenceFunction& _function);
	void generateSequenceDiagram(SequenceViewData& _viewData, const SequenceFunction& _function);

	void generateSequenceDiagramLifeLines(const SequenceFunction& _function, SequenceViewData& _viewData);
	void generateSequenceDiagramCalls(const SequenceFunction& _function, SequenceViewData& _viewData);
	void repositionLifelines(SequenceViewData& _viewData);
	void repositionCalls(SequenceViewData& _viewData);
	void mergeProcessBoxes(SequenceViewData& _viewData);
	void generateItems(SequenceViewData& _viewData);
	void clear(SequenceViewData& _viewData);
	void clear(const SequenceCallItem& _callItem);

	SequenceLifeLineItem generateNewLifeLineItem(const QString& _name, const SequenceViewData& _viewData);

	//! @brief Generate a new call item between two lifelines.
	//! @return The vertical position of the call line item.
	double generateNewCallItem(SequenceLifeLineItem& _from, SequenceLifeLineItem& _to, SequenceViewData& _viewData, const QString& _callText, const CallFlags& _flags);

	ot::Point2DD calculateNextLifeLinePosition(const SequenceViewData& _viewData);
	ot::RectD calculateLifeLineTextBoxRect(const QString& _text, const SequenceViewData& _viewData, const ot::Point2DD& _position);

	double calculateLifelineDistance(const SequenceLifeLineItem& _left, const SequenceLifeLineItem& _right, const SequenceViewData& _viewData);

	ot::GraphicsRectangularItemCfg* createProcessBox(SequenceLifeLineItem& _lifeLine, double _startYPos, double _endYPos, const SequenceViewData& _viewData);

	bool filterKeyValue(const QString& _filePath, size_t _lineCounter, const QString& _command, QString& _cmd, QString& _key, QString& _value);

};
