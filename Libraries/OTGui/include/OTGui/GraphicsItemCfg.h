//! @file GraphicsItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/Size2D.h"
#include "OpenTwinCore/Point2D.h"
#include "OpenTwinCore/SimpleFactory.h"
#include "OpenTwinCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/Border.h"
#include "OTGui/Margins.h"
#include "OTGui/Font.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORTONLY GraphicsItemCfg : public ot::Serializable, public ot::SimpleFactoryObject {
	public:
		enum GraphicsItemFlag {
			NoFlags           = 0x00, //! @brief No item flags
			ItemIsConnectable = 0x01  //! @brief Item can be used as source or destination of a conncetion
		};

		GraphicsItemCfg();
		virtual ~GraphicsItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

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
		void setMargins(double _top, double _right, double _bottom, double _left) { this->setMargins(ot::MarginsD(_top, _right, _bottom, _left)); };

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

	private:
		std::string m_name;
		std::string m_tile;
		std::string m_uid;
		Point2DD m_pos;

		Size2DD m_minSize;
		Size2DD m_maxSize;

		MarginsD m_margins;
		GraphicsItemFlag m_flags;
		ot::Alignment m_alignment;
		ot::SizePolicy m_sizePolicy;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsItemCfg::GraphicsItemFlag);