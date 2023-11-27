//! @file GraphicsItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Size2D.h"
#include "OTCore/Point2D.h"
#include "OTCore/Flags.h"
#include "OTCore/Serializable.h"
#include "OTCore/SimpleFactory.h"
#include "OTGui/Font.h"
#include "OTGui/Border.h"
#include "OTGui/Margins.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORTONLY GraphicsItemCfg : public ot::Serializable, public ot::SimpleFactoryObject {
	public:
		enum GraphicsItemFlag {
			NoFlags             = 0x00, //! @brief No item flags
			ItemIsMoveable      = 0x01, //! @brief Item may be used by the user. If the item has a parent, the item may be moved inside of the parent item
			ItemIsConnectable   = 0x02, //! @brief Item can be used as source or destination of a conncetion
			ItemForwardsTooltip = 0x04  //! @brief If the user hovers over this item and no tooltip is set, the tooltip request will be forwarded to the parent item. If this flag is not set this item also wont forward tooltip requests from child items
		};

		GraphicsItemCfg();
		virtual ~GraphicsItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Set item name
		//! The item name must be unique for one item picker.
		void setName(const std::string& _name) { m_name = _name; };

		//! @brief Item name
		//! The item name must be unique for one item picker.
		const std::string& name(void) const { return m_name; };

		//! @brief Set item title
		//! The item title will be displayed to the user when needed.
		void setTitle(const std::string& _title) { m_tile = _title; };

		//! @brief Item title
		//! The item title will be displayed to the user when needed.
		const std::string& title(void) const { return m_tile; };

		//! @brief Set ToolTip
		//! ToolTips are displayed when the user hovers over an item.
		//! If the root item in a graphics item hierarchy has a tool tip set, child items may be enabled to forward the tooltip request (See GraphicsItemFlags)
		void setToolTip(const std::string& _toolTip) { m_tooltip = _toolTip; };

		//! @brief ToolTip that will be displayed to the user when he hovers over the item
		const std::string& toolTip(void) const { return m_tooltip; };

		//! @brief Set item position
		//! If the item is the root item, the position is the scene position.
		//! If the item is a child item, the position is the local position (default: 0.0; 0.0).
		//! @param _x X position
		//! @param _y Y position
		void setPosition(double _x, double _y) { this->setPosition(Point2DD(_x, _y)); };

		//! @brief Set item position
		//! If the item is the root item, the position is the scene position.
		//! If the item is a child item, the position is the local position (default: 0.0; 0.0).
		//! @param _pos Item position
		void setPosition(const Point2DD& _pos) { m_pos = _pos; };

		//! @brief Item position
		//! If the item is the root item, the position is the scene position.
		//! If the item is a child item, the position is the local position (default: 0.0; 0.0).
		const Point2DD& position(void) { return m_pos; };

		void setMinimumSize(const ot::Size2DD& _size) { m_minSize = _size; };
		const ot::Size2DD& minimumSize(void) const { return m_minSize; };

		void setMaximumSize(const ot::Size2DD& _size) { m_maxSize = _size; };
		const ot::Size2DD& maximumSize(void) const { return m_maxSize; };

		//! @brief Set item margins
		//! @param _top Top margin
		//! @param _right Right margin
		//! @param _bottom Bottom margin
		//! @param _left Left margin
		void setMargins(double _left, double _top, double _right, double _bottom) { this->setMargins(ot::MarginsD(_left, _top, _right, _bottom)); };

		//! @brief Set item margins
		//! @param _margins Margins to set
		void setMargins(const MarginsD& _margins) { m_margins = _margins; };

		//! @brief Item margins
		const MarginsD& margins(void) const { return m_margins; };

		void setGraphicsItemFlags(GraphicsItemFlag _flags) { m_flags = _flags; };
		GraphicsItemFlag graphicsItemFlags(void) const { return m_flags; };

		void setAlignment(ot::Alignment _align) { m_alignment = _align; };
		ot::Alignment alignment(void) const { return m_alignment; };

		void setUid(const std::string& _uid) { m_uid = _uid; };
		const std::string& uid(void) const { return m_uid; };

		void setSizePolicy(ot::SizePolicy _policy) { m_sizePolicy = _policy; };
		ot::SizePolicy sizePolicy(void) const { return m_sizePolicy; };

		void setConnectionDirection(ConnectionDirection _direction) { m_connectionDirection = _direction; };
		ConnectionDirection connectionDirection(void) const { return m_connectionDirection; };

	private:
		std::string m_name;
		std::string m_tile;
		std::string m_uid;
		std::string m_tooltip;
		Point2DD m_pos;

		Size2DD m_minSize;
		Size2DD m_maxSize;

		MarginsD m_margins;
		GraphicsItemFlag m_flags;
		ot::Alignment m_alignment;
		ot::SizePolicy m_sizePolicy;
		ConnectionDirection m_connectionDirection;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsItemCfg::GraphicsItemFlag);