// @otlicense
// File: EntityBlockConnection.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTBlockEntities/EntityBlock.h"

class OT_BLOCKENTITIES_API_EXPORT EntityBlockConnection : public EntityBase
{
public:
	EntityBlockConnection() : EntityBlockConnection(0, nullptr, nullptr, nullptr) {};
	EntityBlockConnection(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);
	virtual ~EntityBlockConnection();

	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };

	static std::string className() { return "EntityBlockConnection"; }
	virtual std::string getClassName(void) const override { return EntityBlockConnection::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	void setConnectionCfg(const ot::GraphicsConnectionCfg& connectionCfg);
	ot::GraphicsConnectionCfg getConnectionCfg() const;
	
	void setGraphicsPickerKey(const std::string& _key);
	const std::string& getGraphicsPickerKey() const { return m_pickerKey; };

	void setLineShape(ot::GraphicsConnectionCfg::ConnectionShape _shape);
	ot::GraphicsConnectionCfg::ConnectionShape getLineShape() const { return m_lineShape; };

	//! @brief The name of the container below the graphics editor. This name need not be editable. 
	//! If the name is empty, it is assumed that the block exists directly below the graphics scene entity
	void setGraphicsScenePackageChildName(const std::string& _name) { m_graphicsScenePackageChildName = _name; }

	//! @brief Create the connection in the graphics scene.
	//! @note Requires that an observer is set that is able to send messages to the viewer.
	void createConnectionItem() const;

	ot::JsonDocument createGraphicsRequestDocument() const;

	void createProperties();
	virtual bool updateFromProperties() override;
	virtual void addVisualizationNodes(void) override;
	virtual int getSchemaVersion(void) override { return 1; };

private:
	ot::GraphicsConnectionCfg::ConnectionShape m_lineShape = ot::GraphicsConnectionCfg::ConnectionShape::DirectLine;
	ot::PenFCfg m_lineStyle;

	std::string m_pickerKey;
	std::string	m_graphicsScenePackageChildName = "";
	ot::UID m_blockIDOrigin;
	ot::UID m_blockIDDestination;
	std::string m_connectorNameOrigin;
	std::string m_connectorNameDestination;
	ot::Point2DD m_originPos;
	ot::Point2DD m_destPos;

	void createNavigationTreeEntry();
	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};

