#pragma once
#pragma warning(disable : 4251)

#include <string>
#include <list>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>

#include "EntityProperties.h"
#include "ModelState.h"
#include "OldTreeIcon.h"

class EntityBase;
class ClassFactoryHandler;

class  __declspec(dllexport) EntityObserver
{
public:
	EntityObserver() {};
	virtual ~EntityObserver() {};

	virtual void entityRemoved(EntityBase *entity) {};
	virtual void entityModified(EntityBase *entity) {};

	virtual void sendMessageToViewer(ot::JsonDocument& doc) { std::list<std::pair<ot::UID, ot::UID>> prefetchIds;  sendMessageToViewer(doc, prefetchIds); };
	virtual void sendMessageToViewer(ot::JsonDocument& doc, std::list<std::pair<ot::UID, ot::UID>> &prefetchIds) {};

};

//! The Entity class is the base class for model entities and provides basic access properties to model entities. 
class  __declspec(dllexport) EntityBase
{
public:
	EntityBase(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler *factory, const std::string &owner);
	virtual ~EntityBase();

	static void setUidGenerator(DataStorageAPI::UniqueUIDGenerator *_uidGenerator);
	static DataStorageAPI::UniqueUIDGenerator *getUidGenerator(void);

	void		setName(std::string n) { name = n; setModified(); };
	std::string getName(void)  const { return name; };
	//! \brief Returns the name of the entity without the parent entity names.
	//! If the name is "root/entity" then the function will return "entity".
	std::string getNameOnly() const; 

	void         setEntityID(ot::UID id) { entityID = id;  setModified(); };
	ot::UID getEntityID(void) const { return entityID; };

	//void         setEntityStorageVersion(ot::UID version) { entityStorageVersion = version; };
	ot::UID getEntityStorageVersion(void) const { return entityStorageVersion; };

	void setInitiallyHidden(bool flag) { initiallyHidden = flag; };
	bool getInitiallyHidden(void) { return initiallyHidden; };

	void setEditable(bool flag) { isEditable = flag; }
	bool getEditable(void) { return isEditable; }

	void setSelectChildren(bool flag) { selectChildren = flag; }
	bool getSelectChildren(void) { return selectChildren; }

	void setManageParentVisibility(bool flag) { manageParentVisibility = flag; }
	bool getManageParentVisibility(void) { return manageParentVisibility; }

	void setManageChildVisibility(bool flag) { manageChildVisibility = flag; }
	bool getManageChildVisibility(void) { return manageChildVisibility; }

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) = 0;

	virtual void removeChild(EntityBase *child) {};

	virtual bool updateFromProperties(void);

	void setModelState(ModelState *ms) { modelState = ms; };
	ModelState *getModelState(void) { return modelState; };

	EntityBase *getParent(void) { return parentEntity; };
	void setParent(EntityBase *parent) { parentEntity = parent; };  // The parent information is not persistent. Changing it therefore does not set the modified flag for the entity.

	void setObserver(EntityObserver *obs) { observer = obs; };
	EntityObserver *getObserver(void) { return observer; };

	ClassFactoryHandler *getClassFactory(void) { return classFactory; };

	EntityProperties &getProperties(void) { return properties; };

	virtual EntityBase *getEntityFromName(const std::string &n) { if (name == n) return this; return nullptr; };

	virtual bool considerForPropertyFilter(void) { return true; };
	virtual bool considerChildrenForPropertyFilter(void) { return true; };

	virtual void StoreToDataBase(void);
	virtual void StoreToDataBase(ot::UID givenEntityVersion);

	void setModified(void);
	void resetModified(void) { isModified = false; };
	bool getModified(void) { return (isModified || properties.anyPropertyNeedsUpdate()); };

	void restoreFromDataBase(EntityBase *parent, EntityObserver *obs, ModelState *ms, bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap);

	virtual void addVisualizationNodes(void) {};

	virtual std::string getClassName(void) { return "EntityBase"; };

	virtual void addPrefetchingRequirementsForTopology(std::list<ot::UID> &prefetchIds);

	enum entityType {TOPOLOGY, DATA};
	virtual entityType getEntityType(void) = 0;

	void setOwningService(const std::string &owner) { owningService = owner; };
	const std::string &getOwningService(void) { return owningService; };

	virtual void detachFromHierarchy(void);

	void setDeletable(bool deletable) { isDeletable = deletable; };
	const bool deletable() const { return isDeletable; }

protected:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage) {};
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap);

	ot::UID createEntityUID(void);
	EntityBase *readEntityFromEntityID(EntityBase *parent, ot::UID entityID, std::map<ot::UID, EntityBase *> &entityMap);
	EntityBase *readEntityFromEntityIDAndVersion(EntityBase *parent, ot::UID entityID, ot::UID version, std::map<ot::UID, EntityBase *> &entityMap, ClassFactoryHandler* factory = nullptr);
	ot::UID getCurrentEntityVersion(ot::UID entityID);
	void entityIsStored(void);

private:
	// Persistent attributes
	std::string		    name;
	ot::UID				    entityID;
	ot::UID				    entityStorageVersion;
	bool                initiallyHidden;
	bool				isEditable;
	bool				isDeletable = true;
	bool				selectChildren;
	bool				manageParentVisibility;
	bool				manageChildVisibility;
	EntityProperties    properties;
	std::string			owningService;

	// Temporary attributes
	EntityBase         *parentEntity;
	EntityObserver     *observer;
	bool			    isModified;
	ModelState		   *modelState;
	ClassFactoryHandler *classFactory;
};

