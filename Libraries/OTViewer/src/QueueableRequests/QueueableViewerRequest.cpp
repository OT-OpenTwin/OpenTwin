// @otlicense

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
