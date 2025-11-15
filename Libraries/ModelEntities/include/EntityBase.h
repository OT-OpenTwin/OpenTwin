// @otlicense
// File: EntityBase.h
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
#pragma warning(disable : 4251)

// OpenTwin header
#include "ModelState.h"
#include "EntityProperties.h"
#include "EntityCallbackBase.h"
#include "EntityFactoryRegistrar.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/BasicEntityInformation.h"
#include "OTGui/EntityTreeItem.h"
#include "OTGui/CopyInformation.h"
#include "OTGui/VisualisationCfg.h"
#include "OTGui/VisualisationTypes.h"

// BSON header
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>

// std header
#include <map>
#include <list>
#include <string>

class EntityBase;

class OT_MODELENTITIES_API_EXPORT EntityObserver
{
public:
	EntityObserver() {};
	virtual ~EntityObserver() {};

	virtual void entityRemoved(EntityBase *entity) {};
	virtual void entityModified(EntityBase *entity) {};

	virtual void sendMessageToViewer(ot::JsonDocument& doc) { std::list<std::pair<ot::UID, ot::UID>> prefetchIds; sendMessageToViewer(doc, prefetchIds); };
	virtual void sendMessageToViewer(ot::JsonDocument& doc, std::list<std::pair<ot::UID, ot::UID>> &prefetchIds) {};
	virtual void requestConfigForModelDialog(const ot::UID& _entityID, const std::string _collectionType, const std::string& _targetFolder, const std::string& _elementType) {};
	virtual void requestVisualisation(ot::UID _entityID, ot::VisualisationCfg& _visualisationCfg) {};

};

//! @brief The Entity class is the base class for model entities and provides basic access properties to model entities. 
class  OT_MODELENTITIES_API_EXPORT EntityBase : public ot::EntityCallbackBase
{
public:
	EntityBase(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);
	EntityBase(const EntityBase& _other) = default;
	EntityBase(EntityBase&& _other) = default;

	virtual ~EntityBase();

	static void setUidGenerator(DataStorageAPI::UniqueUIDGenerator *_uidGenerator);
	static DataStorageAPI::UniqueUIDGenerator *getUidGenerator();

	void setName(const std::string& _name);
	std::string getName() const { return m_treeItem.getEntityName(); };

	//! @brief Returns the name of the entity without the parent entity names.
	//! If the name is "root/entity" then the function will return "entity".
	std::string getNameOnly() const;

	void setEntityID(ot::UID _id);
	ot::UID getEntityID() const { return m_treeItem.getEntityID(); };

	ot::UID getEntityStorageVersion() const { return m_treeItem.getEntityVersion(); };

	ot::BasicEntityInformation getBasicEntityInformation() const { return m_treeItem; };

	void setInitiallyHidden(bool flag) { m_initiallyHidden = flag; };
	bool getInitiallyHidden() { return m_initiallyHidden; };

	void setSelectChildren(bool flag) { m_selectChildren = flag; }
	bool getSelectChildren() { return m_selectChildren; }

	void setManageParentVisibility(bool flag) { m_manageParentVisibility = flag; }
	bool getManageParentVisibility() { return m_manageParentVisibility; }

