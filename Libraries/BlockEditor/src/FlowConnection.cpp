// OpenTwin header
#include <openTwin/FlowConnection.h>
#include <openTwin/FlowConnector.h>

ot::FlowConnection::FlowConnection(FlowConnector* _from, FlowConnector* _to)
	: m_from(_from), m_to(_to)
{
	
}

ot::FlowConnection::~FlowConnection() {}

QPointF ot::FlowConnection::pointFrom(void) const {
	return m_from->connectionPoint();
}

QPointF ot::FlowConnection::pointTo(void) const {
	return m_to->connectionPoint();
}