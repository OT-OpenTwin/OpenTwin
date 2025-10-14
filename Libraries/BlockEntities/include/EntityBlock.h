#pragma once
#include "EntityBase.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Point2D.h"
#include "OTCore/BasicServiceInformation.h"
#include "EntityCoordinates2D.h"

#include "Connector.h"

#include "OTGui/GraphicsConnectionCfg.h"
#include "OTGui/GraphicsFlowItemBuilder.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/CopyInformation.h"

class __declspec(dllexport) EntityBlock : public EntityBase
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

	void SetServiceInformation(const ot::BasicServiceInformation& info) { m_info = info; }
	void setCoordinateEntityID(ot::UID coordinateEntityID) { m_coordinate2DEntityID = coordinateEntityID; };

	const std::map<std::string,ot::Connector>& getAllConnectorsByName() const { return m_connectorsByName; }
	const bool hasConnector(const std::string& connectorName) const { return m_connectorsByName.find(connectorName) != m_connectorsByName.end(); }
	const std::list<ot::UID>& getAllConnections() const { return m_connectionIDs; }

	void AddConnector(const ot::Connector& connector);
	void RemoveConnector(const ot::Connector& connector);

	void AddConnection(const ot::UID id);
	void RemoveConnection(const ot::UID idForRemoval);

	virtual ot::GraphicsItemCfg* CreateBlockCfg() = 0;

	std::string CreateBlockHeadline();

	virtual std::string serialiseAsJSON() override;
	virtual bool deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept override;

	void CreateBlockItem();


protected:
	std::string m_blockTitle = "";
	ot::UID m_coordinate2DEntityID = 0;
	EntityCoordinates2D* m_coordinateEntity = nullptr;
	ot::BasicServiceInformation m_info;
	std::string	m_graphicsScenePackageChildName = "";
	std::string m_navigationOldTreeIconName = "";
	std::string m_navigationOldTreeIconNameHidden = "";

	std::map<std::string,ot::Connector> m_connectorsByName;
	//std::map<std::string, ot::BlockConnection> _connectionsByConnectionKey;
	std::list<ot::UID> m_connectionIDs;

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

	void CreateNavigationTreeEntry();
	void AddConnectors(ot::GraphicsFlowItemBuilder& flowBlockBuilder);
};
