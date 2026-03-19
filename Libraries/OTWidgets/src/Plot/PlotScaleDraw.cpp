// @otlicense

// OpenTwin header
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/PlotScaleDraw.h"

// Qwt header
#include <qwt_text.h>

ot::PlotScaleDraw::PlotScaleDraw()
	: m_numberFormat(String::DisplayNumberFormat::Auto), m_numberPrecision(3)
{

}

QwtText ot::PlotScaleDraw::label(double _value) const
{
	return QwtText(QString::fromStdString(String::numberToString(_value, m_numberFormat, m_numberPrecision)));
}
