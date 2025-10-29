// @otlicense

//! @file PolarPlotCurve.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qwt header
#include <qwt_polar_curve.h>

// Qt header
#include <QtGui/qpen.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PolarPlotCurve : public QwtPolarCurve {
	public:

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor

		PolarPlotCurve(const QString& _title = QString());

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setOutlinePen(const QPen& _pen) { m_outlinePen = _pen; };
		const QPen& getOutlinePen() const { return m_outlinePen; };

		void setPointInterval(int _interval) { m_pointInterval = _interval; };
		int getPointInterval() const { return m_pointInterval; };

	private:
		QPen m_outlinePen;
		int m_pointInterval;
	};

}