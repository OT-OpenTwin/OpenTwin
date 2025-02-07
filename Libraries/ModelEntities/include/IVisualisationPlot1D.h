#pragma once
#include "OTGui/Plot1DCfg.h"

class IVisualisationPlot1D
{
public:
	virtual const ot::Plot1DCfg getPlot() = 0;
	virtual void setPlot(const ot::Plot1DCfg& _config) = 0;
	virtual bool visualisePlot() = 0;
};
