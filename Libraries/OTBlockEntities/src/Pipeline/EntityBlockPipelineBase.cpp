// @otlicense

// OpenTwin header
#include "OTBlockEntities/Pipeline/EntityBlockPipelineBase.h"

ot::EntityBlockPipelineBase::EntityBlockPipelineBase(ot::UID _id, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBlock(_id, _parent, _obs, _ms)
{

}

ot::EntityBlockPipelineBase::~EntityBlockPipelineBase()
{
}

void ot::EntityBlockPipelineBase::addConnectors(ot::GraphicsFlowItemBuilder& _flowBlockBuilder) const
{
	for (const auto& connectorByName : this->getAllConnectors())
	{
		const ot::Connector& connector = connectorByName.second;
		ot::ConnectorType connectorType = connector.getConnectorType();
		if (connectorType == ot::ConnectorType::UNKNOWN) {
			OT_LOG_E("Unknown connector type");
		}
		const std::string connectorName = connector.getConnectorName();
		const std::string connectorTitle = connector.getConnectorTitle();
		if (connectorType == ot::ConnectorType::In)
		{
			_flowBlockBuilder.addLeft(connectorName, connectorTitle, ot::GraphicsFlowItemConnector::TriangleRight);
		}
		else if (connectorType == ot::ConnectorType::InOptional)
		{
			_flowBlockBuilder.addLeft(connectorName, connectorTitle, ot::GraphicsFlowItemConnector::Circle);
		}
		else if (connectorType == ot::ConnectorType::Out)
		{
			_flowBlockBuilder.addRight(connectorName, connectorTitle, ot::GraphicsFlowItemConnector::TriangleRight);
		}
	}
}
