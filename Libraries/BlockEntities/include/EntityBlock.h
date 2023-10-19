#pragma once
#include "EntityBase.h"
#include "OpenTwinCore/CoreTypes.h"
#include "OpenTwinCore/Point2D.h"
#include "OpenTwinCore/BasicServiceInformation.h"
#include "EntityCoordinates2D.h"

#include "Connector.h"
#include "BlockConnection.h"

#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/GraphicsFlowItemCfg.h"

class __declspec(dllexport) EntityBlock : public EntityBase
{
public:
	EntityBlock(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);

	virtual std::string getClassName(void) override { return "EntityBlock"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; }
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };
	
	void setBlockID(ot::UID blockID) { _blockID = blockID; }
	ot::UID getBlockID() const { return _blockID; }
	ot::UID getCoordinateEntityID() const { return _coordinate2DEntityID; }
	
	std::list<ot::Connector> getAllConnectors() const { return _connectors; }
	std::list<ot::BlockConnection> getAllOutgoingConnections() const { return _outgoingConnections; }

	void AddConnector(const ot::Connector& connector);
	void RemoveConnector(const ot::Connector& connector);
	void AddOutgoingConnection(const ot::BlockConnection& connection) { _outgoingConnections.push_back(connection); }

	void setCoordinateEntityID(ot::UID coordinateEntityID) { _coordinate2DEntityID = coordinateEntityID; };
	void SetServiceInformation(const ot::BasicServiceInformation& info) { _info = info; }
	void SetGraphicsScenePackageName(const std::string& name) { _graphicsScenePackage = name; }

protected:
	ot::UID _blockID = 0;
	ot::UID _coordinate2DEntityID = 0;
	ot::BasicServiceInformation _info;
	std::string	_graphicsScenePackage;

	std::list<ot::Connector> _connectors;
	std::list<ot::BlockConnection> _outgoingConnections;

	virtual ot::GraphicsItemCfg* CreateBlockCfg() = 0;

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};
