// @otlicense
// File: QueueableViewerRequest.cpp
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

// OpenTwin header
#include "OTViewer/ViewerAPI.h"
#include "OTViewer/FrontendAPI.h"
#include "OTViewer/QueueableRequests/QueueableViewerRequest.h"

ot::QueueableViewerRequest::QueueableViewerRequest(InsertOrder _insertOrder)
	: BasicQueueObject(_insertOrder)
{}

ot::QueueableViewerRequest::~QueueableViewerRequest()
{}

void ot::QueueableViewerRequest::runOrQueueRequest(QueueableViewerRequest * _request)
{
	FrontendAPI* api = FrontendAPI::instance();
	if (api) {
		api->runOrQueueRequest(_request);
	}
	else {
		OT_LOG_E("FrontendAPI instance not set. Request will be deleted.");
		delete _request;
	}
}
