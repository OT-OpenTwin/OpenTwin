#pragma once

// OpenTwin header
#include <openTwin/BlockConnection.h>

namespace ot {

	class FlowConnector;

	class FlowConnection : public BlockConnection {
	public:
		FlowConnection(FlowConnector* _from, FlowConnector* _to);
		virtual ~FlowConnection();

		virtual QPointF pointFrom(void) const override;
		virtual QPointF pointTo(void) const override;

	private:
		FlowConnector* m_from;
		FlowConnector* m_to;

		FlowConnection() = delete;
		FlowConnection(FlowConnection&) = delete;
		FlowConnection& operator = (FlowConnection&) = delete;
	};
}