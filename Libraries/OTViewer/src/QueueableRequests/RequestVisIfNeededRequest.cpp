// @otlicense

// OpenTwin header
#include "OTViewer/Intern/ViewerDebug.h"
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
