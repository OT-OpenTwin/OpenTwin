// @otlicense
// File: UIQueueableRequest.h
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

// OpenTwin header
#include "OTCore/BasicQueue/BasicQueueObject.h"

class UIQueueableRequest : public ot::BasicQueueObject
{
	OT_DECL_NOCOPY(UIQueueableRequest)
	OT_DECL_NOMOVE(UIQueueableRequest)
	OT_DECL_NODEFAULT(UIQueueableRequest)
public:
	virtual ~UIQueueableRequest() = default;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Mandatory methods

	//! @brief Returns the class name of the object.
	virtual std::string getClassName() const override = 0;

	//! @brief Executes the object.
	//! This method will be called by the queue when the object is popped from the queue.
	virtual int exec() override = 0;

protected:
	static void makeRequest(UIQueueableRequest* _request);

	explicit UIQueueableRequest(ot::BasicQueueObject::InsertOrder _insertOrder);

private:

};