/*
 * debugNotifier.cpp
 *
 *  Created on: September 21, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#include <debugNotifier.h>		// Corresponding header

#include <akAPI/uiAPI.h>
#include <akCore/aException.h>

debugNotifier::debugNotifier(
	ak::UID			_output
) : my_output(_output) {}

debugNotifier::~debugNotifier() {}

void debugNotifier::notify(
	ak::UID									_senderId,
	ak::eventType							_event,
	int										_info1,
	int										_info2
) {
	try {
		if (_senderId != my_output) {
			ak::uiAPI::textEdit::appendText(my_output, ak::uiAPI::special::toEventText(
				_senderId, _event, _info1, _info2));
		}
	}
	catch (const ak::aException & e) { throw ak::aException(e, "debugNotifier::notify()"); }
	catch (const std::exception & e) { throw ak::aException(e.what(), "debugNotifier::notify()"); }
	catch (...) { throw ak::aException("Unknown error", "debugNotifier::notify()"); }
}

void debugNotifier::setOutputUid(
	ak::UID					_outputUid
) { my_output = _outputUid; }