// @otlicense
// File: SelectionHandler.cpp
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

	// Order important to avoid a racing condition.
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

void SelectionHandler::deselectEntity(ot::UID _entityID)
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

void SelectionHandler::getDebugInformation(ot::JsonObject& _object, ot::JsonAllocator& _allocator) {
	using namespace ot;
	_object.AddMember("ObserverCount", static_cast<uint64_t>(m_observer.size()), _allocator);

	JsonArray ownersWithSelectionArr;
	for (const auto& ownerEntry : m_ownersWithSelection) {
		JsonObject ownerObj;
		ownerObj.AddMember("Owner", JsonString(ownerEntry.first, _allocator), _allocator);
		JsonArray entityInfosArr;
		for (const auto& entityInfo : ownerEntry.second) {
			JsonObject entityInfoObj;
			entityInfo.addToJsonObject(entityInfoObj, _allocator);
			entityInfosArr.PushBack(entityInfoObj, _allocator);
		}
		ownerObj.AddMember("Entities", entityInfosArr,  _allocator);
		ownersWithSelectionArr.PushBack(ownerObj, _allocator);
	}
	_object.AddMember("OwnersWithSelection", ownersWithSelectionArr, _allocator);
	{
		std::lock_guard<std::mutex> guard(m_changeSelectedEntitiesBuffer);
		_object.AddMember("SelectedEntities", JsonArray(m_selectedEntityIDs, _allocator), _allocator);
		_object.AddMember("SelectedVisibleEntities", JsonArray(m_selectedVisibleEntityIDs, _allocator), _allocator);
	}
	
	_object.AddMember("NotifyOwnerThreadRunning", m_notifyOwnerThreadRunning.load(), _allocator);
	_object.AddMember("NeedNotifyOwner", m_needNotifyOwner.load(), _allocator);

	JsonArray ownerNotifyMapArr;
	{
		std::lock_guard<std::mutex> guard(m_ownerNotifyMutex);
		for (const auto& ownerEntry : m_ownerNotifyMap) {
			JsonObject ownerObj;
			ownerObj.AddMember("Owner", JsonString(ownerEntry.first, _allocator), _allocator);
			JsonArray entityInfosArr;
			for (const auto& entityInfo : ownerEntry.second) {
				JsonObject entityInfoObj;
				entityInfo.addToJsonObject(entityInfoObj, _allocator);
				entityInfosArr.PushBack(entityInfoObj, _allocator);
			}
			ownerObj.AddMember("Entities", entityInfosArr, _allocator);
			ownerNotifyMapArr.PushBack(ownerObj, _allocator);
		}
		_object.AddMember("OwnerNotifyMap", ownerNotifyMapArr, _allocator);
	}
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
		for (const std::string& cb : entity->getServicesForCallback(EntityBase::Callback::Selection)) {
			ownerEntityListMap[cb].push_back(ot::EntityInformation(entity));
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
		m_ownerNotifyMutex.lock();

		m_needNotifyOwner = true;
		m_ownerNotifyMap = std::move(ownerEntityListMap);

		m_ownerNotifyMutex.unlock();
	}
}

void SelectionHandler::notifyOwnerWorker()
{
	while (m_notifyOwnerThreadRunning) {
		if (m_needNotifyOwner) {
			OwnerEntityMap data;
			
			m_ownerNotifyMutex.lock();

			m_needNotifyOwner = false;
			data = std::move(m_ownerNotifyMap);
			
			m_ownerNotifyMutex.unlock();
			
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

				if (owner.first != OT_INFO_SERVICE_TYPE_MODEL) {
					Application::instance()->getNotifier()->sendMessageToService(true, owner.first, notify);
				}
			}
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