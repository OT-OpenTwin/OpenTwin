// @otlicense

#pragma once
#include <string>

#include "Simulation/DegreesOfFreedom3DLoopCPU.h"
#include "Ports/SourceTarget.h"
#include "SolverSettings/PortSettings.h"
#include "Grid/Point.h"

template<class T>
class Port
{
public:
	Port(std::string portName, std::pair<Point3D,index_t> &location, sourceType sourceType, axis sourceAxis, std::string signalName)
		: _portName(portName), _index(location.second), _point(location.first), _sourceAxis(sourceAxis), _sourceType(sourceType), _signalName(signalName) {}
	virtual ~Port() {}
	
	void ApplySource( DegreesOfFreedom3DLoopCPU<T> & doF, int currentTimeStep, float currentTime) 
	{
		_currentTimeStep = currentTimeStep;
		_currentValue = CalculateCurrentValue(currentTime);

		if (_sourceType == e)
		{
			if (_sourceAxis == x_axis)
			{
				T oldValue = *doF.GetElectricVoltageXComponent(_index);
				doF.SetElectricVoltageXComponentAt(_index, _currentValue + oldValue);
			}
			else if (_sourceAxis == y_axis)
			{
				T oldValue = *doF.GetElectricVoltageYComponent(_index);
				doF.SetElectricVoltageYComponentAt(_index, _currentValue + oldValue);
			}
			else
			{
				T oldValue = *doF.GetElectricVoltageZComponent(_index);
				doF.SetElectricVoltageZComponentAt(_index, _currentValue + oldValue);
			}
		}
		else
		{
			if (_sourceAxis == x_axis)
			{
				T oldValue = *doF.GetMagneticVoltageXComponent(_index);
				doF.SetMagneticVoltageXComponentAt(_index, _currentValue + oldValue);
			}
			else if (_sourceAxis == y_axis)
			{
				T oldValue = *doF.GetMagneticVoltageYComponent(_index);
				doF.SetMagneticVoltageYComponentAt(_index, _currentValue + oldValue);
			}
			else
			{
				T oldValue = *doF.GetMagneticVoltageZComponent(_index);
				doF.SetMagneticVoltageZComponentAt(_index, _currentValue + oldValue);
				
			}
		}
	};
	
	void ApplySourceHW(DegreesOfFreedom3DLoopCPU<T> & doF, int currentTimeStep, float currentTime)
	{
		_currentTimeStep = currentTimeStep;
		_currentValue = CalculateCurrentValue(currentTime);

		if (_sourceType == e)
		{
			if (_sourceAxis == x_axis)
			{
				doF.SetElectricVoltageXComponentAt(_index, _currentValue);
			}
			else if (_sourceAxis == y_axis)
			{
				doF.SetElectricVoltageYComponentAt(_index, _currentValue);
			}
			else
			{
				doF.SetElectricVoltageZComponentAt(_index, _currentValue);
			}
		}
		else
		{
			if (_sourceAxis == x_axis)
			{
				doF.SetMagneticVoltageXComponentAt(_index, _currentValue);
			}
			else if (_sourceAxis == y_axis)
			{
				doF.SetMagneticVoltageYComponentAt(_index, _currentValue);
			}
			else
			{
				doF.SetMagneticVoltageZComponentAt(_index, _currentValue);
			}
		}
	}

	void ApplySourceHWDiscreteValueCalc(DegreesOfFreedom3DLoopCPU<T> & doF, int currentTimeStep)
	{
		_currentTimeStep = currentTimeStep;
		_currentValue = CalculateCurrentValue(currentTimeStep);

		if (_sourceType == e)
		{
			if (_sourceAxis == x_axis)
			{
				doF.SetElectricVoltageXComponentAt(_index, _currentValue);
			}
			else if (_sourceAxis == y_axis)
			{
				doF.SetElectricVoltageYComponentAt(_index, _currentValue);
			}
			else
			{
				doF.SetElectricVoltageZComponentAt(_index, _currentValue);
			}
		}
		else
		{
			if (_sourceAxis == x_axis)
			{
				doF.SetMagneticVoltageXComponentAt(_index, _currentValue);
			}
			else if (_sourceAxis == y_axis)
			{
				doF.SetMagneticVoltageYComponentAt(_index, _currentValue);
			}
			else
			{
				doF.SetMagneticVoltageZComponentAt(_index, _currentValue);
			}
		}
	}
	
	const std::string GetPortName() const { return _portName; }
	const std::string GetSignalName() const { return _signalName; }
	const Point3D GetPoint() const { return _point; }
	const int GetCurrentTimestep() const { return _currentTimeStep; }
	const T GetCurrentValue() const { return _currentValue; }
	const sourceType GetPortTargetField() { return _sourceType; }
	const axis GetPortTargetAxis() { return _sourceAxis; }

private:
		std::string _portName;
		std::string _signalName;
		sourceType _sourceType;
		axis _sourceAxis;
		Point3D _point;

protected:
	index_t _index;
	int _currentTimeStep;
	T _currentValue;

	virtual T CalculateCurrentValue(float currentTime) = 0;

	float EFunc(float exponent) { return expf(exponent); }
	double EFunc(double exponent) { return exp(exponent); }

};
