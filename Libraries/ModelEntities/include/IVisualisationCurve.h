#pragma once
#include "OTGui/Plot1DCurveCfg.h"

class IVisualisationCurve
{
	public:
		virtual bool visualiseCurve() = 0;
		virtual ot::Plot1DCurveCfg getCurve() = 0;
		virtual void setCurve(const ot::Plot1DCurveCfg& _curve) = 0;
};
