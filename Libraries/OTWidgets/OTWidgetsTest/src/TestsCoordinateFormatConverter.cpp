// @otlicense

#include "gtest/gtest.h"
#include "FixtureCoordinateFormatConverter.h"
#include "OTWidgets/CoordinateFormatConverter.h"
TEST_F(FixtureCoordinateFormatConverter, RealPlot_RealDataExisting) 
{
	auto dataset = getDatasetRealOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Real);
	
	const ot::ComplexNumberContainerCartesian* yData = dynamic_cast<const ot::ComplexNumberContainerCartesian*>(&dataset.getDataY());
	EXPECT_NE(yData, nullptr);

	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(dataset.getDataX()[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(yData->m_real[i], (*plotData.m_yData)[i]);
	}
}

TEST_F(FixtureCoordinateFormatConverter, RealPlot_RealDataNotExisting)
{
	auto dataset = getDatasetImagOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Real);

	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(dataset.getDataX()[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(0., (*plotData.m_yData)[i]);

	}
}

TEST_F(FixtureCoordinateFormatConverter, ImPlot_ImDataNotExisting)
{
	auto dataset = getDatasetRealOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Imaginary);

	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(dataset.getDataX()[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(0., (*plotData.m_yData)[i]);

	}
}

TEST_F(FixtureCoordinateFormatConverter, ImPlot_ImDataExisting)
{
	auto dataset = getDatasetImagOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Imaginary);

	const ot::ComplexNumberContainerCartesian* yData = dynamic_cast<const ot::ComplexNumberContainerCartesian*>(&dataset.getDataY());
	EXPECT_NE(yData, nullptr);

	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(dataset.getDataX()[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(yData->m_imag[i], (*plotData.m_yData)[i]);
	}
}

TEST_F(FixtureCoordinateFormatConverter, MagPlot_MagDataNotExisting)
{
	auto dataset = getDatasetPhaseOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Magnitude);

	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(dataset.getDataX()[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(0., (*plotData.m_yData)[i]);

	}
}

TEST_F(FixtureCoordinateFormatConverter, MagPlot_MagDataExisting)
{
	auto dataset = getDatasetMagOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Magnitude);

	const ot::ComplexNumberContainerPolar* yData = dynamic_cast<const ot::ComplexNumberContainerPolar*>(&dataset.getDataY());
	EXPECT_NE(yData, nullptr);

	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(dataset.getDataX()[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(yData->m_magnitudes[i], (*plotData.m_yData)[i]);
	}
}

TEST_F(FixtureCoordinateFormatConverter, PhasePlot_PhaseDataNotExisting)
{
	auto dataset = getDatasetMagOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Phase);

	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(dataset.getDataX()[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(0., (*plotData.m_yData)[i]);

	}
}

TEST_F(FixtureCoordinateFormatConverter, PhasePlot_PhaseDataExisting)
{
	auto dataset = getDatasetPhaseOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Phase);

	const ot::ComplexNumberContainerPolar* yData = dynamic_cast<const ot::ComplexNumberContainerPolar*>(&dataset.getDataY());
	EXPECT_NE(yData, nullptr);

	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(dataset.getDataX()[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(yData->m_phases[i], (*plotData.m_yData)[i]);
	}
}

TEST_F(FixtureCoordinateFormatConverter, ComplexPlot_CartesianBothValues)
{
	auto dataset = getDatasetComplexCartesian();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Complex);
	
	std::vector<double> expectedPhase = getExpectedComplexPhase();
	std::vector<double> expectedMagnitude = getExpectedComplexMag();
	
	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(expectedMagnitude[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(expectedPhase[i], (*plotData.m_yData)[i]);

	}
}

TEST_F(FixtureCoordinateFormatConverter, ComplexPlot_PolarBothValues)
{
	auto dataset = getDatasetComplexPolar();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Complex);

	std::vector<double> expectedPhase = getExpectedComplexPhase();
	std::vector<double> expectedMagnitude = getExpectedComplexMag();

	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(expectedMagnitude[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(expectedPhase[i], (*plotData.m_yData)[i]);

	}
}

TEST_F(FixtureCoordinateFormatConverter, ComplexPlot_PolarOnlyMagnitude)
{
	auto dataset = getDatasetMagOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Complex);

	const ot::ComplexNumberContainerPolar* yData = dynamic_cast<const ot::ComplexNumberContainerPolar*>(&dataset.getDataY());
	std::vector<double> expectedMagnitude = yData->m_magnitudes;

	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(expectedMagnitude[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(0, (*plotData.m_yData)[i]);

	}
}

TEST_F(FixtureCoordinateFormatConverter, ComplexPlot_PolarOnlyPhase)
{
	auto dataset = getDatasetPhaseOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Complex);

	const ot::ComplexNumberContainerPolar* yData = dynamic_cast<const ot::ComplexNumberContainerPolar*>(&dataset.getDataY());
	std::vector<double> expectedPhase = yData->m_phases;

	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(0, (*plotData.m_xData)[i]);
		EXPECT_EQ(expectedPhase[i], (*plotData.m_yData)[i]);

	}
}

TEST_F(FixtureCoordinateFormatConverter, ComplexPlot_CartesianRealOnly)
{
	auto dataset = getDatasetRealOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Complex);

	const ot::ComplexNumberContainerCartesian* yData = dynamic_cast<const ot::ComplexNumberContainerCartesian*>(&dataset.getDataY());
	std::vector<double> expectedMag = yData->m_real;
	
	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(expectedMag[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(0, (*plotData.m_yData)[i]);
	}
}

TEST_F(FixtureCoordinateFormatConverter, ComplexPlot_CartesianImagOnly)
{
	auto dataset = getDatasetImagOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Complex);

	const ot::ComplexNumberContainerCartesian* yData = dynamic_cast<const ot::ComplexNumberContainerCartesian*>(&dataset.getDataY());
	std::vector<double> expectedMag = yData->m_imag;

	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(expectedMag[i], (*plotData.m_xData)[i]);
		EXPECT_EQ(90, (*plotData.m_yData)[i]);
	}
}

TEST_F(FixtureCoordinateFormatConverter, BufferOverwrite)
{
	auto dataset = getDatasetImagOnly();
	ot::CoordinateFormatConverter transformer;
	ot::PointsContainer plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Real);

	
	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(0, (*plotData.m_yData)[i]);
		EXPECT_EQ(dataset.getDataX()[i], (*plotData.m_xData)[i]);
	}

	plotData = transformer.defineXYPoints(dataset, ot::Plot1DCfg::Phase);
	const ot::ComplexNumberContainerCartesian* yData = dynamic_cast<const ot::ComplexNumberContainerCartesian*>(&dataset.getDataY());
	for (size_t i = 0; i < dataset.getNumberOfDatapoints(); i++)
	{
		EXPECT_EQ(90, (*plotData.m_yData)[i]);
		EXPECT_EQ(dataset.getDataX()[i], (*plotData.m_xData)[i]);
	}
}