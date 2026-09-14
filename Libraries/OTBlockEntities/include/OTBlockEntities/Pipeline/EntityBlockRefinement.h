#pragma once

#include "OTBlockEntities/Pipeline/EntityBlockPipelineBase.h"
class OT_BLOCKENTITIES_API_EXPORT EntityBlockRefinement : public ot::EntityBlockPipelineBase
{
public:
	EntityBlockRefinement() : EntityBlockRefinement(0, nullptr, nullptr, nullptr) {};
	EntityBlockRefinement(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);

	static std::string className() { return "EntityBlockRefinement"; }
	virtual std::string getClassName(void) const override { return EntityBlockRefinement::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }
	
	virtual ot::GraphicsConnectionCfg::ConnectionShape getDefaultConnectionShape() const override { return ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine; };
	
	static std::string getIconName() { return "Database_access.svg"; }
	ot::GraphicsItemCfg* createBlockCfg() override;

	void createProperties();

	std::string getSelectedZone();
	std::string getOutputConnectorName() { return m_outputConnector.getConnectorName(); }

private:	
	ot::Connector m_outputConnector;
};
