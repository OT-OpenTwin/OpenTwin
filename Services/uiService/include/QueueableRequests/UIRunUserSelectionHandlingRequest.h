// @otlicense

// OpenTwin header
#include "QueueableRequests/UIQueueableRequest.h"

class UIRunUserSelectionHandlingRequest : public UIQueueableRequest
{
	OT_DECL_NOCOPY(UIRunUserSelectionHandlingRequest)
	OT_DECL_NOMOVE(UIRunUserSelectionHandlingRequest)
	OT_DECL_NODEFAULT(UIRunUserSelectionHandlingRequest)
public:
	static void makeRequest(bool _unique, ot::BasicQueueObject::InsertOrder _insertOrder = ot::BasicQueueObject::InsertBack);

	virtual ~UIRunUserSelectionHandlingRequest() = default;

	static std::string className() { return "UIRunUserSelectionHandlingRequest"; }
	virtual std::string getClassName() const override { return UIRunUserSelectionHandlingRequest::className(); }

	virtual int exec() override;

private:
	explicit UIRunUserSelectionHandlingRequest(ot::BasicQueueObject::InsertOrder _insertOrder);

};
