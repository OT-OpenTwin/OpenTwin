//! @file API.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTsci/API.h"
#include "OTsci/SCINotifierInterface.h"
#include "OTsci/SCIPrinterInterface.h"
#include "OTsci/SCIOSInterface.h"
#include "OTsci/SCIDispatcher.h"
#include "OpenTwinCore/Singleton.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"

// std header
#include <cassert>

#define OT_SCI_API_MANAGER ot::intern::SCIAPIManager::instance()

//! @brief Initialize the SCI API
#define OT_SCI_API_CHECK_INIT(___returnValue) if (!OT_SCI_API_MANAGER.iniDone()) { otAssert(0, "SCI API not initialized yet"); return ___returnValue; }

namespace ot {
	namespace intern {
		class SCIAPIManager : public ot::Singleton<SCIAPIManager> {
			OT_SINGLETON(SCIAPIManager)
		public:
			
			//! @brief Returns true if the API was initialized
			bool iniDone(void) const { return !!m_printer && !!m_notifier && !!m_os; };

			ot::SCINotifierInterface* notifier(void) { return m_notifier; };
			ot::SCIPrinterInterface* printer(void) { return m_printer; };
			ot::SCIOSInterface* os(void) { return m_os; };

		private:
			ot::SCINotifierInterface* m_notifier;
			ot::SCIPrinterInterface* m_printer;
			ot::SCIOSInterface* m_os;

			friend bool ot::SCIAPI::initialize(ot::SCIPrinterInterface* _printer, ot::SCINotifierInterface* _notifier, ot::SCIOSInterface* _os);
			void ini(SCIPrinterInterface* _printer, SCINotifierInterface* _notifier, SCIOSInterface* _os) {
				otAssert(m_notifier == nullptr, "Already initialized");
				OTAssertNullptr(_printer);
				OTAssertNullptr(_notifier);
				OTAssertNullptr(_os);

				m_printer = _printer;
				m_notifier = _notifier;
				m_os = _os;
			}

			SCIAPIManager() : m_notifier((ot::SCINotifierInterface *)nullptr), m_printer((ot::SCIPrinterInterface*)nullptr), m_os((ot::SCIOSInterface*)nullptr) {}
			virtual ~SCIAPIManager() {};
		};
	}
}

bool ot::SCIAPI::initialize(SCIPrinterInterface* _printer, SCINotifierInterface* _notifier, SCIOSInterface* _os) {
	OT_SCI_API_MANAGER.ini(_printer, _notifier, _os);
	return OT_SCI_API_MANAGER.iniDone();
}

void ot::SCIAPI::cleanUp(void) noexcept {
	OT_SCI_API_CHECK_INIT();
	
	InterpreterCore::instance()->scriptLoader()->unloadScripts();
	OS::clearInstance();
	InterpreterCore::clearInstance();
	initialized = false;
}

ot::InterpreterCore * ot::SCIAPI::core(void) {
	if (!initialized) {
		assert(0);		// API not initialized
		return nullptr;
	}
	return InterpreterCore::instance();
}

std::wstring ot::SCIAPI::currentPath(void) {
	if (!initialized) {
		assert(0);		// API not initialized
		return std::wstring();
	}
	return InterpreterCore::instance()->currentPath();
}

void ot::SCIAPI::setCurrentPath(const std::wstring& _path) {
	if (!initialized) {
		assert(0);		// API not initialized
		return;
	}
	InterpreterCore::instance()->setCurrentPath(_path);
}
