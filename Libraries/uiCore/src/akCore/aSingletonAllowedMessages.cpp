// @otlicense
// File: aSingletonAllowedMessages.cpp
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

// AK header
#include <akCore/aSingletonAllowedMessages.h>

ak::aSingletonAllowedMessages * ak::aSingletonAllowedMessages::m_singleton = nullptr;

ak::aSingletonAllowedMessages::aSingletonAllowedMessages(void)
	: m_unknownEvent(true),
	m_activated(true),
	m_changed(true),
	m_cleared(true),
	m_clicked(true),
	m_collpased(true),
	m_cursorPosotionChanged(true),
	m_destroyed(true),
	m_doubleClicked(true),
	m_expanded(true),
	m_focused(true),
	m_invalidEntry(true),
	m_indexChanged(true),
	m_locationChanged(true),
	m_released(true),
	m_keyPressed(true),
	m_selectionChanged(true),
	m_stateChanged(true),
	m_textChanged(true),
	m_toggeledChecked(true),
	m_toggeledUnchecked(true)
{}

ak::aSingletonAllowedMessages * ak::aSingletonAllowedMessages::instance(void) {
	if (m_singleton == nullptr) {
		m_singleton = new aSingletonAllowedMessages;
	}
	return m_singleton;
}

void ak::aSingletonAllowedMessages::setFlag(eventType _type) {
	if (_type & etUnknownEvent) {
		m_unknownEvent = true;
	}
	if (_type & etActivated) {
		m_activated = true;
	}
	if (_type & etChanged) {
		m_changed = true;
	}
	if (_type & etCleared) {
		m_cleared = true;
	}
	if (_type & etClicked) {
		m_clicked = true;
	}
	if (_type & etClosing) {
		m_closeRequested = true;
	}
	if (_type & etCollpased) {
		m_collpased = true;
	}
	if (_type & etCursorPosotionChanged) {
		m_cursorPosotionChanged = true;
	}
	if (_type & etDestroyed) {
		m_destroyed = true;
	}
	if (_type & etDoubleClicked) {
		m_doubleClicked = true;
	}
	if (_type & etExpanded) {
		m_expanded = true;
	}
	if (_type & etFocused) {
		m_focused = true;
	}
	if (_type & etInvalidEntry) {
		m_invalidEntry = true;
	}
	if (_type & etIndexChanged) {
		m_indexChanged = true;
	}
	if (_type & etLocationChanged) {
		m_locationChanged = true;
	}
	if (_type & etKeyPressed) {
		m_keyPressed = true;
	}
	if (_type & etSelectionChanged) {
		m_selectionChanged = true;
	}
	if (_type & etStateChanged) {
		m_stateChanged = true;
	}
	/*if (_type & etTextChanged) {
		m_textChanged = true;
	}*/
	if (_type & etToggeledChecked) {
		m_toggeledChecked = true;
	}
	if (_type & etToggeledUnchecked) {
		m_toggeledUnchecked = true;
	}
}

void ak::aSingletonAllowedMessages::removeFlag(
	eventType			_type
) {
	if (_type & etUnknownEvent) {
		m_unknownEvent = false;
	}
	if (_type & etActivated) {
		m_activated = false;
	}
	if (_type & etChanged) {
		m_changed = false;
	}
	if (_type & etCleared) {
		m_cleared = false;
	}
	if (_type & etClicked) {
		m_clicked = false;
	}
	if (_type & etClosing) {
		m_closeRequested = false;
	}
	if (_type & etCollpased) {
		m_collpased = false;
	}
	if (_type & etCursorPosotionChanged) {
		m_cursorPosotionChanged = false;
	}
	if (_type & etDestroyed) {
		m_destroyed = false;
	}
	if (_type & etDoubleClicked) {
		m_doubleClicked = false;
	}
	if (_type & etExpanded) {
		m_expanded = false;
	}
	if (_type & etFocused) {
		m_focused = false;
	}
	if (_type & etInvalidEntry) {
		m_invalidEntry = false;
	}
	if (_type & etIndexChanged) {
		m_indexChanged = false;
	}
	if (_type & etLocationChanged) {
		m_locationChanged = false;
	}
	if (_type & etKeyPressed) {
		m_keyPressed = false;
	}
	if (_type & etSelectionChanged) {
		m_selectionChanged = false;
	}
	if (_type & etStateChanged) {
		m_stateChanged = false;
	}
	/*if (_type & etTextChanged) {
		m_textChanged = false;
	}*/
	if (_type & etToggeledChecked) {
		m_toggeledChecked = false;
	}
	if (_type & etToggeledUnchecked) {
		m_toggeledUnchecked = false;
	}
}

