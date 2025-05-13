#pragma once
#include <list>
#include <atomic>
#include <string>
#include <map>
#include <mutex>

#include "OTCore/CoreTypes.h"
#include "SelectionChangedObserver.h"

class SelectionHandler
{
public:
	SelectionHandler() = default;
	~SelectionHandler() = default;
	
	//The handler may be processing some notification in another thread. Moving or copying a thread state is not recommendable
	SelectionHandler(const SelectionHandler& _other) = delete; 
	SelectionHandler(SelectionHandler&& _other) = delete; 
	SelectionHandler& operator=(const SelectionHandler& _other) = delete;
	SelectionHandler& operator=(SelectionHandler&& _other) = delete;

	void processSelectionChanged(const std::list<ot::UID>& _selectedEntityIDs, const std::list<ot::UID>&  _selectedVisibleEntityIDs);
	const std::atomic_bool& notificationInProcess() { return m_modelSelectionChangedNotificationInProgress; }
	void clearAllBufferAndNotify();
	void clearAllBuffer();
	void deselectEntity(ot::UID _entityID, const std::string& _owner);

	const std::list<ot::UID>& getSelectedEntityIDs();
	const std::list<ot::UID>& getSelectedVisibleEntityIDs();

	void subscribe(SelectionChangedObserver* _observer);
private:

	std::list<SelectionChangedObserver*> m_observer;
	std::atomic_bool m_modelSelectionChangedNotificationInProgress = false;
	std::mutex m_changeSelectedEntitiesBuffer;

	std::map<std::string,ot::UIDList> m_ownersWithSelection;
	std::list<ot::UID> m_selectedEntityIDs;
	std::list<ot::UID> m_selectedVisibleEntityIDs;

	void toggleButtonEnabledState();
	
	void notifyObservers(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons);
	void notifyOwners();
	void notifyOwnerThread(const std::map<std::string, std::list<ot::UID>>& _ownerEntityListMap);

	void setSelectedEntityIDs(const std::list<ot::UID>& _selectedEntityIDs, const std::list<ot::UID>& _selectedVisibleEntityIDs);
};
