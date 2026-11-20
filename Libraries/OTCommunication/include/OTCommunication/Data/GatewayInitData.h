// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCommunication/CommunicationAPIExport.h"

namespace ot
{

	class GatewayInitData : public Serializable
	{
		OT_DECL_DEFCOPY(GatewayInitData)
		OT_DECL_DEFMOVE(GatewayInitData)
	public:
		GatewayInitData() = default;
		virtual ~GatewayInitData() = default;
		
	private:

	};

}