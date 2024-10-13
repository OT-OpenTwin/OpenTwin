//! @file PlotManager.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PlotView.h"

namespace ot {

	class __declspec(dllexport) PlotManager : public PlotView {
	public:
		PlotManager();
		virtual ~PlotManager();

	protected:
		virtual void importData(const std::string& _projectName, const std::list<Plot1DCurveInfoCfg>& _curvesToImport) override;

	private:

	};

}