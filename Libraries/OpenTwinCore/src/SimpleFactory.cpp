// OpenTwin header
#include "OpenTwinCore/SimpleFactory.h"
#include "OpenTwinCore/Logger.h"


ot::SimpleFactory& ot::SimpleFactory::instance(void) {
	static SimpleFactory g_instance;
	return g_instance;
}

ot::SimpleFactoryObject* ot::SimpleFactory::create(const std::string& _key) {
	auto it = m_data.find(_key);
	if (it == m_data.end()) {
		OT_LOG_EAS("Class with key \"" + _key + "\" was not registered");
		return nullptr;
	}
	return it->second();
}

void ot::SimpleFactory::add(const std::string& _key, std::function<SimpleFactoryObject* (void)> _classConstructor) {
	if (this->contains(_key)) {
		OT_LOG_EAS("Class with key \"" + _key + "\" was already registered");
	}
	else {
		m_data.insert_or_assign(_key, _classConstructor);
	}
}

void ot::SimpleFactory::remove(const std::string& _key) {
	m_data.erase(_key);
}

bool ot::SimpleFactory::contains(const std::string& _key) {
	return m_data.count(_key) > 0;
}