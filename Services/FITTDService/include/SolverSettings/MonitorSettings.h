#pragma once
/*********************************************************************
 * @file   MonitorSettings.h
 * @brief  
 * 
 * @author Jan Wagner
 * @date   July 2022
 *********************************************************************/
#include <string>
#include <map>

#include "EntitySolverMonitor.h"
#include "Ports/SourceTarget.h"
#include "PhysicalQuantities.h"

enum MonitorDomain { frequencyDomain, timeDomain };
enum MonitorGeometry { nodes, edges, volume, surface };
enum MonitorQuantity {vector, x_component, y_component, z_component};
enum MonitorVolume {full, point};

class MonitorSettings
{
public:
	MonitorSettings(std::string monitorName, std::string selectedDomain, std::string monitoredGeometry, std::string observedComponent, std::string monitorQuantity, std::string monitorVolume, int samplingFrequency, PhysicalQuantities observedFrequency, int observedTimeStep)
	: _monitorName(monitorName), _selectedDomain(selectedDomain), _monitoredGeometry(monitoredGeometry), _observedComponent(observedComponent), _monitorQuantity(monitorQuantity), _monitorVolume(monitorVolume), _samplingFrequency(samplingFrequency), _observedFrequency(observedFrequency), _obsTimeStep(observedTimeStep){};
	const std::string GetMonitorName(void) const { return _monitorName; }
	const MonitorDomain GetDomain(void) const { return _supportedDomains.at(_selectedDomain); }
	const MonitorGeometry GetMonitoredGeometry (void) const { return _supportedDoF.at(_monitoredGeometry); }
	const MonitorQuantity GetMonitorQuantity(void) const { return _supportedQuantities.at(_monitorQuantity); }
	const MonitorVolume GetMonitorVolume(void) const { return _supportedVolumes.at(_monitorVolume); }
	const sourceType GetMonitoredField(void) const { return _supportedTargetFields.at(_observedComponent); }
	const int GetSamplingFrequency(void) const { return _samplingFrequency; }
	const double GetObservedFrequency(void) const { return _observedFrequency.GetInternalValue(); }
	const int GetObservedTimeStep(void) const { return _obsTimeStep; }
	std::string Print(void);

private:
	std::string _monitorName;
	std::string _selectedDomain;
	std::string _observedComponent;
	std::string _monitoredGeometry;
	std::string _monitorVolume;
	std::string _monitorQuantity;
	std::string _formerUnit;
	PhysicalQuantities _observedFrequency;
	int _samplingFrequency;
	int _obsTimeStep;

	std::map < std::string, MonitorVolume> _supportedVolumes = { {EntitySolverMonitor::GetPropertyValueVolumeFull(), full}, {EntitySolverMonitor::GetPropertyValueVolumePoint(), point} };
	std::map < std::string, MonitorQuantity> _supportedQuantities = { {EntitySolverMonitor::GetPropertyValueQuantityVector(), vector},{EntitySolverMonitor::GetPropertyValueQuantityComponentX(), x_component},{EntitySolverMonitor::GetPropertyValueQuantityComponentY(), y_component},{EntitySolverMonitor::GetPropertyValueQuantityComponentZ(), z_component} };
	std::map<std::string, MonitorDomain> _supportedDomains = { {EntitySolverMonitor::GetPropertyValueDomainFrequency(),frequencyDomain}, {EntitySolverMonitor::GetPropertyValueDomainTime(), timeDomain} };
	std::map <std::string, MonitorGeometry> _supportedDoF = { {EntitySolverMonitor::GetPropertyValueDoFNode(), nodes}, {EntitySolverMonitor::GetPropertyValueDoFEdge(), edges} };
	std::map<std::string, sourceType> _supportedTargetFields = { {EntitySolverMonitor::GetPropertyValueFieldCompElectr(), e},{EntitySolverMonitor::GetPropertyValueFieldCompMagnetic(), h} };
};
