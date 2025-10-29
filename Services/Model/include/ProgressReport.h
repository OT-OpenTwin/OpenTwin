// @otlicense

#pragma once

#include <string>

namespace ProgressReport
{
	void setProgressInformation(std::string message, bool continuous);
	void setProgress(int percentage);
	void closeProgressInformation(void);
}
