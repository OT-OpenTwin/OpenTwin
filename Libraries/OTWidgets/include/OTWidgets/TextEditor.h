//! @file TextEditor.h
//! @author Alexander Kuester (alexk95)
//! @date December 2023
// ###########################################################################################################################################################################################################################################################################################################################

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
	class TextEditor;
	class SyntaxHighlighter;
	class TextEditorSearchPopup;
	
	class OT_WIDGETS_API_EXPORT TextEditorLineNumberArea : public QWidget {
	public:
		TextEditorLineNumberArea(TextEditor* _editor);
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
	public:
		TextEditor(QWidget* _parent = (QWidget*)nullptr);
		virtual ~TextEditor();

		//! \brief Setup the TextEditor.
		//! \param _config The config.
		//! \param _isUpdate If true the text, name and title won't be set.
		virtual void setupFromConfig(const TextEditorCfg& _config, bool _isUpdate);

		int lineNumberAreaWidth() const;
		void lineNumberAreaPaintEvent(QPaintEvent * _event);

		void setContentChanged();
		void setContentSaved();
		bool getContentChanged() const;

		void setCode(const QString& _text);
		void setCode(const QStringList& _lines);

		bool saveToFile(const QString& _fileName);

		QStringList code() const;

		//! \brief Returns the current syntax highlighter.
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

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void resizeEvent(QResizeEvent * _event) override;
		virtual void wheelEvent(QWheelEvent* _event) override;
		
	private Q_SLOTS:
		void slotUpdateLineNumberAreaWidth(int _newBlockCount);
		void slotHighlightCurrentLine();
		void slotUpdateLineNumberArea(const QRect & _rect, int _dy);
		void slotFindRequested();
		void slotFindClosing();
		void slotDuplicateLine();
		void slotCurrentColorStyleChanged();
		void slotSelectionChanged();

	private:
		friend class TextEditorSearchPopup;

		void getCurrentLineSelection(QList<QTextEdit::ExtraSelection>& _selections);
		void addAdditionalSelections(QList<QTextEdit::ExtraSelection>& _selections);
		void updateDocumentSyntax();

		TextEditorSearchPopup* m_searchPopup;
		
		int m_tabSpaces;
		bool m_newLineSamePrefix;
		bool m_enableDuplicateLineShortcut;
		bool m_enableSameTextHighlighting;
		int m_sameTextHighlightingMinimum;

		TextEditorLineNumberArea* m_lineNumberArea;
		DocumentSyntax            m_documentSyntax;
		SyntaxHighlighter*		  m_syntaxHighlighter;

		std::string m_fileExtensionFilter;
	};
}
