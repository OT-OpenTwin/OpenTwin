#pragma once
#include <string>

#include "OTCore/CoreTypes.h"
#include "OTGui/TableRange.h"
#include <OTCore/Color.h>

class RangleSelectionVisualisationHandler
{
public: 
	void selectRange(ot::UIDList iDs, ot::UIDList versions);

private:
	void requestToOpenTable(const std::string& _tableName);
	void requestColouringRanges(bool _clearSelection, const std::string& _tableName, const ot::Color& _colour, const std::list<ot::TableRange>& ranges);
};
