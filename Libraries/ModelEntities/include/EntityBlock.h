#pragma once
#include "EntityBase.h"
#include "OpenTwinCore/CoreTypes.h"
#include "OpenTwinCore/Point2D.h"
#include "Connector.h"
#include "BlockConnection.h"

class __declspec(dllexport) EntityBlock : public EntityBase
{
public:
	EntityBlock(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactory* factory, const std::string& owner);

	virtual std::string getClassName(void) override { return "EntityBlock"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; }
	void SetBlockName(const std::string& blockID) { _blockName = blockID; }
	void SetBlockPosition(ot::Point2DD& position) { _location = position; }
	void AddConnector(const ot::Connector& connector) { _connectors.push_back(connector); }
	void AddOutgoingConnection(const ot::BlockConnection& connection) { _outgoingConnections.push_back(connection); }
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };

protected:
	std::string _blockName;
	ot::Point2DD _location;
	std::list<ot::Connector> _connectors;
	std::list<ot::BlockConnection> _outgoingConnections;

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};
