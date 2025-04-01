#include "OTWidgets/CoordinateFormatConverter.h"
#include <cmath>
#include <numbers>
#include <functional>

ot::PointsContainer ot::CoordinateFormatConverter::defineXYPoints(const PlotDatasetData& _datasetData, Plot1DCfg::AxisQuantity _axisQuantity)
{
	PointsContainer pointsContainer;
	pointsContainer.m_xData = {};
	pointsContainer.m_yData = {};

	const ComplexNumberContainer* yDataPointer =  &_datasetData.getDataY();
	//Set xData pointer:
	if (_axisQuantity != Plot1DCfg::AxisQuantity::Complex)
	{
		assert(_datasetData.getDataX().size() != 0); //No data set should have no xData set.
		pointsContainer.m_xData = &_datasetData.getDataX();
	}
	else
	{
		//Only here we put the magnitude on the x-axis
		const ComplexNumberContainerCartesian* cartesian = dynamic_cast<const ComplexNumberContainerCartesian*>(yDataPointer);
		if (cartesian != nullptr)
		{
			//Here the original data does not contain the magnitude, so we take it from the buffer or calculate it freshly and set it in the buffer.
			pointsContainer.m_xData = getBufferedMagnitude(_datasetData,false);
		}
		else
		{
			const ComplexNumberContainerPolar* polar = dynamic_cast<const ComplexNumberContainerPolar*>(yDataPointer);
			assert(polar != nullptr);
			if (polar->m_magnitudes.size() != 0)
			{
				pointsContainer.m_xData = &polar->m_magnitudes;
			}
			else
			{
				pointsContainer.m_xData = getBufferedMagnitude(_datasetData, true);
			}
		}
	}

	//Now we set the y-axis data
	if (_axisQuantity == Plot1DCfg::AxisQuantity::Phase)
	{
		const ComplexNumberContainerPolar* polar = dynamic_cast<const ComplexNumberContainerPolar*>(yDataPointer);
		if (polar != nullptr)
		{
			if (polar->m_phases.size() == 0)
			{
				pointsContainer.m_yData = getBufferedPhase(_datasetData, true);
			}
			else
			{
				pointsContainer.m_yData = &polar->m_phases;
			}
		}
		else
		{
			pointsContainer.m_yData = getBufferedPhase(_datasetData, false);
		}
	}
	else if (_axisQuantity == Plot1DCfg::AxisQuantity::Complex)
	{
		const ComplexNumberContainerPolar* polar = dynamic_cast<const ComplexNumberContainerPolar*>(yDataPointer);
		if (polar!= nullptr)
		{
			//Original data exists in polar format
			if (polar->m_phases.size() == 0)
			{
				pointsContainer.m_yData = getBufferedPhase(_datasetData, true,true);
			}
			else
			{
				pointsContainer.m_yData = &polar->m_phases;
			}
		}
		else
		{
			pointsContainer.m_yData = getBufferedPhase(_datasetData, false,true);
		}
		
	}
	else if (_axisQuantity == Plot1DCfg::AxisQuantity::Magnitude)
	{
		const ComplexNumberContainerPolar* polar = dynamic_cast<const ComplexNumberContainerPolar*>(yDataPointer);
		if (polar != nullptr)
		{
			if (polar->m_magnitudes.size() == 0)
			{
				pointsContainer.m_yData = getBufferedMagnitude(_datasetData,true);
			}
			else
			{
				pointsContainer.m_yData = &polar->m_magnitudes;
			}
		}
		else
		{
			pointsContainer.m_yData = getBufferedMagnitude(_datasetData, false);
		}
	}
	else if (_axisQuantity == Plot1DCfg::AxisQuantity::Real)
	{
		const ComplexNumberContainerCartesian* cartesian = dynamic_cast<const ComplexNumberContainerCartesian*>(yDataPointer);
		if (cartesian != nullptr)
		{
			if (cartesian->m_real.size() == 0)
			{
				pointsContainer.m_yData = getBufferedReal(_datasetData, true);
			}
			else
			{
				pointsContainer.m_yData = &cartesian->m_real;
			}
		}
		else
		{
			pointsContainer.m_yData = getBufferedReal(_datasetData, false);
		}
	}
	else if (_axisQuantity == Plot1DCfg::AxisQuantity::Imaginary)
	{
		const ComplexNumberContainerCartesian* cartesian = dynamic_cast<const ComplexNumberContainerCartesian*>(yDataPointer);
		if (cartesian != nullptr)
		{
			if (cartesian->m_imag.size() == 0)
			{
				pointsContainer.m_yData = getBufferedImaginary(_datasetData, true);
			}
			else
			{
				pointsContainer.m_yData = &cartesian->m_imag;
			}
		}
		else
		{
			pointsContainer.m_yData = getBufferedImaginary(_datasetData, false);
		}
	}	
	return pointsContainer;
}

