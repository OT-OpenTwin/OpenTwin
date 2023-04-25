#pragma once

#include <string>

namespace ProgressReport
{
	void setProgressInformation(std::string message, bool continuous);
	void setProgress(int percentage);
	void closeProgressInformation(void);

	enum lockType {ANY_OPERATION, MODEL_CHANGE};
	void setUILock(bool flag, lockType type);
}
