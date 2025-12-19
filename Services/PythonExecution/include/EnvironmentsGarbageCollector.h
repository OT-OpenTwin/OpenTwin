#pragma once
#include <string>
#include "WorkerWaiterState.h"
#include "InterpreterPathSettings.h"
class EnvironmentsGarbageCollector
{
public:
	EnvironmentsGarbageCollector() = default;

	void setInterpreterPathSettings(const InterpreterPathSettings& _settings) { m_interpreterPathSettings = _settings; }
	void run();
private:
	InterpreterPathSettings m_interpreterPathSettings;
	const int32_t m_daysUntilRemoval = 3;
	const int32_t m_numberOfEnvironmentsLimit = 5;
	const std::string m_removalMarker = "_remove";
	// All custom environments beyond the limit are marked for cleanup if they have not been accessed for m_daysUntilRemoval days
	void markForCleanup();
	void removeMarkedEnvironments();
};
