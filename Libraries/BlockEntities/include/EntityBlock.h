#pragma once
#include "EntityBase.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Point2D.h"
#include "OTCore/BasicServiceInformation.h"
#include "EntityCoordinates2D.h"

#include "Connector.h"

#include "OTGui/GraphicsPickerCollectionCfg.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTGui/GraphicsFlowItemBuilder.h"
#include "OTGui/GraphicsItemCfg.h"

class __declspec(dllexport) EntityBlock : public EntityBase
{
public:
	EntityBlock(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	EntityBlock(const EntityBlock& _other) = default;
	EntityBlock(EntityBlock&& _other) = default;
	EntityBlock& operator=(const EntityBlock& _other) = default;
	EntityBlock& operator=(EntityBlock&& _other) = default;
	virtual ~EntityBlock();

	virtual std::string getClassName(void) override { return "EntityBlock"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };
	virtual void addVisualizationNodes(void) override;

	ot::UID getCoordinateEntityID() const { return _coordinate2DEntityID; }
	const std::string& getBlockTitle() const { return _blockTitle; }
	void SetGraphicsScenePackageName(const std::string& name) { _graphicsScenePackage = name; }
	void SetServiceInformation(const ot::BasicServiceInformation& info) { _info = info; }
	void setCoordinateEntityID(ot::UID coordinateEntityID) { _coordinate2DEntityID = coordinateEntityID; };

	const std::map<std::string,ot::Connector>& getAllConnectorsByName() const { return _connectorsByName; }
	const bool hasConnector(const std::string& connectorName) const { return _connectorsByName.find(connectorName) != _connectorsByName.end(); }
	const std::list<ot::UID>& getAllConnections() const { return _connectionIDs; }

	void AddConnector(const ot::Connector& connector);
	void RemoveConnector(const ot::Connector& connector);

	void AddConnection(const ot::UID id);
	void RemoveConnection(const ot::UID idForRemoval);

	virtual ot::GraphicsItemCfg* CreateBlockCfg() = 0;

	std::string CreateBlockHeadline();

protected:
	std::string _blockTitle = "";
	ot::UID _coordinate2DEntityID = 0;
	ot::BasicServiceInformation _info;
	std::string	_graphicsScenePackage;
	std::string _navigationOldTreeIconName = "";
	std::string _navigationOldTreeIconNameHidden = "";

	std::map<std::string,ot::Connector> _connectorsByName;
	//std::map<std::string, ot::BlockConnection> _connectionsByConnectionKey;
	std::list<ot::UID> _connectionIDs;

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

	void CreateNavigationTreeEntry();
	void CreateBlockItem();
	void AddConnectors(ot::GraphicsFlowItemBuilder& flowBlockBuilder);
};
