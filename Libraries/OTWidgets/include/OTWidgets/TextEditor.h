// @otlicense
// File: TextEditor.h
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

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTGui/TextEditorCfg.h"
#include "OTWidgets/PlainTextEdit.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qregularexpression.h>

class QPaintEvent;
class QResizeEvent;

namespace ot {

	class Label;
	class TextEditor;
	class SyntaxHighlighter;
	class TextEditorSearchPopup;
	
	class OT_WIDGETS_API_EXPORT TextEditorLineNumberArea : public QWidget {
		OT_DECL_NOCOPY(TextEditorLineNumberArea)
		OT_DECL_NOMOVE(TextEditorLineNumberArea)
		OT_DECL_NODEFAULT(TextEditorLineNumberArea)
	public:
		explicit TextEditorLineNumberArea(TextEditor* _editor);
		virtual ~TextEditorLineNumberArea();

		QSize sizeHint() const override;

	protected:
		void paintEvent(QPaintEvent * _event) override;

	private:
		TextEditor*	m_editor;
	};

	// ###################################################################################################################################

	// ###################################################################################################################################

	// ###################################################################################################################################

	class OT_WIDGETS_API_EXPORT TextEditor : public PlainTextEdit {
		Q_OBJECT
		OT_DECL_NOCOPY(TextEditor)
		OT_DECL_NOMOVE(TextEditor)
		OT_DECL_NODEFAULT(TextEditor)
	public:
		explicit TextEditor(QWidget* _parent);
		virtual ~TextEditor();

		//! @brief Setup the TextEditor.
		//! @param _config The config.
		//! @param _replaceText If true the text will be replaced.
		virtual void setupFromConfig(const TextEditorCfg& _config, bool _replaceText = true);

		int lineNumberAreaWidth() const;
		void lineNumberAreaPaintEvent(QPaintEvent * _event);

		void setContentChanged();
		void setContentSaved();
		bool getContentChanged() const;

		void setPlainText(const QString& _text);

		bool saveToFile(const QString& _fileName);

		QString toPlainText() const;

		//! @brief Returns the current syntax highlighter.
		//! The TextEditor keeps ownership of the highlighter.
		SyntaxHighlighter* getSyntaxHighlighter() { return m_syntaxHighlighter; };

		void setTabSpaces(int _spaces) { m_tabSpaces = _spaces; };
		int tabSpaces() const { return m_tabSpaces; };

		void setNewLineWithSamePrefix(bool _enabled) { m_newLineSamePrefix = _enabled; };
		bool newLineWithSamePrefix() const { return m_newLineSamePrefix; };

		void setDuplicateLineShortcutEnabled(bool _enabled) { m_enableDuplicateLineShortcut = _enabled; };
		bool isDuplicateLineShortcutEnabled() const { return m_enableDuplicateLineShortcut; };

		void setEnableSameTextHighlighting(bool _enabled) { m_enableSameTextHighlighting = _enabled; };
		bool isSameTextHighlightingEnabled() const { return m_enableSameTextHighlighting; };

		void setFileExtensionFilter(const std::list<FileExtension::DefaultFileExtension>& _extensions);
		void setFileExtensionFilter(const std::string& _filter) { m_fileExtensionFilter = _filter; };
		const std::string& getFileExtensionFilter() const { return m_fileExtensionFilter; };

	Q_SIGNALS:							
		void saveRequested();

	public Q_SLOTS:
		void slotSaveRequested();

		// ###################################################################################################################################

		// Protected: Event handling

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void resizeEvent(QResizeEvent * _event) override;
		virtual void wheelEvent(QWheelEvent* _event) override;
		virtual void showEvent(QShowEvent* _event) override;
		
		// ###################################################################################################################################

		// Private: Slots

	private Q_SLOTS:
		void slotUpdateLineNumberAreaWidth(int _newBlockCount);
		void slotHighlightCurrentLine();
		void slotUpdateLineNumberArea(const QRect & _rect, int _dy);
		void slotFindRequested();
		void slotFindClosing();
		void slotDuplicateLine();
		void slotCurrentColorStyleChanged();
		void slotSelectionChanged();
		void slotShowMore();
		void slotUpdateShowMorePosition();

		// ###################################################################################################################################

		// Private: Helper

	private:
		friend class TextEditorSearchPopup;

		void getCurrentLineSelection(QList<QTextEdit::ExtraSelection>& _selections);
		void addAdditionalSelections(QList<QTextEdit::ExtraSelection>& _selections);
		void updateDocumentSyntax();

		void splitTextBufferIntoChunks();

		TextEditorSearchPopup* m_searchPopup;
		
		int m_tabSpaces;
		bool m_newLineSamePrefix;
		bool m_enableDuplicateLineShortcut;
		bool m_enableSameTextHighlighting;
		int m_sameTextHighlightingMinimum;

		std::string m_completeText = "";
		const uint32_t m_maxOfDisplayedChars = 40000;

		TextEditorLineNumberArea* m_lineNumberArea;
		DocumentSyntax            m_documentSyntax;
		SyntaxHighlighter*		  m_syntaxHighlighter;

		std::string m_fileExtensionFilter;

		Label* m_showMoreLabel;

		QString m_textBuffer;
		int m_loadedChunkCount;
		QVector<QStringView> m_textChunks;
	};
}
