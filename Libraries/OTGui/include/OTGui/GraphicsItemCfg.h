// @otlicense
// File: GraphicsItemCfg.h
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

// OpenTwin Core header
#include "OTSystem/Flags.h"
#include "OTCore/Size2D.h"
#include "OTCore/Point2D.h"
#include "OTCore/Serializable.h"

// OpenTwin Gui header
#include "OTGui/Font.h"
#include "OTGui/Margins.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/Transform.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/GraphicsZValues.h"

// std header
#include <map>
#include <string>

#pragma warning(disable:4251)

namespace ot {

	//! @class GraphicsItemCfg
	//! @brief The GraphicsItemCfg is the base class for all graphics item configurations.
	class OT_GUI_API_EXPORT GraphicsItemCfg : public ot::Serializable {
		OT_DECL_NOMOVE(GraphicsItemCfg)
		OT_DECL_DEFCOPY(GraphicsItemCfg)
	public:
		static std::string factoryTypeKey() { return "GIType"; };

		static constexpr double defaultAdditionalTriggerDistance() { return 15.; };

		//! @brief GraphicsItemFlag
		enum GraphicsItemFlag {
			NoFlags                    = 0 << 0, //! @brief No item flags.
			ItemIsMoveable             = 1 << 0, //! @brief Item may be used by the user. If the item has a parent, the item may be moved inside of the parent item.
			ItemIsSelectable           = 1 << 1, //! @brief The item can be selected by the user.
			ItemIsConnectable          = 1 << 2, //! @brief Item can be used as source or destination of a conncetion.

			ItemSnapsToGridTopLeft     = 1 << 3, //! @brief Item snaps to grid on the top left corner of the bounding rect.
			ItemSnapsToGridCenter      = 1 << 4, //! @brief Item snaps to grid at the center of the bounding rect (will be ignored if ItemSnapsToGridTopLeft is set).

			//! @brief If the user hovers over this item and no tooltip is set, the tooltip request will be forwarded to the parent item.
			//! @note If this flag is not set this item also wont forward tooltip requests from child items.
			ItemForwardsTooltip        = 1 << 5,

			//! @brief User transform and transform shortcuts are enabled for this item.
			//! The user can rotate and flip the item by using the shortcuts or actions.
			ItemUserTransformEnabled   = 1 << 6,

			//! @brief Item transformations to the parent item will not affect this item.
			//! If the item is located in a group for example, any transformations to the group won't affect this item.
			//! The item will apply the inverse transformation to keep its transformation.
			ItemIgnoresParentTransform = 1 << 7,

			//! @brief Item receives state changes.
			//! The item will paint its border and/or background differently if the item is selected or is hovered by the user.
			//! A StyleRefPainter2D will be used for painting the state.
			//! @ref enum class ot::ColorStyleValueEntry
			ItemHandlesState           = 1 << 8,

			//! @brief Item forwards state changes to child items.
			//! If the root item is a container item (e.g. GraphicsGroupItem) it have this flag set in order to forward the state change to its child items.
			ItemForwardsState          = 1 << 9,

			ItemSilencesNotifcations   = 1 << 10 //! @brief Item will not send notifications to the scene when it is changed.
		};
		typedef Flags<GraphicsItemFlag> GraphicsItemFlags; //! @brief GraphicsItemFlags

		GraphicsItemCfg();
		virtual ~GraphicsItemCfg();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Pure virtual methods

		//! @brief Returns the unique GraphicsItemCfg type name that is used in the GraphicsItemCfgFactory.
		virtual std::string getFactoryKey() const = 0;

		//! @brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy() const = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class methods

		//! @brief Add the object contents to the provided JSON object.
		//! @param _document The JSON document (used to get the allocator).
		//! @param _object The JSON object to add the contents to.
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Set item name.
		void setName(const std::string& _name) { m_name = _name; };

		//! @brief Item name.
		const std::string& getName() const { return m_name; };

		//! @brief Set item title.
		//! The item title will be displayed to the user when needed.
		void setTitle(const std::string& _title) { m_title = _title; };

		//! @brief Item title.
		//! The item title will be displayed to the user when needed.
		const std::string& getTitle() const { return m_title; };

		//! @brief Set ToolTip.
		//! @ref getToolTip
		void setToolTip(const std::string& _toolTip) { m_tooltip = _toolTip; };

