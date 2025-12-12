// @otlicense
// File: SelectionHandler.h
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

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "EntityInformation.h"

// Model header
#include "SelectionChangedObserver.h"

// std header
#include <map>
#include <list>
#include <mutex>
#include <atomic>
#include <string>

namespace std { class thread; }

class SelectionHandler
{
public:
	SelectionHandler();
	~SelectionHandler();
	
	//The handler may be processing some notification in another thread. Moving or copying a thread state is not recommendable
	SelectionHandler(const SelectionHandler& _other) = delete; 
	SelectionHandler(SelectionHandler&& _other) = delete; 
	SelectionHandler& operator=(const SelectionHandler& _other) = delete;
	SelectionHandler& operator=(SelectionHandler&& _other) = delete;

	void processSelectionChanged(const std::list<ot::UID>& _selectedEntityIDs, const std::list<ot::UID>&  _selectedVisibleEntityIDs);
	void clearAllBufferAndNotify();
	void clearAllBuffer();
	void deselectEntity(ot::UID _entityID);

	std::list<ot::UID> getSelectedEntityIDs();
	std::list<ot::UID> getSelectedVisibleEntityIDs();

	void subscribe(SelectionChangedObserver* _observer);

	void getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator);

private:
	typedef std::map<std::string, std::list<ot::EntityInformation>> OwnerEntityMap;

	std::list<SelectionChangedObserver*> m_observer;
	std::mutex m_changeSelectedEntitiesBuffer;

	std::map<std::string,std::list<ot::EntityInformation>> m_ownersWithSelection;
	std::list<ot::UID> m_selectedEntityIDs;
	std::list<ot::UID> m_selectedVisibleEntityIDs;

	std::atomic_bool m_notifyOwnerThreadRunning;
	std::atomic_bool m_needNotifyOwner;
	std::mutex m_ownerNotifyMutex;
	OwnerEntityMap m_ownerNotifyMap;
	std::thread* m_ownerNotifyThread;

	void toggleButtonEnabledState();
	
	void notifyObservers(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons);
	void notifyOwners();
	void notifyOwnerWorker();

	void setSelectedEntityIDs(const std::list<ot::UID>& _selectedEntityIDs, const std::list<ot::UID>& _selectedVisibleEntityIDs);
};
