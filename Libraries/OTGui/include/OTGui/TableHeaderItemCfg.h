// @otlicense

//! @file TableHeaderItemCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class OT_GUI_API_EXPORT TableHeaderItemCfg : public Serializable {
	public:
		TableHeaderItemCfg();
		TableHeaderItemCfg(const std::string& _text);
		TableHeaderItemCfg(const TableHeaderItemCfg& _other);
		virtual ~TableHeaderItemCfg();

		TableHeaderItemCfg& operator = (const TableHeaderItemCfg& _other);

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setText(const std::string& _text) { m_text = _text; };
		const std::string& getText(void) const { return m_text; };

	private:
		std::string m_text;
	};

}