		//! @brief ToolTip that will be displayed to the user when he hovers over the item.
		//! If the root item in a graphics item hierarchy has a tool tip set, child items may be enabled to forward the tooltip request (see GraphicsItemFlags).
		const std::string& getToolTip() const { return m_tooltip; };

		//! @brief Set item position.
		//! If the item is the root item, the position is the scene position.
		//! If the item is a child item, the position is the local position (default: 0.0; 0.0).
		//! @param _x X position
		//! @param _y Y position
		void setPosition(double _x, double _y) { this->setPosition(Point2DD(_x, _y)); };

		//! @brief Set item position.
		//! If the item is the root item, the position is the scene position.
		//! If the item is a child item, the position is the local position (default: 0.0; 0.0).
		//! @param _pos Item position.
		void setPosition(const Point2DD& _pos) { m_pos = _pos; };

		//! @brief Item position.
		//! If the item is the root item, the position is the scene position.
		//! If the item is a child item, the position is the local position (default: 0.0; 0.0).
		const Point2DD& getPosition() const { return m_pos; };

		//! @param _d Distance in all directions.
		//! @ref setAdditionalTriggerDistance(const MarginsD& _d)
		void setAdditionalTriggerDistance(double _d) { this->setAdditionalTriggerDistance(MarginsD(_d, _d, _d, _d)); };

		//! @ref setAdditionalTriggerDistance(const MarginsD& _d)
		void setAdditionalTriggerDistance(double _left, double _top, double _right, double _bottom) { this->setAdditionalTriggerDistance(MarginsD(_left, _top, _right, _bottom)); };

		//! @brief Sets the additional trigger distance.
		//! The additional trigger distance will expand/shrink the imaginary bounding rect of the item when checking for mouse press, mouse hover, and other events.
		void setAdditionalTriggerDistance(const MarginsD& _d) { m_additionalTriggerDistance = _d; };

		//! @ref setAdditionalTriggerDistance(const MarginsD& _d)
		const MarginsD& getAdditionalTriggerDistance() const { return m_additionalTriggerDistance; };

		//! @brief Sets the item minimum size.
		//! @ref getMinimumSize
		void setMinimumSize(double _width, double _height) { this->setMinimumSize(Size2DD(_width, _height)); };

		//! @brief Sets the item minimum size.
		//! @ref getMinimumSize
		void setMinimumSize(const Size2DD& _size) { m_minSize = _size; };

		//! @brief Item minimum size.
		//! If the graphics item is resized (e.g. via layout) then it may not shrink below the minimum size.
		const Size2DD& getMinimumSize() const { return m_minSize; };

		//! @brief Sets the item maximum size.
		//! @ref getMaximumSize
		void setMaximumSize(double _width, double _height) { this->setMaximumSize(Size2DD(_width, _height)); };

		//! @brief Sets the item maximum size.
		//! @ref getMaximumSize
		void setMaximumSize(const Size2DD& _size) { m_maxSize = _size; };

		//! @brief Item maximum size.
		//! If the graphics item is resized (e.g. via layout) then it may not brow above the maximum size.
		const Size2DD& getMaximumSize() const { return m_maxSize; };

		//! @brief Sets the items minimum and maximum size.
		//! @ref getMaximumSize
		//! @ref getMinimumSize
		void setFixedSize(double _width, double _height) { this->setFixedSize(Size2DD(_width, _height)); };

		//! @brief Sets the items minimum and maximum size.
		//! @ref getMaximumSize
		//! @ref getMinimumSize
		void setFixedSize(const Size2DD& _size);

		//! @brief Set item margins.
		//! @param _margin Margin in all directions.
		void setMargins(double _margin) { this->setMargins(ot::MarginsD(_margin, _margin, _margin, _margin)); };
		
		//! @brief Set item margins.
		//! @param _top Top margin.
		//! @param _right Right margin.
		//! @param _bottom Bottom margin.
		//! @param _left Left margin.
		void setMargins(double _left, double _top, double _right, double _bottom) { this->setMargins(ot::MarginsD(_left, _top, _right, _bottom)); };

		//! @brief Set item margins.
		//! @param _margins Margins to set.
		void setMargins(const MarginsD& _margins) { m_margins = _margins; };

		//! @brief Item margins.
		const MarginsD& getMargins() const { return m_margins; };

