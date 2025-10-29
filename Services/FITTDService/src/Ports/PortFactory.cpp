// @otlicense

#include "Ports/PortFactory.h"
#include "Ports/SourceTarget.h"
#include "Ports/Signal.h"

#include "Ports/PortSinusGaussian.h"
#include "Ports/PortGaussian.h"

#include "Grid/Point.h"

Port<float>* PortFactory::CreatePort(PortSettings settings, std::pair<Point3D,index_t> sourceLocation)
{	
	axis portAxis =	settings.GetAxis();
	sourceType portField = settings.GetSourceType();
	const Signal excitationSignal =	settings.GetExcitationSignal();
	signalType excitationType = excitationSignal.GetSignalType();
	std::string portName = settings.GetPortName();

	if (excitationType == sinusoidalGauss)
	{
		auto *port = new PortSinusGaussian<float>(portName, sourceLocation, portField, portAxis, settings.GetExcitationSignalName(), excitationSignal.GetFrequencyMin(), excitationSignal.GetFrequencyMax());
		//auto *port = new PortGaussian<float>(portName, sourceLocation, portField, portAxis, settings.GetExcitationSignalName(), 30,100);
		return port;
	}
	else
	{
		throw std::invalid_argument("Unsopported signal type");
	}
	
}

