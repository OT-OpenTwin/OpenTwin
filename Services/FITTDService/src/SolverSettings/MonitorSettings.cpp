#include "SolverSettings/MonitorSettings.h"

std::string MonitorSettings::Print(void)
{
	std::string monitorDescription = "Monitor: " + _monitorName + "\n" +
		"Monitors: " + _selectedDomain + " Domain\n" +
		"Degrees of freedom: " + _observedComponent +" "+ _monitorQuantity +" on "+ _monitoredGeometry + "\n" +
		"Sampling frequency: " + std::to_string(_samplingFrequency) + "\n"+
		"Monitored frequency: " + _observedFrequency.Print();

	return monitorDescription;
}
