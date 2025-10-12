//! @file GlobalShortcutWrapper.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtGui/qkeysequence.h>

// std header
#include <map>

class QShortcut;

namespace ot {

	class OT_WIDGETS_API_EXPORT GlobalShortcutWrapper : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(GlobalShortcutWrapper)
		OT_DECL_NOMOVE(GlobalShortcutWrapper)
		OT_DECL_NODEFAULT(GlobalShortcutWrapper)
	public:
		GlobalShortcutWrapper(const QString& _keySequence);
		~GlobalShortcutWrapper();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		QString keySequenceFromShortcut(const QShortcut* _shortcut) const;

		QShortcut* getShortcut(void) const { return m_shortcut; };
		const QKeySequence& getKeySequence(void) const { return m_keySequence; };

	Q_SIGNALS:
		void shortcutActivated();
		void shortcutActivatedAmbiguously();

	protected:
		bool eventFilter(QObject* _object, QEvent* _event) override;

	private Q_SLOTS:
		void slotShortcutActivated(void);
		void slotShortcutActivatedAmbiguously(void);

	private:
		QShortcut* m_shortcut;
		QKeySequence m_keySequence;

	};

}