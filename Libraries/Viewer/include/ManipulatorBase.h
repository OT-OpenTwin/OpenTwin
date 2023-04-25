#pragma once

#include "OpenTwinCore/CoreTypes.h"

#include <string>
#include <list>

class ManipulatorBase
{
public:
	ManipulatorBase() : messageReplyTo(ot::invalidServiceID) {};
	virtual ~ManipulatorBase() {};

	virtual void cancelOperation(void) = 0;
	virtual void performOperation(void) = 0;

	virtual bool propertyGridValueChanged(int itemID) { return false; }

	void setReplyTo(ot::serviceID_t replyTo) { messageReplyTo = replyTo; }
	void setAction(const std::string &selectionAction) { messageSelectionAction = selectionAction; }
	void setOptionNames(const std::list<std::string> &optionNames) { messageOptionNames = optionNames; }
	void setOptionValues(const std::list<std::string> &optionValues) { messageOptionValues = optionValues; }

protected:
	ot::serviceID_t messageReplyTo;
	std::string messageSelectionAction;
	std::list<std::string> messageOptionNames;
	std::list<std::string> messageOptionValues;
};

