//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTsci/OTsciAPIExport.h"

#include <string>

namespace aci {

	class InterpreterCore;
	class AbstractPrinter;
	class AbstractOSHandler;
	class AbstractInterpreterNotifier;

	namespace API {

		OTSCI_API_EXPORT bool initialize(AbstractPrinter * _printer, AbstractInterpreterNotifier * _notifier, AbstractOSHandler * _osHandler);

		OTSCI_API_EXPORT void cleanUp(void) noexcept;

		OTSCI_API_EXPORT InterpreterCore * core(void);

		OTSCI_API_EXPORT std::wstring currentPath(void);

		OTSCI_API_EXPORT void setCurrentPath(const std::wstring& _path);
	}

}
