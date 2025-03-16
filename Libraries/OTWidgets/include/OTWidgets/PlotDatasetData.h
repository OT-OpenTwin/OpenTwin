//! @file PlotDatasetData.h
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/Plot1DCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

class QwtPlotCurve;

namespace ot {

	class PolarPlotData;

	class OT_WIDGETS_API_EXPORT PlotDatasetData {
		OT_DECL_NOCOPY(PlotDatasetData)
		OT_DECL_NODEFAULT(PlotDatasetData)
	public:
		PlotDatasetData(double* _dataX, double* _dataXcalc, double* _dataY, double* _dataYim, double* _dataYcalc, long _dataSize);
		~PlotDatasetData();

		PlotDatasetData(PlotDatasetData&& _other) noexcept;
		PlotDatasetData& operator = (PlotDatasetData&& _other) noexcept;
		
		PlotDatasetData createCopy(void) const;

		void calculateData(Plot1DCfg::AxisQuantity _axisQuantity, QwtPlotCurve* _cartesianCurve, PolarPlotData* _polarData);

		void replaceData(double* _dataX, double* _dataXcalc, double* _dataY, double* _dataYim, double* _dataYcalc, long _dataSize);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		double* getDataX(void) const { return m_dataX; };
		
		double* getDataXcalc(void) const { return m_dataXcalc; };
		
		double* getDataY(void) const { return m_dataY; };
		
		void setDataYim(double* _dataYim) { m_dataYim = _dataYim; };
		double* getDataYim(void) const { return m_dataYim; };
		
		double* getDataYcalc(void) const { return m_dataYcalc; };
		
		long getDataSize(void) const { return m_dataSize; };

		bool getDataAt(int _index, double& _x, double& _y);

		bool getData(double*& _x, double*& _y, long& _size);

		bool getYim(double*& _yim, long& _size);

		bool getCopyOfData(double*& _x, double*& _y, long& _size);

		bool getCopyOfYim(double*& _yim, long& _size);

	private:
		void memFree(void);

		double* m_dataX;
		double* m_dataXcalc;
		double* m_dataY;
		double* m_dataYim;
		double* m_dataYcalc;
		long    m_dataSize;
	};

}