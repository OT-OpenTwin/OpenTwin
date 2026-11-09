// @otlicense
// File: RequestVisIfNeededRequest.h
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
#include "OTViewer/QueueableRequests/QueueableViewerRequest.h"

namespace ot
{

	class OT_VIEWER_API_EXPORT RequestVisIfNeededRequest : public QueueableViewerRequest
	{
		OT_DECL_NOCOPY(RequestVisIfNeededRequest)
		OT_DECL_NOMOVE(RequestVisIfNeededRequest)
		OT_DECL_NODEFAULT(RequestVisIfNeededRequest)
	public:
		//! @brief Requests the visualization of an entity if it is not already visualized.
		//! @param _viewerID The ID of the viewer where the visualization should be requested.
		//! @param _entityID The ID of the entity for which the visualization should be requested.
		//! @param _uniqueSuffix If specified the request will be marked unique for the given class name + provided suffix.
		static void makeRequest(UID _viewerID, UID _entityID, const std::string& _uniqueSuffix = "");

		static std::string className() { return "RequestVisIfNeededRequest"; }
		virtual std::string getClassName() const override { return RequestVisIfNeededRequest::className(); };
		virtual int exec() override;

	private:
		RequestVisIfNeededRequest(UID _viewerID, UID _entityID, const std::string& _uniqueSuffix);
		virtual ~RequestVisIfNeededRequest() = default;
		
	private:
		UID m_viewerID;
		UID m_entityID;

	};

}