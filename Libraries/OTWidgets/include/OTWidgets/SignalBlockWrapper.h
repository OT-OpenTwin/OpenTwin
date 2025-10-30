// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot {
	
	//! @brief The SignalBlockWrapper is a RAII wrapper for blocking signals of a given QObject.
	class OT_WIDGETS_API_EXPORT SignalBlockWrapper {
		OT_DECL_NOCOPY(SignalBlockWrapper)
		OT_DECL_NODEFAULT(SignalBlockWrapper)
	public:
		//! @brief Constructor.
		//! The currently set "block signals" state of the given object will be stored and the signals will be blocked.
		SignalBlockWrapper(QObject* _object) : m_object(_object), m_initialState(false) {
			if (m_object) {
				m_initialState = m_object->signalsBlocked();
				m_object->blockSignals(true);
			}
		}

		//! @brief Destructor.
		//! The "blocked signals" state of the provided object will be set back to the initial value, any previous changes will be ignored.
		~SignalBlockWrapper() {
			if (m_object) {
				m_object->blockSignals(m_initialState);
			}
		}
	private:
		QObject* m_object;
		bool m_initialState;
	};

}