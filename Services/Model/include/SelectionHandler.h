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
	bool notificationInProcess() { return m_modelSelectionChangedNotificationInProgress || m_needNotifyOwner; }
	void clearAllBufferAndNotify();
	void clearAllBuffer();
	void deselectEntity(ot::UID _entityID, const std::string& _owner);

	std::list<ot::UID> getSelectedEntityIDs();
	std::list<ot::UID> getSelectedVisibleEntityIDs();

	void subscribe(SelectionChangedObserver* _observer);
private:
	typedef std::map<std::string, std::list<ot::EntityInformation>> OwnerEntityMap;

	std::list<SelectionChangedObserver*> m_observer;
	std::atomic_bool m_modelSelectionChangedNotificationInProgress = false;
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
