// @otlicense
#pragma once
#include "OTCore/CoreAPIExport.h"

enum class PythonEventType
{
	Unknown,
	PropertyChanged,
	Delete,
	Execute,
	Done,
	TableFilterChanged
};
