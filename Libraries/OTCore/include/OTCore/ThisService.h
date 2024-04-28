//! @file ThisService.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/CoreAPIExport.h"

namespace ot {

	//! @class ThisService
	//! @brief The central service information class of a service.
	//! The ThisService is a singleton that should be used by every class to retreive or set the current service information.
	class OT_CORE_API_EXPORT ThisService : public ServiceBase {
		OT_DECL_NOCOPY(ThisService)
	public:
		//! @brief Returns global instance.
		static ThisService& instance(void);

		//! @brief Adds the current service id to the provided JSON document.
		static void addIdToJsonDocument(JsonDocument& _document);

		//! @brief Sets the current service id from the provided JSON document.
		static serviceID_t getIdFromJsonDocument(const JsonDocument& _document);

	private:
		//! @brief Private constructor.
		ThisService() {};

		//! @brief Private destructor.
		~ThisService() {};
	};

}