ot::CoordinateFormatConverter::~CoordinateFormatConverter()
{
	if (m_buffer != nullptr)
	{
		delete m_buffer;
		m_buffer = nullptr;
	}
}

std::vector<double> ot::CoordinateFormatConverter::calculateMagnitude(const PlotDatasetData& _datasetData)
{
	const ComplexNumberContainerCartesian* cartesian = dynamic_cast<const ComplexNumberContainerCartesian*>(&_datasetData.getDataY());
	assert(cartesian != nullptr);

	const std::vector<double>& yIm = cartesian->m_imag;
	const std::vector<double>& yRe = cartesian->m_real;
	
	size_t numberOfEntries = _datasetData.getNumberOfDatapoints();
	std::vector<double> magnitudes;
	magnitudes.reserve(numberOfEntries);

	std::function<double(size_t index)> f_calculateMagnitude; 
	if (yIm.size() == 0)
	{
		f_calculateMagnitude = [&yRe](size_t index)
			{  
				return yRe[index];
			};
	}
	else if (yRe.size() == 0)
	{
		f_calculateMagnitude = [&yIm](size_t index)
			{
				return yIm[index];
			};
	}
	else
	{
		f_calculateMagnitude = [&yRe, &yIm](size_t index)
			{
				return std::sqrt(std::pow(yIm[index], 2) + std::pow(yRe[index], 2));
			};
	}

	for (size_t i = 0; i < numberOfEntries; i++)
	{
		const double magnitude = f_calculateMagnitude(i);
		magnitudes.push_back(magnitude);
	}
	return magnitudes;
}

std::vector<double> ot::CoordinateFormatConverter::calculatePhase(const PlotDatasetData& _datasetData)
{
	const ComplexNumberContainerCartesian* cartesian = dynamic_cast<const ComplexNumberContainerCartesian*>(&_datasetData.getDataY());
	assert(cartesian != nullptr);

	const std::vector<double>& yIm = cartesian->m_imag;
	const std::vector<double>& yRe = cartesian->m_real;

	size_t numberOfEntries = _datasetData.getNumberOfDatapoints();
	std::vector<double> phases;
	phases.reserve(numberOfEntries);

	std::function<double(size_t index)> f_calculatePhase;
	if (yIm.size() == 0)
	{
		f_calculatePhase = [&yRe](size_t index)
			{
				return std::atan2(0,yRe[index]) / std::numbers::pi * 180;
			};
	}
	else if (yRe.size() == 0)
	{
		f_calculatePhase = [&yIm](size_t index)
			{
				return std::atan2(yIm[index],0) / std::numbers::pi * 180;
			};
	}
	else
	{
		f_calculatePhase = [&yRe, &yIm](size_t index)
			{
				return std::atan2(yIm[index], yRe[index]) / std::numbers::pi * 180;
			};
	}

	for (size_t i = 0; i < numberOfEntries; i++)
	{
		const double phase = f_calculatePhase(i);
		phases.push_back(phase);
	}
	return phases;
}

