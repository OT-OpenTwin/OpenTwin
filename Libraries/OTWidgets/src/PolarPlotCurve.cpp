// @otlicense

// OpenTwin header
#include "OTWidgets/PolarPlotCurve.h"

ot::PolarPlotCurve::PolarPlotCurve(const QString& _title) :
	QwtPolarCurve(_title), m_outlinePen(Qt::NoPen), m_pointInterval(1)
{

}
