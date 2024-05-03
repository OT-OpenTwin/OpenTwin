//! @file ColorStyleValue.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/Serializable.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtGui/qcolor.h>
#include <QtGui/qbrush.h>

// std header
#include <string>

namespace ot {

	class Painter2D;

	class OT_WIDGETS_API_EXPORT ColorStyleValue : public Serializable {
	public:
		ColorStyleValue();
		ColorStyleValue(const ColorStyleValue& _other);
		~ColorStyleValue();

		ColorStyleValue& operator = (const ColorStyleValue& _other);

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

		QString qss(void) const;
		QColor color(void) const;
		QBrush brush(void) const;

		//! @brief Set the painter which will also set the brush
		//! The object takes ownership
		void setPainter(Painter2D* _painter);
		void setPainter(const Painter2D* _painter);
		Painter2D* painter(void) const { return m_painter; };

		void setColor(DefaultColor _color);
		void setColor(Color _color);

	private:
		std::string m_name;
		Painter2D* m_painter;
	};

}
