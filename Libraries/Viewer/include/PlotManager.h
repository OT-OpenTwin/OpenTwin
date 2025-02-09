//! @file PlotManager.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/Plot.h"

namespace ot {

	class __declspec(dllexport) PlotManager : public Plot {
	public:
		PlotManager();
		virtual ~PlotManager();

	protected:
		virtual void importData(const std::string& _projectName, const std::list<Plot1DCurveCfg>& _curvesToImport) override;

	private:

	};

}