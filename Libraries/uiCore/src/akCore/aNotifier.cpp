// @otlicense

// AK header
#include <akCore/aNotifier.h>

ak::aNotifier::aNotifier() : m_isEnabled(true) {}

void ak::aNotifier::enable(void) { m_isEnabled = true; }

void ak::aNotifier::disable(void) { m_isEnabled = false; }

bool ak::aNotifier::isEnabled(void) const { return m_isEnabled; }
