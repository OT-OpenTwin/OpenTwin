//! @file Border.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/PenCfg.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	//! @class Border
	//! @ingroup OTGui
	//! @brief The Border class is used to describe a border with four sides.
	//! The Border uses integer values.
	class OT_GUI_API_EXPORT Border : public ot::Serializable {
	public:
		Border() {};
		Border(const PenCfg& _pen);
		Border(const ot::Color& _color, int _width);
		Border(const ot::Color& _color, int _leftWidth, int _topWidth, int _rightWidth, int _bottomWidth);
		Border(const PenCfg& _left, const PenCfg& _top, const PenCfg& _right, const PenCfg& _bottom);
		Border(const Border& _other);
		virtual ~Border();

		Border& operator = (const Border& _other);
		
		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Will set the color for all four borders.
		//! @param _color Color to set.
		void setColor(const ot::Color& _color);

		//! @brief Will set the width for all four borders.
		//! @param _width Width to set.
		void setWidth(int _width);

		//! @brief Set the top border.
		inline void setTop(const PenCfg& _s) { m_top = _s; };
		inline const PenCfg& top(void) const { return m_top; };

		inline void setLeft(const PenCfg& _s) { m_left = _s; };
		inline const PenCfg& left(void) const { return m_left; };

		inline void setRight(const PenCfg& _s) { m_right = _s; };
		inline const PenCfg& right(void) const { return m_right; };

		inline void setBottom(const PenCfg& _s) { m_bottom = _s; };
		inline const PenCfg& bottom(void) const { return m_bottom; };

	private:
		PenCfg m_top;
		PenCfg m_left;
		PenCfg m_right;
		PenCfg m_bottom;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class BorderF
	//! @ingroup OTGui
	//! @brief The BorderF class is used to describe a border with four sides.
	//! The BorderF uses floating point values.
	class OT_GUI_API_EXPORT BorderF : public ot::Serializable {
	public:
		BorderF() {};
		BorderF(const ot::Color& _color, double _width);
		BorderF(const ot::Color& _color, double _leftWidth, double _topWidth, double _rightWidth, double _bottomWidth);
		BorderF(const PenFCfg& _left, const PenFCfg& _top, const PenFCfg& _right, const PenFCfg& _bottom);
		BorderF(const BorderF& _other);
		virtual ~BorderF();

		BorderF& operator = (const BorderF& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Will set the color for all four borders.
		//! @param _color Color to set.
		void setColor(const ot::Color& _color);

		//! @brief Will set the width for all four borders.
		//! @param _width Width to set.
		void setWidth(double _width);

		//! @brief Set the top border.
		inline void setTop(const PenFCfg& _s) { m_top = _s; };
		inline const PenFCfg& top(void) const { return m_top; };

		inline void setLeft(const PenFCfg& _s) { m_left = _s; };
		inline const PenFCfg& left(void) const { return m_left; };

		inline void setRight(const PenFCfg& _s) { m_right = _s; };
		inline const PenFCfg& right(void) const { return m_right; };

		inline void setBottom(const PenFCfg& _s) { m_bottom = _s; };
		inline const PenFCfg& bottom(void) const { return m_bottom; };

	private:
		PenFCfg m_top;
		PenFCfg m_left;
		PenFCfg m_right;
		PenFCfg m_bottom;
	};

}