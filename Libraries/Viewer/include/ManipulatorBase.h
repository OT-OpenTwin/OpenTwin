// @otlicense
// File: ManipulatorBase.h
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

#pragma once

#include "OTCore/CoreTypes.h"

#include <string>
#include <list>

namespace ot { class Property; };

class ManipulatorBase
{
public:
	ManipulatorBase() : messageReplyTo(ot::invalidServiceID) {};
	virtual ~ManipulatorBase() {};

	virtual void cancelOperation(void) = 0;
	virtual void performOperation(void) = 0;

	virtual bool propertyGridValueChanged(const ot::Property* _property) { return false; }

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

