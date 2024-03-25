//! @file ColorStyle.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTWidgets/ColorStyleValue.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qmetatype.h>

// std header
#include <map>
#include <string>

namespace ot {

	class OT_WIDGETS_API_EXPORT ColorStyle : public Serializable {
	public:
		ColorStyle();
		ColorStyle(const ColorStyle& _other);
		~ColorStyle();

		ColorStyle& operator = (const ColorStyle& _other);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter/Getter

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& name(void) const { return m_name; };

		void setStyleSheet(const QString& _sheet) { m_styleSheet = _sheet; };
		const QString& styleSheet(void) const { return m_styleSheet; };

		void addValue(const ColorStyleValue& _value);
		bool hasValue(const std::string& _name) const;
		const ColorStyleValue& getValue(const std::string& _name, const ColorStyleValue& _default = ColorStyleValue()) const;

	private:
		std::string m_name;
		QString m_styleSheet;
		std::map<std::string, ColorStyleValue> m_values;
	};

}

Q_DECLARE_METATYPE(const ot::ColorStyle&)