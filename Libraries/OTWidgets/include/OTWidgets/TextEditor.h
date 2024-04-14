//! @file TextEditor.h
//! @author Alexander Kuester (alexk95)
//! @date December 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PlainTextEdit.h"
#include "OTCore/CoreTypes.h"

// Qt header
#include <qstring.h>
#include <qstringlist.h>
#include <qsyntaxhighlighter.h>
#include <qregularexpression.h>

class QPaintEvent;
class QResizeEvent;
class QSyntaxHighlighter;

namespace ot {
	class TextEditor;
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

		int lineNumberAreaWidth(void) const;
		void lineNumberAreaPaintEvent(QPaintEvent * _event);

		void setContentChanged(bool _changed = true) { m_contentChanged = _changed; };
		bool contentChanged(void) const { return m_contentChanged; };

		void setTextEditorName(const std::string& _name) { m_textEditorName = _name; };
		const std::string& textEditorName(void) const { return m_textEditorName; };

		void setCode(const QString& _text);
		void setCode(const QStringList& _lines);

		QStringList code(void) const;

		void setSyntaxHighlighter(QSyntaxHighlighter * _highlighter);
		QSyntaxHighlighter * syntaxHighlighter(void) { return m_syntaxHighlighter; }

		bool requiresRefreshing(ot::UID displayedTextEntityID, ot::UID displayedTextEntityVersion);

		void setTabSpaces(int _spaces) { m_tabSpaces = _spaces; };
		int tabSpaces(void) const { return m_tabSpaces; };

	Q_SIGNALS:							
		void saveRequested(void);

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;

		virtual void resizeEvent(QResizeEvent * _event) override;

	private Q_SLOTS:
		void slotUpdateLineNumberAreaWidth(int _newBlockCount);
		void slotHighlightCurrentLine();
		void slotUpdateLineNumberArea(const QRect & _rect, int _dy);
		void slotSaveRequested(void);
		void slotTextChanged(void);
		void slotFindRequested(void);
		void slotFindClosing(void);

	private:
		TextEditorSearchPopup* m_searchPopup;

		bool m_contentChanged;
		std::string m_textEditorName;
		int m_tabSpaces;

		ot::UID								m_displayedTextEntityID = 0;
		ot::UID								m_displayedTextEntityVersion = 0;

		TextEditorLineNumberArea*			m_lineNumberArea;
		QSyntaxHighlighter *				m_syntaxHighlighter;
	};
}
