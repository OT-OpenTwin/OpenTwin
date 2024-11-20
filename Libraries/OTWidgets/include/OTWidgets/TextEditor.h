//! @file TextEditor.h
//! @author Alexander Kuester (alexk95)
//! @date December 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTGui/TextEditorCfg.h"
#include "OTWidgets/ColorStyle.h"
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

		int lineNumberAreaWidth(void) const;
		void lineNumberAreaPaintEvent(QPaintEvent * _event);

		void setContentChanged(bool _changed = true);
		bool getContentChanged(void) const { return m_contentChanged; };

		void setCode(const QString& _text);
		void setCode(const QStringList& _lines);

		QStringList code(void) const;

		//! \brief Stores the syntax highlighter.
		//! An existing syntax highlighter will be replaced.
		//! The TextEditor takes ownership of the highlighter.
		void storeSyntaxHighlighter(SyntaxHighlighter* _highlighter);

		//! \brief Returns the current syntax highlighter.
		//! The TextEditor keeps ownership of the highlighter.
		SyntaxHighlighter* getSyntaxHighlighter(void) { return m_syntaxHighlighter; };

		//! \brief Returns the current syntax highlighter and sets it to 0 internally.
		//! The caller takes ownership of the highlighter.
		SyntaxHighlighter* takeSyntaxHighlighter(void);

		bool requiresRefreshing(ot::UID displayedTextEntityID, ot::UID displayedTextEntityVersion);

		void setTabSpaces(int _spaces) { m_tabSpaces = _spaces; };
		int tabSpaces(void) const { return m_tabSpaces; };

		void setNewLineWithSamePrefix(bool _enabled) { m_newLineSamePrefix = _enabled; };
		bool newLineWithSamePrefix(void) const { return m_newLineSamePrefix; };

		void setDuplicateLineShortcutEnabled(bool _enabled) { m_enableDuplicateLineShortcut = _enabled; };
		bool isDuplicateLineShortcutEnabled(void) const { return m_enableDuplicateLineShortcut; };

		void setEnableSameTextHighlighting(bool _enabled) { m_enableSameTextHighlighting = _enabled; };
		bool isSameTextHighlightingEnabled(void) const { return m_enableSameTextHighlighting; };

	Q_SIGNALS:							
		void saveRequested(void);

	protected:
		virtual void contentSaved(void) {};
		virtual void contentChanged(void) {};
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void resizeEvent(QResizeEvent * _event) override;
		virtual void wheelEvent(QWheelEvent* _event) override;
		
	private Q_SLOTS:
		void slotUpdateLineNumberAreaWidth(int _newBlockCount);
		void slotHighlightCurrentLine();
		void slotUpdateLineNumberArea(const QRect & _rect, int _dy);
		void slotSaveRequested(void);
		void slotTextChanged(void);
		void slotFindRequested(void);
		void slotFindClosing(void);
		void slotDuplicateLine(void);
		void slotCurrentColorStyleChanged(const ot::ColorStyle& _style);
		void slotSelectionChanged(void);

	private:
		friend class TextEditorSearchPopup;

		void getCurrentLineSelection(QList<QTextEdit::ExtraSelection>& _selections);
		void addAdditionalSelections(QList<QTextEdit::ExtraSelection>& _selections);

		TextEditorSearchPopup* m_searchPopup;
		
		int m_lastSavedUndoStackCount;
		bool m_contentChanged;

		int m_tabSpaces;
		bool m_newLineSamePrefix;
		bool m_enableDuplicateLineShortcut;
		bool m_enableSameTextHighlighting;
		int m_sameTextHighlightingMinimum;

		ot::UID								m_displayedTextEntityID = 0;
		ot::UID								m_displayedTextEntityVersion = 0;

		TextEditorLineNumberArea*			m_lineNumberArea;
		SyntaxHighlighter*				m_syntaxHighlighter;
	};
}
