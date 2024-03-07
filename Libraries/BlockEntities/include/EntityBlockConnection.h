#pragma once
#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockConnection : public EntityBase
{
public:
	EntityBlockConnection(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner);
	virtual ~EntityBlockConnection();

	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };

	virtual std::string getClassName(void) override { return "EntityBlockConnection"; };
	virtual entityType getEntityType(void) override { return TOPOLOGY; };

	std::string getBlockID() const { return std::to_string(getEntityID()); }
	ot::GraphicsConnectionCfg getConnectionCfg();
	void setConnectionCfg(const ot::GraphicsConnectionCfg& connectionCfg);
	void SetGraphicsScenePackageName(const std::string& name) { _graphicsScenePackage = name; }
	void SetServiceInformation(const ot::BasicServiceInformation& info) { _info = info; }
	void CreateConnections();
	
	virtual int getSchemaVersion(void) override { return 1; };
private:
	std::string _navigationTreeIconName = "";
	std::string _navigationTreeIconNameHidden = "";
	ot::GraphicsConnectionCfg::ConnectionStyle _lineStyle = ot::GraphicsConnectionCfg::ConnectionStyle::DirectLine;
	ot::Color _color;
	int _lineWidth;

	ot::BasicServiceInformation _info;
	std::string	_graphicsScenePackage;
	ot::UID _blockIDOrigin;
	ot::UID _blockIDDestination;
	std::string _connectorNameOrigin;
	std::string _connectorNameDestination;
	
	virtual void addVisualizationNodes(void) override;
	void CreateNavigationTreeEntry();

	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};

