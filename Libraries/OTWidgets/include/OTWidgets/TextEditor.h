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

		OT_PROPERTY(bool, contentChanged, setContentChanged, contentChanged)
		OT_PROPERTY_REF(std::string, textEditorName, setTextEditorName, textEditorName)
		OT_PROPERTY_REF(QString, textEditorTitle, setTextEditorTitle, textEditorTitle)
	public:
		TextEditor(QWidget* _parent = (QWidget*)nullptr);
		virtual ~TextEditor();

		int lineNumberAreaWidth(void) const;
		void lineNumberAreaPaintEvent(QPaintEvent * _event);

		void setCode(const QString& _text);
		void setCode(const QStringList& _lines);

		QStringList code(void) const;

		void setSyntaxHighlighter(QSyntaxHighlighter * _highlighter);
		QSyntaxHighlighter * syntaxHighlighter(void) { return m_syntaxHighlighter; }

		bool requiresRefreshing(ot::UID displayedTextEntityID, ot::UID displayedTextEntityVersion);
	signals:							
		void saveRequested(void);

	protected:
		virtual void resizeEvent(QResizeEvent * _event) override;

	private slots:
		void slotUpdateLineNumberAreaWidth(int _newBlockCount);
		void slotHighlightCurrentLine();
		void slotUpdateLineNumberArea(const QRect & _rect, int _dy);
		void slotSaveRequested(void);
		void slotTextChanged(void);

	private:
		ot::UID								m_displayedTextEntityID = 0;
		ot::UID								m_displayedTextEntityVersion = 0;

		QShortcut* m_saveShortcut;
		TextEditorLineNumberArea*			m_lineNumberArea;
		QSyntaxHighlighter *				m_syntaxHighlighter;
	};
}