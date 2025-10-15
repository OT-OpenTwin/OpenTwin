#pragma once
#include "EntityBlock.h"

class __declspec(dllexport) EntityBlockConnection : public EntityBase
{
public:
	EntityBlockConnection() : EntityBlockConnection(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockConnection(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);
	virtual ~EntityBlockConnection();

	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };

	static std::string className() { return "EntityBlockConnection"; }
	virtual std::string getClassName(void) override { return EntityBlockConnection::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	ot::GraphicsConnectionCfg getConnectionCfg();
	void setConnectionCfg(const ot::GraphicsConnectionCfg& connectionCfg);
	
	//! @brief The name of the container below the graphics editor. This name need not be editable. 
	//! If the name is empty, it is assumed that the block exists directly below the graphics scene entity
	void setGraphicsScenePackageChildName(const std::string& _name) { m_graphicsScenePackageChildName = _name; }
	void setServiceInformation(const ot::BasicServiceInformation& info) { m_info = info; }
	void CreateConnections();

	void createProperties();
	virtual bool updateFromProperties() override;
	virtual void addVisualizationNodes(void) override;
	virtual int getSchemaVersion(void) override { return 1; };
private:
	std::string m_navigationOldTreeIconName = "";
	std::string m_navigationOldTreeIconNameHidden = "";
	ot::GraphicsConnectionCfg::ConnectionShape _lineShape = ot::GraphicsConnectionCfg::ConnectionShape::DirectLine;
	ot::PenFCfg m_lineStyle;

	ot::BasicServiceInformation m_info;
	std::string	m_graphicsScenePackageChildName = "";
	ot::UID _blockIDOrigin;
	ot::UID _blockIDDestination;
	std::string _connectorNameOrigin;
	std::string _connectorNameDestination;
	
	
	void CreateNavigationTreeEntry();
	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};

