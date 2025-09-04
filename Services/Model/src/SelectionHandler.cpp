#include "stdafx.h"
#include "SelectionHandler.h"
#include "EntityMaterial.h"
#include "QueuingHttpRequestsRAII.h"
#include "Application.h"
#include "Model.h"

SelectionHandler::SelectionHandler() :
	m_needNotifyOwner(false), m_ownerNotifyThread(nullptr), m_notifyOwnerThreadRunning(true)
{
	m_ownerNotifyThread = new std::thread(&SelectionHandler::notifyOwnerWorker, this);
}

SelectionHandler::~SelectionHandler() {
	m_needNotifyOwner = false;
	m_notifyOwnerThreadRunning = false;

	if (m_ownerNotifyThread) {
		if (m_ownerNotifyThread->joinable()) {
			m_ownerNotifyThread->join();
		}
		delete m_ownerNotifyThread;
		m_ownerNotifyThread = nullptr;
	}
}

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

std::list<ot::UID> SelectionHandler::getSelectedEntityIDs()
{
	std::lock_guard<std::mutex> guard(m_changeSelectedEntitiesBuffer);
	return m_selectedEntityIDs;
}

std::list<ot::UID> SelectionHandler::getSelectedVisibleEntityIDs()
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
	OwnerEntityMap ownerEntityListMap;
	ot::EntityInformation entityInfos;

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
		EntityBase* entity = model->getEntityByID(entityID);
		const std::string ownerName = entity->getOwningService();
		if (ownerName != modelServiceName)
		{
			ownerEntityListMap[ownerName].push_back(ot::EntityInformation(entity));
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
		std::lock_guard<std::mutex> lock(m_ownerNotifyMutex);
		m_needNotifyOwner = true;
		m_ownerNotifyMap = std::move(ownerEntityListMap);
	}
}

void SelectionHandler::notifyOwnerWorker()
{
	while (m_notifyOwnerThreadRunning) {
		if (m_needNotifyOwner) {
			OwnerEntityMap data;
			{
				std::lock_guard<std::mutex> lock(m_ownerNotifyMutex);
				m_needNotifyOwner = false;
				m_modelSelectionChangedNotificationInProgress = true;
				data = std::move(m_ownerNotifyMap);
			}
			
			for (const auto& owner : data) {
				ot::JsonDocument notify;
				notify.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_SelectionChanged, notify.GetAllocator()), notify.GetAllocator());

				ot::UIDList entityIDs;
				ot::JsonArray entityInfos;
				for (const ot::EntityInformation& entityInfo : owner.second) {
					ot::JsonObject entry;
					entityInfo.addToJsonObject(entry, notify.GetAllocator());
					entityInfos.PushBack(entry, notify.GetAllocator());
					entityIDs.push_back(entityInfo.getEntityID());
				}
				notify.AddMember(OT_ACTION_PARAM_MODEL_EntityInfo, entityInfos, notify.GetAllocator());

				Application::instance()->getNotifier()->sendMessageToService(true, owner.first, notify);
			}

			m_modelSelectionChangedNotificationInProgress = false;
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

void SelectionHandler::setSelectedEntityIDs(const std::list<ot::UID>& _selectedEntityIDs, const std::list<ot::UID>& _selectedVisibleEntityIDs)
{
	std::lock_guard<std::mutex> guard(m_changeSelectedEntitiesBuffer);
	m_selectedEntityIDs = _selectedEntityIDs;
	m_selectedVisibleEntityIDs = _selectedVisibleEntityIDs;
}