std::vector<ak::eventType> ak::aSingletonAllowedMessages::enabledMessages(void) const {
	std::vector<eventType> ret;
	if (m_activated) { ret.push_back(etActivated); }
	if (m_changed) { ret.push_back(etChanged); }
	if (m_cleared) { ret.push_back(etCleared); }
	if (m_clicked) { ret.push_back(etClicked); }
	if (m_closeRequested) { ret.push_back(etClosing); }
	if (m_collpased) { ret.push_back(etCollpased); }
	if (m_cursorPosotionChanged) { ret.push_back(etCursorPosotionChanged); }
	if (m_destroyed) { ret.push_back(etDestroyed); }
	if (m_doubleClicked) { ret.push_back(etDoubleClicked); }
	if (m_expanded) { ret.push_back(etExpanded); }
	if (m_focused) { ret.push_back(etFocused); }
	if (m_indexChanged) { ret.push_back(etIndexChanged); }
	if (m_invalidEntry) { ret.push_back(etInvalidEntry); }
	if (m_keyPressed) { ret.push_back(etKeyPressed); }
	if (m_locationChanged) { ret.push_back(etLocationChanged); }
	if (m_selectionChanged) { ret.push_back(etSelectionChanged); }
	if (m_stateChanged) { ret.push_back(etStateChanged); }
	if (m_toggeledChecked) { ret.push_back(etToggeledChecked); }
	if (m_toggeledUnchecked) { ret.push_back(etToggeledUnchecked); }
	if (m_unknownEvent) { ret.push_back(etUnknownEvent); }
	return ret;
}

std::vector<ak::eventType> ak::aSingletonAllowedMessages::disabledMessages(void) const {
	std::vector<eventType> ret;
	if (!m_activated) { ret.push_back(etActivated); }
	if (!m_changed) { ret.push_back(etChanged); }
	if (!m_cleared) { ret.push_back(etCleared); }
	if (!m_clicked) { ret.push_back(etClicked); }
	if (!m_closeRequested) { ret.push_back(etClosing); }
	if (!m_collpased) { ret.push_back(etCollpased); }
	if (!m_cursorPosotionChanged) { ret.push_back(etCursorPosotionChanged); }
	if (!m_destroyed) { ret.push_back(etDestroyed); }
	if (!m_doubleClicked) { ret.push_back(etDoubleClicked); }
	if (!m_expanded) { ret.push_back(etExpanded); }
	if (!m_focused) { ret.push_back(etFocused); }
	if (!m_indexChanged) { ret.push_back(etIndexChanged); }
	if (!m_invalidEntry) { ret.push_back(etInvalidEntry); }
	if (!m_keyPressed) { ret.push_back(etKeyPressed); }
	if (!m_locationChanged) { ret.push_back(etLocationChanged); }
	if (!m_selectionChanged) { ret.push_back(etSelectionChanged); }
	if (!m_stateChanged) { ret.push_back(etStateChanged); }
	if (!m_toggeledChecked) { ret.push_back(etToggeledChecked); }
	if (!m_toggeledUnchecked) { ret.push_back(etToggeledUnchecked); }
	if (!m_unknownEvent) { ret.push_back(etUnknownEvent); }
	return ret;
}
