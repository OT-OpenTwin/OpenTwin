#pragma once
#pragma warning(disable : 4251)

#include <string>
#include <list>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>

#include "EntityProperties.h"
#include "ModelState.h"
#include "EntityFactoryRegistrar.h"
#include "OldTreeIcon.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/CopyInformation.h"
#include "OTGui/VisualisationCfg.h"

class EntityBase;

class  __declspec(dllexport) EntityObserver
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

//! \brief The Entity class is the base class for model entities and provides basic access properties to model entities. 
class  __declspec(dllexport) EntityBase
{
public:
	EntityBase(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);
	EntityBase(const EntityBase& _other) = default;
	EntityBase(EntityBase&& _other) = default;

	virtual ~EntityBase();

	static void setUidGenerator(DataStorageAPI::UniqueUIDGenerator *_uidGenerator);
	static DataStorageAPI::UniqueUIDGenerator *getUidGenerator();

	void setName(std::string n) { m_name = n; setModified(); };
	std::string getName() const { return m_name; };

	//! \brief Returns the name of the entity without the parent entity names.
	//! If the name is "root/entity" then the function will return "entity".
	std::string getNameOnly() const;

	void setEntityID(ot::UID id) { m_entityID = id; setModified(); };
	ot::UID getEntityID() const { return m_entityID; };

	ot::UID getEntityStorageVersion() const { return m_entityStorageVersion; };

	void setInitiallyHidden(bool flag) { m_initiallyHidden = flag; };
	bool getInitiallyHidden() { return m_initiallyHidden; };

	void setEditable(bool flag) { m_isEditable = flag; }
	bool getEditable() { return m_isEditable; }

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

	EntityProperties &getProperties() { return m_properties; };

	virtual EntityBase *getEntityFromName(const std::string &n) { if (m_name == n) return this; return nullptr; };

	virtual bool considerForPropertyFilter() { return true; };
	virtual bool considerChildrenForPropertyFilter() { return true; };

	virtual void storeToDataBase();
	virtual void storeToDataBase(ot::UID givenEntityVersion);

	void setModified();
	void resetModified() { m_isModified = false; };
	bool getModified() { return (m_isModified || m_properties.anyPropertyNeedsUpdate()); };

	void restoreFromDataBase(EntityBase *parent, EntityObserver *obs, ModelState *ms, bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap);

	virtual void addVisualizationNodes() {};

	virtual std::string getClassName() { return "EntityBase"; };

	virtual void addPrefetchingRequirementsForTopology(std::list<ot::UID> &prefetchIds);

	enum entityType {TOPOLOGY, DATA};
	virtual entityType getEntityType() const = 0;

	void setOwningService(const std::string &owner) { m_owningService = owner; };
	const std::string &getOwningService() { return m_owningService; };

	virtual void detachFromHierarchy();

	void setDeletable(bool deletable) { m_isDeletable = deletable; };
	const bool deletable() const { return m_isDeletable; }

	//! @brief Creates a copy of the entity. The override from instantiable classes need to set the observer=nullptr and the parent=nullptr. 
	//! Otherwise the originals are deleted from the modelstate, if the clone happens in the model service
	//! @return 
	virtual EntityBase* clone() { return nullptr; };

	//! @brief 
	//! @return Empty string if entity does not support copy. 
	virtual std::string serialiseAsJSON() { return ""; };

	//! @brief Entity specific (optional) implementation of a deserialisation fro a string (copy/paste functionality). 
	//! In this function, it is necessary to create new IDs. Don't store the entity. Storing and unique name creation are taken care of by the model service.
	//! @param _copyInformation Additional information from the ui are located here. E.g. the cursor position.
	//! @param _entityMap Any entity that is deserialised in the process needs to be added to the map. This will be used for storing and adding to model state
	//! @return true if the serialisation was successfull
	virtual bool deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept
	{ 
		return false; 
	}

protected:
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
	std::string		     m_name;
	ot::UID              m_entityID;
	ot::UID				 m_entityStorageVersion;
	bool                 m_initiallyHidden;
	bool			 	 m_isEditable;
	bool				 m_isDeletable;
	bool				 m_selectChildren;
	bool				 m_manageParentVisibility;
	bool				 m_manageChildVisibility;
	EntityProperties     m_properties;
	std::string			 m_owningService;

	// Temporary attributes
	EntityBase*          m_parentEntity;
	EntityObserver*      m_observer;
	bool			     m_isModified;
	ModelState*          m_modelState;
};

