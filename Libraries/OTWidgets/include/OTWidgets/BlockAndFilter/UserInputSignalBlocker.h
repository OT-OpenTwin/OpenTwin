// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <unordered_set>

namespace ot
{

	class OT_WIDGETS_API_EXPORT UserInputSignalBlocker : public QObject
	{
		OT_DECL_NOCOPY(UserInputSignalBlocker)
		OT_DECL_NOMOVE(UserInputSignalBlocker)
	public:
		UserInputSignalBlocker(QObject* _parent = (QObject*)nullptr);
		virtual ~UserInputSignalBlocker() = default;

		virtual bool eventFilter(QObject* _watched, QEvent* _event) override;

		void addToWhitelist(QObject* _object) { m_whitelist.insert(_object); };
		void removeFromWhitelist(QObject* _object) { m_whitelist.erase(_object); };
		bool isWhitelisted(QObject* _object) const { return m_whitelist.find(_object) != m_whitelist.end(); };

		static bool isUserInputEvent(QEvent* _event);

	private:
		std::unordered_set<QObject*> m_whitelist;
	};

}