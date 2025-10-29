// @otlicense

//! @file StyleValue.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTCore/Color.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT StyleValue : public Serializable {
	public:
		StyleValue();
		StyleValue(const StyleValue& _other);
		virtual ~StyleValue();

		StyleValue& operator = (const StyleValue& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		void setKey(const std::string& _key) { m_key = _key; };
		const std::string& key(void) const { return m_key; };

		void setQss(const std::string& _qss) { m_qss = _qss; };
		const std::string& qss(void) const { return m_qss; };

		void setColor(const ot::Color& _color) { m_color = _color; };
		const ot::Color& color(void) const { return m_color; };

	private:
		std::string m_key;
		std::string m_qss;
		ot::Color m_color;
	};

}
