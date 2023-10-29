//! @file OToolkitAPI.h
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OToolkitAPI/otoolkitapi_global.h"
#include "OToolkitAPI/Tool.h"

#define OTOOL_LOG(___sender, ___message) otoolkit::api::getGlobalInterface()->log(___sender, otoolkit::APIInterface::Information, ___message);
#define OTOOL_LOGW(___sender, ___message) otoolkit::api::getGlobalInterface()->log(___sender, otoolkit::APIInterface::Warning, ___message);
#define OTOOL_LOGE(___sender, ___message) otoolkit::api::getGlobalInterface()->log(___sender, otoolkit::APIInterface::Error, ___message);

namespace otoolkit {

	class OTOOLKITAPI_EXPORT APIInterface
	{
	public:
		enum InterfaceLogType {
			Information,
			Warning,
			Error
		};

		virtual void log(const QString& _sender, InterfaceLogType _type, const QString& _message) = 0;

		virtual bool addTool(Tool* _tool) = 0;

	protected:
		APIInterface() {};
		virtual ~APIInterface() {};
	};

	namespace api {

		OTOOLKITAPI_EXPORTONLY void initialize(APIInterface* _interface);

		OTOOLKITAPI_EXPORTONLY otoolkit::APIInterface* getGlobalInterface(void);

	}


}

