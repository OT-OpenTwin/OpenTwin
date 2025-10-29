// @otlicense

#pragma once
#include <string>
#include <memory>

#include "OTCore/CoreTypes.h"
#include "OTGui/TableRange.h"
#include "EntityTableSelectedRanges.h"
#include "OTCore/Color.h"


class RangeSelectionVisualisationHandler
{
public: 
	void selectRange(const ot::UIDList& _selectedEntityIDs);

private:
	std::list<ot::UID> m_bufferedSelectionRanges;

	void bufferSelectionEntities(const std::list<std::shared_ptr<EntityTableSelectedRanges>>& _selectedEntities);
	std::list<std::shared_ptr<EntityTableSelectedRanges>> extractSelectionRanges(const ot::UIDList& _selectedEntityIDs);
	const std::list<std::shared_ptr<EntityTableSelectedRanges>> findSelectionsThatNeedVisualisation(const std::list<std::shared_ptr<EntityTableSelectedRanges>>& _selectedEntities);
	bool requestToOpenTable(const std::string& _tableName, const ot::UIDList& _visualizingEntities);
	void requestColouringRanges(bool _clearSelection, const std::string& _tableName, const ot::Color& _colour, const std::list<ot::TableRange>& ranges);
};
