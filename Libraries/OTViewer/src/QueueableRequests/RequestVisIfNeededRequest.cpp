// @otlicense
// File: RequestVisIfNeededRequest.cpp
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
#include "OTViewer/Debugging/ViewerDebug.h"
#include "OTViewer/ViewerAPI.h"
#include "OTViewer/QueueableRequests/RequestVisIfNeededRequest.h"

void ot::RequestVisIfNeededRequest::makeRequest(UID _viewerID, UID _entityID, const std::string& _uniqueSuffix)
{
	QueueableViewerRequest::runOrQueueRequest(new RequestVisIfNeededRequest(_viewerID, _entityID, _uniqueSuffix));
}

ot::RequestVisIfNeededRequest::RequestVisIfNeededRequest(UID _viewerID, UID _entityID, const std::string& _uniqueSuffix)
	: QueueableViewerRequest(InsertOrder::InsertBack), m_viewerID(_viewerID), m_entityID(_entityID)
{
	if (!_uniqueSuffix.empty())
	{
		this->makeQueueUnique(this->getQueueObjectKey() + _uniqueSuffix, UniqueInsertBahavior::RemoveExisting);
	}
}

int ot::RequestVisIfNeededRequest::exec()
{
	bool result = ViewerAPI::requestVisualizationIfNeeded(m_viewerID, m_entityID);
	OT_VIEWER_VIEWSEL_DBG_PTR(this, ": Requesting visualization of entity " << m_entityID << 
		" in viewer " << m_viewerID << " returned " << (result ? "true" : "false"));
	return result ? AppExitCode::Success : AppExitCode::ResultFalse;
}