	void setManageChildVisibility(bool flag) { m_manageChildVisibility = flag; }
	bool getManageChildVisibility() { return m_manageChildVisibility; }

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) = 0;

	virtual void removeChild(EntityBase *child) {};

	//! @brief Will be called whenever a property of the entity has changed.
	//! The function needs to update the entity accordingly.
	//! @return Returns true if the property grid needs to be updated.
	virtual bool updateFromProperties();

	void setModelState(ModelState *ms) { m_modelState = ms; };
	ModelState *getModelState() { return m_modelState; };

	EntityBase *getParent() { return m_parentEntity; };
	void setParent(EntityBase *parent) { m_parentEntity = parent; };  // The parent information is not persistent. Changing it therefore does not set the modified flag for the entity.

	void setObserver(EntityObserver *obs) { m_observer = obs; };
	EntityObserver *getObserver() { return m_observer; };

	EntityProperties& getProperties() { return m_properties; };
	const EntityProperties& getProperties() const { return m_properties; };

	virtual EntityBase *getEntityFromName(const std::string& _name) { return (m_treeItem.getEntityName() == _name ? this : nullptr); };

	virtual bool considerForPropertyFilter() { return true; };
	virtual bool considerChildrenForPropertyFilter() { return true; };

	virtual void storeToDataBase();
	virtual void storeToDataBase(ot::UID _givenEntityVersion);

	void setModified();
	void resetModified() { m_isModified = false; };
	bool getModified() { return (m_isModified || m_properties.anyPropertyNeedsUpdate()); };

	//! @brief Calls readSpecificDataFromDataBase to load entity specific data from the bson document. Topology entities are added to the entity map. 
	//! The map is intended to be the centralized map in the model service, which is only supposed to hold topology entities.
	void restoreFromDataBase(EntityBase *parent, EntityObserver *obs, ModelState *ms, bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap);

	virtual void addVisualizationNodes() {};

	virtual std::string getClassName() const { return "EntityBase"; };

	virtual void addPrefetchingRequirementsForTopology(std::list<ot::UID> &prefetchIds);

	enum entityType {TOPOLOGY, DATA};
	virtual entityType getEntityType() const = 0;

	virtual void detachFromHierarchy();

	void setDeletable(bool deletable) { m_isDeletable = deletable; };
	bool getDeletable() const { return m_isDeletable; }

	//! @brief Creates a copy of the entity. The override from instantiable classes need to set the observer=nullptr and the parent=nullptr. 
	//! Otherwise the originals are deleted from the modelstate, if the clone happens in the model service
	//! @return 
	virtual EntityBase* clone() { return nullptr; };

	//! @brief 
	//! @return Empty string if entity does not support copy. 
	virtual std::string serialiseAsJSON() { return ""; };

	//! @brief Entity specific (optional) implementation of a deserialisation from a string (copy/paste functionality). 
	//! In this function, it is necessary to create new IDs. Don't store the entity. Storing and unique name creation are taken care of by the model service.
	//! @param _copyInformation Additional information from the ui are located here. E.g. the cursor position.
	//! @param _entityMap Any entity that is deserialised in the process needs to be added to the map. This will be used for storing and adding to model state
	//! @return true if the serialisation was successfull
	virtual bool deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, const ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept
	{ 
		return false; 
	}


	void setTreeItem(const ot::EntityTreeItem& _treeItem, bool _resetTreeItemModified = false);
	const ot::EntityTreeItem& getTreeItem() const { return m_treeItem; };

	void setVisibleTreeItemIcon(const std::string& _icon) { m_treeItem.setVisibleIcon(_icon); };
	void setHiddenTreeItemIcon(const std::string& _icon) { m_treeItem.setHiddenIcon(_icon); };
	void setTreeItemIcons(const ot::NavigationTreeItemIcon& _icons) { m_treeItem.setIcons(_icons); };
	const ot::NavigationTreeItemIcon& getTreeItemIcons() const { return m_treeItem.getIcons(); };

	void setTreeItemEditable(bool _editable) { m_treeItem.setIsEditable(_editable); };
	bool getTreeItemEditable() { return m_treeItem.getIsEditable(); };

	void setTreeItemSelectChildren(bool _selectChildren) { m_treeItem.setSelectChilds(_selectChildren); };
	bool getTreeItemSelectChildren() { return m_treeItem.getSelectChilds(); };

	void setVisualizationTypes(const ot::VisualisationTypes& _types, bool _resetVisualizationTypesModified = false);
	const ot::VisualisationTypes& getVisualizationTypes() const { return m_visualizationTypes; };

protected:
	virtual void callbackDataChanged() override { setModified(); };

	virtual int getSchemaVersion() { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) {};
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap);

	ot::UID createEntityUID();
	EntityBase *readEntityFromEntityID(EntityBase *parent, ot::UID entityID, std::map<ot::UID, EntityBase *> &entityMap);
	EntityBase *readEntityFromEntityIDAndVersion(EntityBase *parent, ot::UID entityID, ot::UID version, std::map<ot::UID, EntityBase *> &entityMap);
	ot::UID getCurrentEntityVersion(ot::UID entityID);
	void entityIsStored();
	bsoncxx::builder::basic::document serialiseAsMongoDocument();

private:
	// Persistent attributes
	bool                    m_initiallyHidden;
	bool				    m_isDeletable;
	bool				    m_selectChildren;
	bool				    m_manageParentVisibility;
	bool				    m_manageChildVisibility;
	EntityProperties        m_properties;

	// Optionally persistent attributes
	ot::EntityTreeItem      m_treeItem;           // Holds name, id, version and tree item information (persistent), other info is stored when modified
	ot::VisualisationTypes  m_visualizationTypes; // Only stored when modified

	// Temporary attributes
	EntityBase*             m_parentEntity;
	EntityObserver*         m_observer;
	bool			        m_isModified;
	ModelState*             m_modelState;
};

OT_ADD_FLAG_FUNCTIONS(EntityBase::Callback, EntityBase::CallbackFlags)