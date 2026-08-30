// @otlicense

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