std::vector<double> ot::CoordinateFormatConverter::calculateImag(const PlotDatasetData& _datasetData)
{
	const ComplexNumberContainerPolar* polar = dynamic_cast<const ComplexNumberContainerPolar*>(&_datasetData.getDataY());
	assert(polar != nullptr);

	const std::vector<double>& phases = polar->m_phases;
	const std::vector<double>& magnitude = polar->m_magnitudes;

	size_t numberOfEntries = _datasetData.getNumberOfDatapoints();
	std::vector<double> imagValues;
	imagValues.reserve(numberOfEntries);

	std::function<double(size_t index)> f_calculateImag;
	if (phases.size() == 0 || magnitude.size() == 0)
	{
		f_calculateImag = [](size_t index)
			{
				return 0;
			};
	}
	else
	{
		f_calculateImag = [&magnitude, &phases](size_t index)
			{
				return magnitude[index] * sin(phases[index]);
			};
	}

	for (size_t i = 0; i < numberOfEntries; i++)
	{
		const double imag = f_calculateImag(i);
		imagValues.push_back(imag);
	}
	return imagValues;
}

std::vector<double> ot::CoordinateFormatConverter::calculateReal(const PlotDatasetData& _datasetData)
{
	const ComplexNumberContainerPolar* polar = dynamic_cast<const ComplexNumberContainerPolar*>(&_datasetData.getDataY());
	assert(polar != nullptr);

	const std::vector<double>& phases = polar->m_phases;
	const std::vector<double>& magnitude = polar->m_magnitudes;

	size_t numberOfEntries = _datasetData.getNumberOfDatapoints();
	std::vector<double> realValues;
	realValues.reserve(numberOfEntries);

	std::function<double(size_t index)> f_calculateReal;
	if (phases.size() == 0)
	{
		f_calculateReal = [&magnitude](size_t index)
			{
				return magnitude[index];
			};
	}
	else if(magnitude.size() == 0)
	{
		f_calculateReal = [](size_t index)
			{
				return 0;
			};
	}
	else
	{
		f_calculateReal = [&magnitude, &phases](size_t index)
			{
				return magnitude[index] * cos(phases[index]);
			};
	}

	for (size_t i = 0; i < numberOfEntries; i++)
	{
		const double real = f_calculateReal(i);
		realValues.push_back(real);
	}
	return realValues;
}

std::vector<double>* ot::CoordinateFormatConverter::getBufferedPhase(const PlotDatasetData& _datasetData, bool _allZeros, bool _keepBuffer)
{
	//First check the buffer if it already has the right data
	if (m_buffer != nullptr)
	{
		ComplexNumberContainerPolar* polar = dynamic_cast<ComplexNumberContainerPolar*>(m_buffer);
		if (polar != nullptr && polar->m_phases.size() != 0)
		{
			return &polar->m_phases;
		}
	}

	//Now we deal with an already existing buffer
	resetBufferPolar(_keepBuffer);
	
	ComplexNumberContainerPolar* polar = dynamic_cast<ComplexNumberContainerPolar*>(m_buffer);
	if (_allZeros)
	{
		polar->m_phases = std::vector<double>(_datasetData.getNumberOfDatapoints(), 0);
	}
	else
	{
		polar->m_phases = calculatePhase(_datasetData);
	}
	return &polar->m_phases;
}

