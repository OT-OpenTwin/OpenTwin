//! @file Border.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/Outline.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	//! \class Border
	//! \ingroup OTGui
	//! \brief The Border class is used to describe a border with four sides.
	//! The Border uses integer values.
	class OT_GUI_API_EXPORT Border : public ot::Serializable {
	public:
		Border() {};
		Border(const ot::Color& _color, int _width);
		Border(const ot::Color& _color, int _leftWidth, int _topWidth, int _rightWidth, int _bottomWidth);
		Border(const Outline& _left, const Outline& _top, const Outline& _right, const Outline& _bottom);
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
		inline void setTop(const Outline& _s) { m_top = _s; };
		inline const Outline& top(void) const { return m_top; };

		inline void setLeft(const Outline& _s) { m_left = _s; };
		inline const Outline& left(void) const { return m_left; };

		inline void setRight(const Outline& _s) { m_right = _s; };
		inline const Outline& right(void) const { return m_right; };

		inline void setBottom(const Outline& _s) { m_bottom = _s; };
		inline const Outline& bottom(void) const { return m_bottom; };

	private:
		Outline m_top;
		Outline m_left;
		Outline m_right;
		Outline m_bottom;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! \class BorderF
	//! \ingroup OTGui
	//! \brief The BorderF class is used to describe a border with four sides.
	//! The BorderF uses floating point values.
	class OT_GUI_API_EXPORT BorderF : public ot::Serializable {
	public:
		BorderF() {};
		BorderF(const ot::Color& _color, double _width);
		BorderF(const ot::Color& _color, double _leftWidth, double _topWidth, double _rightWidth, double _bottomWidth);
		BorderF(const OutlineF& _left, const OutlineF& _top, const OutlineF& _right, const OutlineF& _bottom);
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
		inline void setTop(const OutlineF& _s) { m_top = _s; };
		inline const OutlineF& top(void) const { return m_top; };

		inline void setLeft(const OutlineF& _s) { m_left = _s; };
		inline const OutlineF& left(void) const { return m_left; };

		inline void setRight(const OutlineF& _s) { m_right = _s; };
		inline const OutlineF& right(void) const { return m_right; };

		inline void setBottom(const OutlineF& _s) { m_bottom = _s; };
		inline const OutlineF& bottom(void) const { return m_bottom; };

	private:
		OutlineF m_top;
		OutlineF m_left;
		OutlineF m_right;
		OutlineF m_bottom;
	};

}