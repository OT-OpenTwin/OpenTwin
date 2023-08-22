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
	void setBlockName(const std::string& blockID) { _blockName = blockID; }
	void setBlockPosition(ot::Point2DD& position) { _location = position; }
	std::list<ot::Connector> getAllConnectors() const { return _connectors; }
	std::list<ot::BlockConnection> getAllOutgoingConnections() const {	return _outgoingConnections;	}
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };
	
	void AddConnector(const ot::Connector& connector) { _connectors.push_back(connector); }
	void AddOutgoingConnection(const ot::BlockConnection& connection) { _outgoingConnections.push_back(connection); }

protected:
	std::string _blockName;
	ot::Point2DD _location;
	std::list<ot::Connector> _connectors;
	std::list<ot::BlockConnection> _outgoingConnections;

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};
