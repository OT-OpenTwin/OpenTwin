// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/BasicQueue/BasicQueueObject.h"

class UIQueueableRequest : public ot::BasicQueueObject
{
	OT_DECL_NOCOPY(UIQueueableRequest)
	OT_DECL_NOMOVE(UIQueueableRequest)
	OT_DECL_NODEFAULT(UIQueueableRequest)
public:
	virtual ~UIQueueableRequest() = default;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Mandatory methods

	//! @brief Returns the class name of the object.
	virtual std::string getClassName() const override = 0;

	//! @brief Executes the object.
	//! This method will be called by the queue when the object is popped from the queue.
	virtual int exec() override = 0;

protected:
	static void makeRequest(UIQueueableRequest* _request);

	explicit UIQueueableRequest(ot::BasicQueueObject::InsertOrder _insertOrder);

private:

};