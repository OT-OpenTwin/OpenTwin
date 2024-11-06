//! @file SceneNodePlot1DCurve.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "SceneNodePlot1DCurve.h"

SceneNodePlot1DCurve::SceneNodePlot1DCurve(ot::PlotDataset* _dataset)
	: m_dataset(_dataset), m_modelEntityVersion(0)
{

}

SceneNodePlot1DCurve::~SceneNodePlot1DCurve() {

}