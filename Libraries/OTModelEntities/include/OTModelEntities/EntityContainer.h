// @otlicense
// File: EntityContainer.h
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

#include "OTModelEntities/EntityBase.h"

#include <list>

class __declspec(dllexport) EntityContainer : public EntityBase
{
public:
	static ot::EntityTreeItem createDefaultTreeItem();

	EntityContainer() : EntityContainer(0, nullptr, nullptr, nullptr) {};
	EntityContainer(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
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

	bool replaceChild(EntityBase* _child);
	void takeOverChildren(EntityContainer* _other);

	const std::list<EntityBase *>& getChildrenList();

	//! @brief Returns a list of all children of the container recursivly.
	std::list<EntityBase*> getAllChildren() const;

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual EntityBase *getEntityFromName(const std::string &n) override;

	virtual bool considerForPropertyFilter() const override { return false; };
	virtual bool considerChildrenForPropertyFilter() const override { return true; };

	virtual void storeToDataBase() override;

	virtual void addVisualizationNodes() override;

	static std::string className() { return "EntityContainer"; };
	virtual std::string getClassName() const override { return EntityContainer::className(); } ;

	virtual void addPrefetchingRequirementsForTopology(std::list<ot::UID> &prefetchIds) override;

	virtual entityType getEntityType() const override;

	void setCreateVisualizationItem(bool flag);
	bool getCreateVisualizationItem() { return m_createVisualizationItem; };

	virtual void detachFromHierarchy() override;

	virtual std::string serialiseAsJSON() override;
	virtual bool deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, const ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept;
	
	virtual void fillContextMenu(const ot::MenuRequestData* _requestData, ot::MenuCfg& _menuCfg) override;

protected:
	virtual int getSchemaVersion() override  { return 1; } ;
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;


private:
	std::list<EntityBase *> m_children;
	bool m_createVisualizationItem;
};