std::vector<double>* ot::CoordinateFormatConverter::getBufferedMagnitude(const PlotDatasetData& _datasetData, bool _allZeros, bool _keepBuffer)
{
	//First check the buffer if it already has the right data
	if (m_buffer != nullptr)
	{
		ComplexNumberContainerPolar* polar = dynamic_cast<ComplexNumberContainerPolar*>(m_buffer);
		if (polar != nullptr && polar->m_magnitudes.size() != 0)
		{
			return &polar->m_magnitudes;
		}
	}
	
	//Now we deal with an already existing buffer
	resetBufferPolar(_keepBuffer);

	ComplexNumberContainerPolar* polar = dynamic_cast<ComplexNumberContainerPolar*>(m_buffer);
	if (_allZeros)
	{
		polar->m_magnitudes = std::vector<double>(_datasetData.getNumberOfDatapoints(), 0);
	}
	else
	{
		polar->m_magnitudes = calculateMagnitude(_datasetData);
	}
	return &polar->m_magnitudes;
}

std::vector<double>* ot::CoordinateFormatConverter::getBufferedImaginary(const PlotDatasetData& _datasetData, bool allZeros, bool _keepBuffer)
{
	//First check the buffer if it already has the right data
	if (m_buffer != nullptr)
	{
		ComplexNumberContainerCartesian* cartesian = dynamic_cast<ComplexNumberContainerCartesian*>(m_buffer);
		if (cartesian != nullptr)
		{
			return &cartesian->m_imag;
		}
	}
	
	resetBufferCartesian(_keepBuffer);	

	ComplexNumberContainerCartesian* cartesian = dynamic_cast<ComplexNumberContainerCartesian*>(m_buffer);
	if (allZeros)
	{
		cartesian->m_imag = std::vector<double>(_datasetData.getNumberOfDatapoints(), 0);
	}
	else
	{
		cartesian->m_imag = calculateImag(_datasetData);
	}
	return &cartesian->m_imag;
}

std::vector<double>* ot::CoordinateFormatConverter::getBufferedReal(const PlotDatasetData& _datasetData, bool allZeros, bool _keepBuffer)
{
	//First check the buffer if it already has the right data
	if (m_buffer != nullptr)
	{
		ComplexNumberContainerCartesian* cartesian = dynamic_cast<ComplexNumberContainerCartesian*>(m_buffer);
		if (cartesian != nullptr)
		{
			return &cartesian->m_real;
		}
	}

	//If the buffer has not the right data, we need to build a new buffer with the correct data
	resetBufferCartesian(_keepBuffer);


	ComplexNumberContainerCartesian* cartesian = dynamic_cast<ComplexNumberContainerCartesian*>(m_buffer);
	if (allZeros)
	{
		cartesian->m_real= std::vector<double>(_datasetData.getNumberOfDatapoints(), 0);
	}
	else
	{
		cartesian->m_real= calculateReal(_datasetData);
	}
	return &cartesian->m_real;
}

void ot::CoordinateFormatConverter::resetBufferPolar(bool _keepBuffer)
{
	if (m_buffer != nullptr)
	{
		ComplexNumberContainerPolar* polar = dynamic_cast<ComplexNumberContainerPolar*>(m_buffer);
		//If the buffer has not the right data, we need to build a new buffer with the correct data. Also for building up the entire tuple we may want to keep the buffer
		//Keep buffer if polar != nullptr && _keepBuffer
		if (polar == nullptr || !_keepBuffer)
		{
			delete m_buffer;
			m_buffer = new ComplexNumberContainerPolar();
		}
	}
	else
	{
		m_buffer = new ComplexNumberContainerPolar();
	}
}

void ot::CoordinateFormatConverter::resetBufferCartesian(bool _keepBuffer)
{
	if (m_buffer != nullptr)
	{
		ComplexNumberContainerCartesian* cartesian = dynamic_cast<ComplexNumberContainerCartesian*>(m_buffer);
		//If the buffer has not the right data, we need to build a new buffer with the correct data. Also for building up the entire tuple we may want to keep the buffer
		//Keep buffer if polar != nullptr && _keepBuffer
		if (cartesian == nullptr || !_keepBuffer)
		{
			delete m_buffer;
			m_buffer = new ComplexNumberContainerCartesian();
		}
	}
	else
	{
		m_buffer = new ComplexNumberContainerCartesian();
	}
}

