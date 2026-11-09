// @otlicense
// File: QueueableViewerRequest.h
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
#include "OTViewer/ViewerAPIExport.h"

namespace ot
{

	class OT_VIEWER_API_EXPORT QueueableViewerRequest : public BasicQueueObject
	{
	public:
		QueueableViewerRequest(InsertOrder _insertOrder = InsertOrder::InsertBack);
		virtual ~QueueableViewerRequest();
		
		//! @brief Runs the request or queues it for later execution depending on the current frontend state.
		static void runOrQueueRequest(QueueableViewerRequest* _request);

		// ###########################################################################################################################################################################################################################################################################################################################
		
		// Mandatory methods
		
		virtual std::string getClassName() const override = 0;
		virtual int exec() override = 0;
		
		// ###########################################################################################################################################################################################################################################################################################################################
		
		// Queue handling
		
		virtual void stopQueueObjectExecution() override {};
	};

}