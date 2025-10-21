#pragma once

#include "EntityBase.h"
#include "BlockEntitiesAPIExport.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Point2D.h"
#include "OTCore/BasicServiceInformation.h"
#include "EntityCoordinates2D.h"
#include "OldTreeIcon.h"

#include "Connector.h"

#include "OTGui/GraphicsConnectionCfg.h"
#include "OTGui/GraphicsFlowItemBuilder.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/CopyInformation.h"

class OT_BLOCKENTITIES_API_EXPORT EntityBlock : public EntityBase
{
public:
	EntityBlock(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);
	EntityBlock(const EntityBlock& _other) = default;
	EntityBlock(EntityBlock&& _other) = default;
	EntityBlock& operator=(const EntityBlock& _other) = default;
	EntityBlock& operator=(EntityBlock&& _other) = default;
	virtual ~EntityBlock();

	virtual entityType getEntityType(void) const override { return TOPOLOGY; }
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };
	virtual void addVisualizationNodes(void) override;

	ot::UID getCoordinateEntityID() const { return m_coordinate2DEntityID; }
	EntityCoordinates2D* getCoordinateEntity() { return m_coordinateEntity; }
	const std::string& getBlockTitle() const { return m_blockTitle; }
	
	//! @brief The name of the container below the graphics editor. This name need not be editable. 
	//! If the name is empty, it is assumed that the block exists directly below the graphics scene entity
	void setGraphicsScenePackageChildName(const std::string& _name) { m_graphicsScenePackageChildName = _name; }

	void setServiceInformation(const ot::BasicServiceInformation& info) { m_info = info; };
	ot::BasicServiceInformation getServiceInformation() const { return m_info; };

	void setCoordinateEntity(const EntityCoordinates2D& _coordinateEntity) { m_coordinate2DEntityID = _coordinateEntity.getEntityID(); };
	void setCoordinateEntityID(ot::UID coordinateEntityID) { m_coordinate2DEntityID = coordinateEntityID; };

	const std::map<std::string,ot::Connector>& getAllConnectorsByName() const { return m_connectorsByName; }
	const bool hasConnector(const std::string& connectorName) const { return m_connectorsByName.find(connectorName) != m_connectorsByName.end(); }

	virtual ot::GraphicsItemCfg* createBlockCfg() = 0;

	std::string createBlockHeadline();

	virtual std::string serialiseAsJSON() override;
	virtual bool deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept override;

	void createBlockItem();

protected:
	virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

	void addConnector(const ot::Connector& connector);
	void addConnector(ot::Connector&& connector);
	void removeConnector(const ot::Connector& connector);
	void clearConnectors();
	const std::map<std::string, ot::Connector>& getAllConnectors() const { return m_connectorsByName; }
	ot::Connector getConnectorByName(const std::string& _connectorName) const;
	size_t getConnectorCount() const { return m_connectorsByName.size(); }

	void createNavigationTreeEntry();
	void addConnectors(ot::GraphicsFlowItemBuilder& flowBlockBuilder);

	void setNavigationTreeIcon(const OldTreeIcon& icon) { m_navigationTreeIcon = icon; setModified(); };
	const OldTreeIcon& getNavigationTreeIcon() const { return m_navigationTreeIcon; };

	void setBlockTitle(const std::string& title) { m_blockTitle = title; setModified(); };

private:
	std::string m_blockTitle = "";
	ot::UID m_coordinate2DEntityID = 0;
	EntityCoordinates2D* m_coordinateEntity = nullptr;
	ot::BasicServiceInformation m_info;
	std::string	m_graphicsScenePackageChildName = "";

	OldTreeIcon m_navigationTreeIcon;

	std::map<std::string, ot::Connector> m_connectorsByName;
};
