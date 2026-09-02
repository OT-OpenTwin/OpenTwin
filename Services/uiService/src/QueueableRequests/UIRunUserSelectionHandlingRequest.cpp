// @otlicense

// OpenTwin header
#include "AppBase.h"
#include "QueueableRequests/UIRunUserSelectionHandlingRequest.h"

void UIRunUserSelectionHandlingRequest::makeRequest(bool _unique, ot::BasicQueueObject::InsertOrder _insertOrder)
{
	UIRunUserSelectionHandlingRequest* request(new UIRunUserSelectionHandlingRequest(_insertOrder));
	if (_unique)
	{
		request->makeQueueUnique(ot::BasicQueueObject::UniqueInsertBahavior::RemoveExisting);
	}
	UIQueueableRequest::makeRequest(request);
}

int UIRunUserSelectionHandlingRequest::exec()
{
	AppBase::instance()->runSelectionHandling(ot::SelectionOrigin::User);
	return 0;
}

UIRunUserSelectionHandlingRequest::UIRunUserSelectionHandlingRequest(ot::BasicQueueObject::InsertOrder _insertOrder)
	: UIQueueableRequest(_insertOrder)
{}