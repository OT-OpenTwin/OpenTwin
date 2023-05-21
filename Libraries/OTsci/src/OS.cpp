//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include <aci/OS.h>
#include <cassert>

aci::OS * g_instance{ nullptr };

aci::OS * aci::OS::instance(void) {
	if (g_instance == nullptr) { g_instance = new OS; }
	return g_instance;
}
void aci::OS::clearInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

void aci::OS::attachOSHandler(AbstractOSHandler * _handler) {
	assert(_handler);
	m_handler = _handler;
}