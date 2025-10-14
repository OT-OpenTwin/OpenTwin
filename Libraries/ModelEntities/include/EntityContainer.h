#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "OldTreeIcon.h"

#include <list>

class __declspec(dllexport) EntityContainer : public EntityBase
{
public:
	EntityContainer() : EntityContainer(0, nullptr, nullptr, nullptr, "") {};
	EntityContainer(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityContainer();

	/**
	 * Only for model service. For other services it is enough to use the addEntityToModel from modelcomponent. The modelservice adds the children by itself.
	 * 
	 * \param child
	 */
	virtual void addChild(EntityBase *child);
	/**
	 * Only for model service.
	 *
	 * \param child
	 */
	virtual void removeChild(EntityBase *child) override;

	const std::list<EntityBase *> &getChildrenList(void);

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual EntityBase *getEntityFromName(const std::string &n) override;

	virtual bool considerForPropertyFilter(void) override { return false; };
	virtual bool considerChildrenForPropertyFilter(void) override { return true; };

	virtual void StoreToDataBase(void) override;

	virtual void addVisualizationNodes(void) override;

	virtual std::string getClassName(void) override { return "EntityContainer"; } ;

	virtual void addPrefetchingRequirementsForTopology(std::list<ot::UID> &prefetchIds) override;

	virtual entityType getEntityType(void) const override;

	void setCreateVisualizationItem(bool flag);
	bool getCreateVisualizationItem(void) { return createVisualizationItem; }

	virtual void detachFromHierarchy(void) override;

	void setTreeIcon(const OldTreeIcon& icon) { m_treeIcon = icon; };
	const OldTreeIcon& getTreeIcon(void) const { return m_treeIcon; };

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;


private:
	OldTreeIcon m_treeIcon;
	std::list<EntityBase *> children;
	bool createVisualizationItem;
};



