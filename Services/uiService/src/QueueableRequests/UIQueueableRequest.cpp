// @otlicense

// OpenTwin header
#include "AppBase.h"
#include "QueueableRequests/UIQueueableRequest.h"

void UIQueueableRequest::makeRequest(UIQueueableRequest* _request)
{
	AppBase::instance()->getQueueableObjectHandler().runOrQueue(_request);
}

UIQueueableRequest::UIQueueableRequest(ot::BasicQueueObject::InsertOrder _insertOrder)
	: ot::BasicQueueObject(_insertOrder)
{}
