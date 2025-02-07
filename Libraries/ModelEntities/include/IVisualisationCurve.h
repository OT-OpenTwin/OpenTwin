#pragma once
#include "OTGui/Plot1DCurveInfoCfg.h"

class IVisualisationCurve
{
	public:
		virtual bool visualiseCurve() = 0;
		virtual ot::Plot1DCurveInfoCfg getCurve() = 0;
		virtual void setCurve(const ot::Plot1DCurveInfoCfg& _curve) = 0;
};
