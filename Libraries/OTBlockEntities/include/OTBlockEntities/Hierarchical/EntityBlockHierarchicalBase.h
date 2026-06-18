// @otlicense
// File: EntityBlockHierarchicalBase.h
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
#include "OTCore/ProjectInformation.h"
#include "OTGui/Graphics/Builder/GraphicsHierarchicalItemBuilder.h"
#include "OTBlockEntities/EntityBlock.h"

class EntityBinaryData;

namespace ot {

	class OT_BLOCKENTITIES_API_EXPORT EntityBlockHierarchicalBase : public EntityBlock
	{
	public:
		EntityBlockHierarchicalBase() : EntityBlockHierarchicalBase(0, nullptr, nullptr, nullptr) {};
		EntityBlockHierarchicalBase(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);

		virtual entityType getEntityType(void) const override { return TOPOLOGY; }

		//! @brief Will be called whenever a property of the entity has changed.
		//! Will reset the update flag for all properties and request the recreation of the block item.
		//! Call this method at the end of the updateFromProperties() method of the derived class to ensure that the block item is updated with the new property values.
		virtual bool updateFromProperties() override;
		virtual void createProperties() override;

		//! @brief Fills the context menu config for this entity.
		//! Adds:
		//!    -> Open
		//!    -> EntityBlock::fillContextMenu()
		//! @param _requestData Data related to the context menu request. Caller keeps ownership of the pointer.
		//! @param _menuCfg Context menu config to fill.
		virtual void fillContextMenu(const MenuRequestData* _requestData, MenuCfg& _menu) override;

		virtual ot::GraphicsItemCfg* createBlockCfg() override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data accessors

		void setCenterImage(const EntityBase& _entity, ot::ImageFileFormat _format) { setCenterImage(_entity.getEntityID(), _entity.getEntityStorageVersion(), _format); };
		void setCenterImage(ot::UID _entityID, ot::UID _entityVersion, ot::ImageFileFormat _format);
		void removeCenterImage();
		bool hasCenterImage() const { return m_centerImageUID != ot::invalidUID; };
		ot::UID getCenterImageID() const { return m_centerImageUID; };
		ot::UID getCenterImageVersion() const { return m_centerImageVersion; };
		ot::ImageFileFormat getCenterImageFormat() const { return m_centerImageFormat; };
		std::shared_ptr<EntityBinaryData> getCenterImageData();

		void setConnectorState(Alignment _connectorAlignment, GraphicsHierarchicalItemBuilder::ExpanderState _state);
		GraphicsHierarchicalItemBuilder::ExpanderState getConnectorState(Alignment _connectorAlignment) const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Property accessors

		constexpr static std::string_view generalPropertyGroupName = "General";
		constexpr static std::string_view titlePropertyGroupName = "Title";
		constexpr static std::string_view centerImagePropertyGroupName = "Image";
		constexpr static std::string_view footerPropertyGroupName = "Footer";

		GraphicsHierarchicalItemBuilder::BackgroundShape getBackgroundShape() const;
		const Painter2D* getBackgroundPainter() const;
		const Painter2D* getBorderPainter() const;
		int getBorderWidth() const;

		bool getUseCustomSize() const;
		int getCustomWidth() const;
		int getCustomHeight() const;
		Size2DD getCustomSize() const;

		bool getShowTitle() const;
		std::string getCustomTitle() const;
		Font getTitleFont() const;
		const Painter2D* getTitlePainter() const;
		Alignment getTitleAlignment() const;

		bool getMaintainCenterImageAspectRatio() const;
		Alignment getCenterImageAlignment() const;

		bool getShowFooter() const;
		std::string getFooterText() const;
		Font getFooterFont() const;
		const Painter2D* getFooterPainter() const;
		Alignment getFooterAlignment() const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected

	protected:
		virtual void createNavigationTreeEntry() override;
		virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
		virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap) override;

		void ensureCenterImageLoaded();

	private:
		ot::UID m_centerImageUID;
		ot::UID m_centerImageVersion;
		ot::ImageFileFormat m_centerImageFormat;
		std::shared_ptr<EntityBinaryData> m_centerImageData;

		ot::GraphicsHierarchicalItemBuilder::ExpanderState m_topConnectorState;
		ot::GraphicsHierarchicalItemBuilder::ExpanderState m_bottomConnectorState;
		ot::GraphicsHierarchicalItemBuilder::ExpanderState m_leftConnectorState;
		ot::GraphicsHierarchicalItemBuilder::ExpanderState m_rightConnectorState;

		void createTextProperties(const std::string& _group, bool _isTopText, const std::string& _showTextPropertyName);
		void createImageProperties(const std::string& _group, bool _isCenter);

		bool updateTextProperties(const std::string& _group, const std::string& _showTextPropertyName);
	};

}
