#include "stdafx.h"
#include "SelectionHandler.h"
#include "EntityMaterial.h"
#include "QueuingHttpRequestsRAII.h"
#include "Application.h"
#include "Model.h"

void SelectionHandler::processSelectionChanged(const std::list<ot::UID>& _selectedEntityIDs, const std::list<ot::UID>& _selectedVisibleEntityIDs)
{
	QueuingHttpRequestsRAII queueRequests;

	setSelectedEntityIDs(_selectedEntityIDs, _selectedVisibleEntityIDs);
	toggleButtonEnabledState();
	Application::instance()->getModel()->updatePropertyGrid();
	Application::instance()->flushRequestsToFrontEnd();
	//Order important to avoid a racing condition.
	notifyOwners(); 
}

void SelectionHandler::clearAllBufferAndNotify()
{
	ot::UIDList selectecEntities{}, selectedVisibleEntities{};
	processSelectionChanged(selectecEntities, selectedVisibleEntities);
}

void SelectionHandler::clearAllBuffer()
{
	m_selectedEntityIDs.clear();
	m_selectedVisibleEntityIDs.clear();
	m_ownersWithSelection.clear();
}

void SelectionHandler::deselectEntity(ot::UID _entityID, const std::string& _owner)
{
	std::lock_guard<std::mutex> guard(m_changeSelectedEntitiesBuffer);

	m_selectedEntityIDs.remove(_entityID);
	m_selectedVisibleEntityIDs.remove(_entityID);

	notifyOwners();
}

const std::list<ot::UID>& SelectionHandler::getSelectedEntityIDs()
{
	std::lock_guard<std::mutex> guard(m_changeSelectedEntitiesBuffer);
	return m_selectedEntityIDs;
}

const std::list<ot::UID>& SelectionHandler::getSelectedVisibleEntityIDs()
{
	std::lock_guard<std::mutex> guard(m_changeSelectedEntitiesBuffer);
	 return m_selectedVisibleEntityIDs; 
}

void SelectionHandler::subscribe(SelectionChangedObserver* _observer)
{
	m_observer.push_back(_observer);
}

void SelectionHandler::toggleButtonEnabledState()
{
	std::list<std::string> enabled;
	std::list<std::string> disabled;

	if (m_selectedEntityIDs.size() > 0)
	{
		enabled.push_back("Model:Edit:Delete");
	}
	else
	{
		disabled.push_back("Model:Edit:Delete");
	}

	Model* model = Application::instance()->getModel();
	std::list<EntityBase*> selectedEntities;
	for (auto entityID : m_selectedEntityIDs)
	{
		EntityBase* baseEntity = model->getEntityByID(entityID);

		if (baseEntity != nullptr)
		{
			selectedEntities.push_back(baseEntity);
		}
	}
	
	notifyObservers(selectedEntities,enabled,disabled);
	
	Application::instance()->getNotifier()->enableDisableControls(enabled, disabled);
}

void SelectionHandler::notifyObservers(std::list<EntityBase*>& _selectedEntities, std::list<std::string>& _enabledButtons, std::list<std::string>& _disabledButtons)
{
	for (SelectionChangedObserver* observer : m_observer)
	{
		observer->updatedSelection(_selectedEntities,_enabledButtons,_disabledButtons);
	}
}

void SelectionHandler::notifyOwners()
{
	std::map<std::string, std::list<ot::UID>> ownerEntityListMap;

	// All owners which were involved in the previous selection will receive a notification. 
	// Portentially, they receive an empty ID list, which says that their prior selected entity is now deselected.
	for (const auto& owner : m_ownersWithSelection)
	{
		ownerEntityListMap[owner.first] = {};
	}

	// Here we check which owners are part of the selection and assign their entities
	const std::string modelServiceName = Application::instance()->getServiceName();
	Model* model = Application::instance()->getModel();
	for (auto entityID : m_selectedEntityIDs)
	{
		const std::string ownerName = model->getEntityByID(entityID)->getOwningService();
		if (ownerName != modelServiceName)
		{
			ownerEntityListMap[ownerName].push_back(entityID);
		}
	}

	// Now we loop through all owners and send them their list of selected entities
	m_ownersWithSelection.clear();
	for (auto& ownerEntry : ownerEntityListMap)
	{
		if (!ownerEntry.second.empty())
		{
			m_ownersWithSelection[ownerEntry.first] = ownerEntry.second;
		}
	}

	if (!ownerEntityListMap.empty())
	{
		m_modelSelectionChangedNotificationInProgress = true;

		std::thread workerThread(&SelectionHandler::notifyOwnerThread, this, ownerEntityListMap);
		workerThread.detach();
	}
}

void SelectionHandler::notifyOwnerThread(const std::map<std::string, std::list<ot::UID>>& _ownerEntityListMap)
{
	for (auto owner : _ownerEntityListMap)
	{
		ot::JsonDocument notify;
		notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_SelectionChanged, notify.GetAllocator()), notify.GetAllocator());
		notify.AddMember(OT_ACTION_PARAM_MODEL_SelectedEntityIDs, ot::JsonArray(owner.second, notify.GetAllocator()), notify.GetAllocator());

		Application::instance()->getNotifier()->sendMessageToService(true, owner.first, notify);
	}

	m_modelSelectionChangedNotificationInProgress = false;
}

void SelectionHandler::setSelectedEntityIDs(const std::list<ot::UID>& _selectedEntityIDs, const std::list<ot::UID>& _selectedVisibleEntityIDs)
{
	std::lock_guard<std::mutex> guard(m_changeSelectedEntitiesBuffer);
	m_selectedEntityIDs = _selectedEntityIDs;
	m_selectedVisibleEntityIDs = _selectedVisibleEntityIDs;
}