// @otlicense

#pragma once

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>

// C++ header
#include <vector>

namespace ak {
	
	class UICORE_API_EXPORT aSingletonAllowedMessages
	{
	public:
		
		aSingletonAllowedMessages(const aSingletonAllowedMessages &other) = delete;

		aSingletonAllowedMessages & operator=(const aSingletonAllowedMessages &) = delete;
		
		static aSingletonAllowedMessages * instance();

		// ###################################################################################

		// Setter

		void setFlag(eventType		_type);

		void removeFlag(eventType	_type);

		// ###################################################################################

		// Getter

		std::vector<eventType> enabledMessages(void) const;

		std::vector<ak::eventType> disabledMessages(void) const;

		bool unknownEvent(void) const { return m_unknownEvent; }

		bool activatedEvent(void) const { return m_activated; }

		bool changedEvent(void) const { return m_changed; }

		bool clearedEvent(void) const { return m_cleared; }

		bool clickedEvent(void) const { return m_clicked; }

		bool closeRequestedEvent(void) const { return m_closeRequested; }

		bool collapsedEvent(void) const { return m_collpased; }

		bool cursorPositionChangedEvent(void) const { return m_cursorPosotionChanged; }

		bool destroyedEvent(void) const { return m_destroyed; }

		bool doubleClickedEvent(void) const { return m_doubleClicked; }

		bool expandedEvent(void) const { return m_expanded; }

		bool focusedEvent(void) const { return m_focused; }

		bool invalidEntryEvent(void) const { return m_invalidEntry; }

		bool indexChangedEvent(void) const { return m_indexChanged; }

		bool keyPressedEvent(void) const { return m_keyPressed; }

		bool locationChangedEvent(void) const { return m_locationChanged; }

		bool releasedEvent(void) const { return m_released; }

		bool selectionChangedEvent(void) const { return m_selectionChanged; }

		bool stateChangedEvent(void) const { return m_stateChanged; }

		bool textChangedEvent(void) const { return m_textChanged; }

		bool toggledCheckedEvent(void) const { return m_toggeledChecked; }

		bool toggledUncheckedEvent(void) const { return m_toggeledUnchecked; }

	protected:
		aSingletonAllowedMessages();

		static aSingletonAllowedMessages *	m_singleton;

		bool						m_unknownEvent;
		bool						m_activated;
		bool						m_changed;
		bool						m_cleared;
		bool						m_clicked;
		bool						m_closeRequested;
		bool						m_collpased;
		bool						m_cursorPosotionChanged;
		bool						m_destroyed;
		bool						m_doubleClicked;
		bool						m_expanded;
		bool						m_focused;
		bool						m_invalidEntry;
		bool						m_indexChanged;
		bool						m_locationChanged;
		bool						m_released;
		bool						m_keyPressed;
		bool						m_selectionChanged;
		bool						m_stateChanged;
		bool						m_textChanged;
		bool						m_toggeledChecked;
		bool						m_toggeledUnchecked;
			
	};

}