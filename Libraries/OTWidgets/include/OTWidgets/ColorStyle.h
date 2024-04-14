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

		void setColorStyleName(const std::string& _name) { m_name = _name; };
		const std::string& colorStyleName(void) const { return m_name; };

		void setStyleSheet(const QString& _sheet) { m_styleSheet = _sheet; };
		const QString& styleSheet(void) const { return m_styleSheet; };

		void addValue(const ColorStyleValue& _value, bool _replace = false);
		bool hasValue(const std::string& _name) const;
		const ColorStyleValue& getValue(const std::string& _name, const ColorStyleValue& _default = ColorStyleValue()) const;
		void setValues(const std::map<std::string, ColorStyleValue>& _values) { m_values = _values; };
		const std::map<std::string, ColorStyleValue>& getValues(void) const { return m_values; };

		void addFile(const std::string& _name, const QString& _path, bool _replace = false);
		bool hasFile(const std::string& _name) const;
		QString getFile(const std::string& _name) const;
		void setFiles(const std::map<std::string, QString>& _files) { m_files = _files; };
		const std::map<std::string, QString>& getFiles(void) const { return m_files; };

		void addInteger(const std::string& _name, int _value, bool _replace = false);
		bool hasInteger(const std::string& _name) const;
		int getInteger(const std::string& _name) const;
		void setIntegers(const std::map<std::string, int>& _integers) { m_int = _integers; };
		const std::map<std::string, int>& getIntegers(void) const { return m_int; };

		void addDouble(const std::string& _name, double _value, bool _replace = false);
		bool hasDouble(const std::string& _name) const;
		double getDouble(const std::string& _name) const;
		void setDoubles(const std::map<std::string, double>& _doubles) { m_double = _doubles; };
		const std::map<std::string, double>& getDoubles(void) const { return m_double; };

		bool setupFromFile(QByteArray _data);

	private:
		std::string m_name;
		QString m_styleSheet;
		std::map<std::string, ColorStyleValue> m_values;
		std::map<std::string, QString> m_files;
		std::map<std::string, int> m_int;
		std::map<std::string, double> m_double;
	};

}

Q_DECLARE_METATYPE(const ot::ColorStyle&)