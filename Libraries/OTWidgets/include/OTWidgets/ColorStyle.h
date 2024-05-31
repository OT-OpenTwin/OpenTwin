//! @file ColorStyle.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/ColorStyleTypes.h"
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
		bool hasValue(ColorStyleValueEntry _type) const;
		const ColorStyleValue& getValue(ColorStyleValueEntry _type, const ColorStyleValue& _default = ColorStyleValue()) const;
		void setValues(const std::map<ColorStyleValueEntry, ColorStyleValue>& _values) { m_values = _values; };
		const std::map<ColorStyleValueEntry, ColorStyleValue>& getValues(void) const { return m_values; };

		void addFile(ColorStyleFileEntry _type, const QString& _path, bool _replace = false);
		bool hasFile(ColorStyleFileEntry _type) const;
		QString getFile(ColorStyleFileEntry _type) const;
		void setFiles(const std::map<ColorStyleFileEntry, QString>& _files) { m_files = _files; };
		const std::map<ColorStyleFileEntry, QString>& getFiles(void) const { return m_files; };

		void addInteger(ColorStyleIntegerEntry _type, int _value, bool _replace = false);
		bool hasInteger(ColorStyleIntegerEntry _type) const;
		int getInteger(ColorStyleIntegerEntry _type) const;
		void setIntegers(const std::map<ColorStyleIntegerEntry, int>& _integers) { m_int = _integers; };
		const std::map<ColorStyleIntegerEntry, int>& getIntegers(void) const { return m_int; };

		void addDouble(ColorStyleDoubleEntry _type, double _value, bool _replace = false);
		bool hasDouble(ColorStyleDoubleEntry _type) const;
		double getDouble(ColorStyleDoubleEntry _type) const;
		void setDoubles(const std::map<ColorStyleDoubleEntry, double>& _doubles) { m_double = _doubles; };
		const std::map<ColorStyleDoubleEntry, double>& getDoubles(void) const { return m_double; };

		bool setupFromFile(QByteArray _data);

	private:
		std::string m_name;
		QString m_styleSheet;
		std::map<ColorStyleValueEntry, ColorStyleValue> m_values;
		std::map<ColorStyleFileEntry, QString> m_files;
		std::map<ColorStyleIntegerEntry, int> m_int;
		std::map<ColorStyleDoubleEntry, double> m_double;
	};

}

Q_DECLARE_METATYPE(const ot::ColorStyle&)