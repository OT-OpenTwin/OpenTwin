//! @file API.h
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTsci/OTsciAPIExport.h"

#include <string>

namespace ot {

	class SCINotifierInterface;
	class SCIOSInterface;
	class SCIPrinterInterface;
	class SCIDispatcher;

	namespace SCIAPI {

		OTSCI_API_EXPORT bool initialize(ot::SCIPrinterInterface* _printer, ot::SCINotifierInterface* _notifier, ot::SCIOSInterface* _os);

		OTSCI_API_EXPORT void cleanUp(void) noexcept;

		OTSCI_API_EXPORT SCIDispatcher& dispatcher(void);

		OTSCI_API_EXPORT std::wstring currentWorkingDirectory(void);

		OTSCI_API_EXPORT void setCurrentWorkingDirectory(const std::wstring& _path);
	}

}
