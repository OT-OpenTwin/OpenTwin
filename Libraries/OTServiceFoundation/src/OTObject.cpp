#include "OTServiceFoundation/OTObject.h"
#include "OTServiceFoundation/Dispatcher.h"

ot::AbstractDispatchItem::AbstractDispatchItem(const std::string& _actionName, const ot::Flags<ot::MessageType>& _messageFlags) : m_actionName(_actionName), m_messageFlags(_messageFlags) {
	Dispatcher::instance()->add(this);
}

ot::AbstractDispatchItem::~AbstractDispatchItem() {
	Dispatcher::instance()->remove(this);
}

bool ot::AbstractDispatchItem::mayDispatch(ot::MessageType _inboundMessageType) const {
	return m_messageFlags.flagIsSet(_inboundMessageType);
}