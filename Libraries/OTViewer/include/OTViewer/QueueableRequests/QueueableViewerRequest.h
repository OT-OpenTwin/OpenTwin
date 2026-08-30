// @otlicense

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