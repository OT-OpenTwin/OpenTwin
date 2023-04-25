/*
 * debugNotifier.h
 *
 *  Created on: September 28, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#pragma once

// AK header
#include <akCore/aNotifier.h>

class debugNotifier : public ak::aNotifier {
public:
	debugNotifier(
		ak::UID			_output
	);

	virtual ~debugNotifier();

	//! @brief Will call the callback function with the provided parameters
	//! @param _senderId The sender ID the message was send from
	//! @param _event The event message
	//! @param _info1 Message addition 1
	//! @param _info2 Message addition 2
	//! @throw sim::Exception to forward exceptions coming from the application core class
	virtual void notify(
		ak::UID					_senderId,
		ak::eventType			_event,
		int						_info1,
		int						_info2
	) override;

	void setOutputUid(
		ak::UID					_outputUid
	);

private:
	ak::UID			my_output;
};

