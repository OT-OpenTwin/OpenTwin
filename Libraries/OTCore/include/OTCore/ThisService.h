//! @file ThisService.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/ServiceBase.h"

namespace ot {

	class ThisService : public ServiceBase {
		OT_DECL_NOCOPY(ThisService)
	public:
		static ThisService& instance(void);

		static void addIdToJsonDocument(JsonDocument& _document);
		static serviceID_t getIdFromJsonDocument(const JsonDocument& _document);

	private:
		ThisService() {};
		~ThisService() {};

	};

}
