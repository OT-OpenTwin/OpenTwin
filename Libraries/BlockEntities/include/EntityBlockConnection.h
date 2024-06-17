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

	ot::GraphicsConnectionCfg getConnectionCfg();
	void setConnectionCfg(const ot::GraphicsConnectionCfg& connectionCfg);
	void SetGraphicsScenePackageName(const std::string& name) { _graphicsScenePackage = name; }
	void SetServiceInformation(const ot::BasicServiceInformation& info) { _info = info; }
	
	void createProperties();
	virtual bool updateFromProperties() override;
	virtual void addVisualizationNodes(void) override;
	virtual int getSchemaVersion(void) override { return 1; };
private:
	std::string _navigationTreeIconName = "";
	std::string _navigationTreeIconNameHidden = "";
	ot::GraphicsConnectionCfg::ConnectionShape _lineShape = ot::GraphicsConnectionCfg::ConnectionShape::DirectLine;
	ot::OutlineF m_lineStyle;

	ot::BasicServiceInformation _info;
	std::string	_graphicsScenePackage;
	ot::UID _blockIDOrigin;
	ot::UID _blockIDDestination;
	std::string _connectorNameOrigin;
	std::string _connectorNameDestination;
	
	
	void CreateNavigationTreeEntry();
	void CreateConnections();
	void AddStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};

