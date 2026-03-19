// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/String.h"
#include "OTWidgets/WidgetTypes.h"

// Qwt header
#include <qwt_scale_draw.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PlotScaleDraw : public QwtScaleDraw
	{
		OT_DECL_NOCOPY(PlotScaleDraw)
		OT_DECL_NOMOVE(PlotScaleDraw)
	public:
		explicit PlotScaleDraw();
		virtual ~PlotScaleDraw() = default;

		virtual QwtText label(double _value) const override;

		void setNumberFormat(String::DisplayNumberFormat _format) { m_numberFormat = _format; };
		String::DisplayNumberFormat getNumberFormat() const { return m_numberFormat; };

		void setNumberPrecision(int _precision) { m_numberPrecision = _precision; };
		int getNumberPrecision() const { return m_numberPrecision; };

	private:
		String::DisplayNumberFormat m_numberFormat;
		int m_numberPrecision;
	};

}