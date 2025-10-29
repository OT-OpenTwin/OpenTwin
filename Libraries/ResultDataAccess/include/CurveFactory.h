// @otlicense

#pragma once
#include "MetadataSeries.h"
#include "EntityResult1DCurve.h"
#include "OTGui/Plot1DCurveCfg.h"
class __declspec(dllexport) CurveFactory
{
public:
	static void addToConfig(const MetadataSeries& _series, ot::Plot1DCurveCfg& _config);

private:
	static const std::string createQuery(ot::UID _seriesID);
	static const std::string createProjection();
};