		void setGraphicsItemFlag(GraphicsItemFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setGraphicsItemFlags(const GraphicsItemFlags& _flags) { m_flags = _flags; };
		const GraphicsItemFlags& getGraphicsItemFlags() const { return m_flags; };

		//! @brief Set the item alignment.
		//! @ref getAlignment.
		void setAlignment(Alignment _align) { m_alignment = _align; };

		//! @brief Current item alignment.
		//! The alignment only has an effect if the item is nested.
		Alignment getAlignment() const { return m_alignment; };

		//! @brief Set item UID.
		//! @ref getUid
		void setUid(const UID& _uid) { m_uid = _uid; };

		//! @brief Item UID.
		//! The item UID is used to uniquely identify an item in a GraphicsScene.
		//! The must never exist two items with the same UID.
		const ot::UID& getUid() const { return m_uid; };

		//! @brief Set item Z value.
		//! @ref getZValue.
		void setZValue(int32_t _zValue) { m_zValue = _zValue; };

		//! @brief Current item Z value.
		//! The Z value is used to define the drawing order of items in a scene.
		//! The higher the Z value, the later the item will be drawn (on top of items with lower Z values).
		//! If two items have the same Z value, the item insertion order will define the drawing order (the later an item was inserted, the later it will be drawn).
		int32_t getZValue() const { return m_zValue; };

		//! @brief Set item size policy.
		//! @ref getSizePolicy.
		void setSizePolicy(SizePolicy _policy) { m_sizePolicy = _policy; };

		//! @brief Current item size policy.
		//! The item size policy is used for nested items.
		//! @ref ot::SizePolicy
		SizePolicy getSizePolicy() const { return m_sizePolicy; };

		//! @brief Set the item connection direction.
		//! @ref getConnectionDirection()
		void setConnectionDirection(ConnectionDirection _direction) { m_connectionDirection = _direction; };

		//! @brief Current item connection direction.
		//! This has only an effect if GraphicsItemFlag::ItemIsConnectable is set.
		//! @ref ot::ConnectionDirection
		ConnectionDirection getConnectionDirection() const { return m_connectionDirection; };

		//! @brief Adds the provided entry to the string map.
		//! If an entry for the same key already exists it will be replaced.
		//! @ref getStringMap
		void addStringMapEntry(const std::string& _key, const std::string& _value) { m_stringMap.insert_or_assign(_key, _value); };

		//! @brief Sets the string map.
		//! @ref getStringMap
		void setStringMap(const std::map<std::string, std::string>& _map) { m_stringMap = _map; };

		//! @brief The string map may be used to reference a value via a key in a complex graphics item.
		//! For example a text item with enabled reference mode will use the string map to set its text when created in the frontend.
		//! @warning The string map must be set for the root item only. Child items will be ignored.
		const std::map<std::string, std::string>& getStringMap() const { return m_stringMap; };

		//! @brief Returns the string set for the given key in the string map.
		//! Returns the string "#<_key>" if the value was not found.
		//! @ref getStringMap
		std::string getStringForKey(const std::string& _key) const;

		//! @brief Set the item transform.
		void setTransform(const Transform& _transform) { m_transform = _transform; };

		//! @brief Get the item transform.
		const Transform& getTransform() const { return m_transform; };

	private:
		std::string m_name; //! @brief Item name.
		std::string m_title; //! @brief Item title.
		ot::UID m_uid; //! @brief Item UID.
		std::string m_tooltip; //! @brief Item tool tip.
		Point2DD m_pos; //! @brief Item position.
		MarginsD m_additionalTriggerDistance; //! @ref setAdditionalTriggerDistance

		int32_t m_zValue;

		Size2DD m_minSize; //! @brief Minimum item size.
		Size2DD m_maxSize; //! @brief Maximum item size.

		MarginsD m_margins; //! @brief Item margins.
		GraphicsItemFlags m_flags; //! @brief Item flags.
		ot::Alignment m_alignment; //! @brief Item alignment.
		ot::SizePolicy m_sizePolicy; //! @brief Item size policy.
		ConnectionDirection m_connectionDirection; //! @brief Item connection direction.

		Transform m_transform; //! @brief Item transformation.

		std::map<std::string, std::string> m_stringMap; //! @brief Item's string map.
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsItemCfg::GraphicsItemFlag)