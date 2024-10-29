#pragma once
#include <list>
#include <atomic>
#include <string>
#include <map>

#include "OTCore/CoreTypes.h"
#include "IVisualisationText.h"

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
	void clearAllBuffer();
	void deselectEntity(ot::UID _entityID, const std::string& _owner);

	const std::list<ot::UID>& getSelectedEntityIDs() { return m_selectedEntityIDs; }
	const std::list<ot::UID>& getSelectedVisibleEntityIDs() { return m_selectedVisibleEntityIDs; }
private:
	std::atomic_bool m_modelSelectionChangedNotificationInProgress = false;
	std::map<std::string,ot::UIDList> m_ownersWithSelection;
	std::list<ot::UID> m_selectedEntityIDs;
	std::list<ot::UID> m_selectedVisibleEntityIDs;

	void toggleButtonEnabledState();
	bool anyMaterialItemSelected();
	void notifyOwners();
	void createVisualisationRequests();
	void notifyOwnerThread(const std::map<std::string, std::list<ot::UID>>& _ownerEntityListMap);

	void sendTextVisualisationRequest(IVisualisationText* _textVisEntity, const std::string& _ownerServiceName);
};
