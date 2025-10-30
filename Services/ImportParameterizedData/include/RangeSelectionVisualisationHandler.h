// @otlicense
// File: RangeSelectionVisualisationHandler.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
