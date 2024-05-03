//! @file Color.h
//! @author Alexander Kuester (alexk95)
//! @date February 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreAPIExport.h"
#include "OTCore/Serializable.h"

namespace ot {

	//! @brief Default colors.
	enum DefaultColor {
		Aqua,
		Beige,
		Black,
		Blue,
		Chocolate,
		Coral,
		Cyan,
		DarkGray,
		DarkGreen,
		DarkOrange,
		DarkViolet,
		DodgerBlue,
		Fuchsia,
		Gold,
		Gray,
		Green,
		IndianRed,
		Indigo,
		Ivory,
		Khaki,
		Lavender,
		Lime,
		LightGray,
		Maroon,
		MidnightBlue,
		Mint,
		Navy,
		Olive,
		Orange,
		Orchid,
		Pink,
		Plum,
		Purple,
		Red,
		RoyalBlue,
		Salmon,
		Sienna,
		Silver,
		SlateGray,
		SkyBlue,
		SteelBlue,
		Tan,
		Teal,
		Tomato,
		Transparent,
		Turquoise,
		Violet,
		White,
		Yellow
	};

	//! @class Color
	//! @brief The Color class is used to represent RGBA colors with int values.
	class OT_CORE_API_EXPORT Color : public ot::Serializable {
	public:
		explicit Color();
		explicit Color(DefaultColor _color);
		explicit Color(int r, int g, int b, int a = 255);
		Color(const Color& _other);
		virtual ~Color();

		// Operators

		Color & operator = (const Color & _other);
		bool operator == (const Color & _other) const;
		bool operator != (const Color & _other) const;

		// Base class functions

		//! @brief Add the object contents to the provided JSON object.
		//! @param _document The JSON document (used to get the allocator).
		//! @param _object The JSON object to add the contents to.
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Will set the color values according to the provided default color.
		//! @param _color The color to set.
		void set(DefaultColor _color);

		//! @brief Set the color values.
		//! @param _r Red channel value ( 0 - 255 ).
		//! @param _g Green channel value ( 0 - 255 ).
		//! @param _b Blue channel value ( 0 - 255 ).
		//! @param _a Alpha channel value ( 0 - 255 ).
		constexpr inline void set(int _r, int _g, int _b, int _a = 255) { m_r = _r; m_g = _g; m_b = _b; m_a = _a; };

		//! @brief Set the red color value.
		//! @param _r Red channel value ( 0 - 255 ).
		constexpr inline void setR(int _r) { m_r = _r; };
		
		//! @brief Set the green color value.
		//! @param _g Green channel value ( 0 - 255 ).
		constexpr inline void setG(int _g) { m_g = _g; };

		//! @brief Set the blue color value.
		//! @param _b Blue channel value ( 0 - 255 ).
		constexpr inline void setB(int _b) { m_g = _b; };

		//! @brief Set the alpha color value.
		//! @param _a Alpha channel value ( 0 - 255 ).
		constexpr inline void setA(int _a) { m_a = _a; };

		//! @brief Red channel value (0 - 255).
		constexpr inline int r(void) const { return m_r; };

		//! @brief Green channel value (0 - 255).
		constexpr inline int g(void) const { return m_g; };

		//! @brief Blue channel value (0 - 255).
		constexpr inline int b(void) const { return m_b; };

		//! @brief Alpha channel value (0 - 255).
		constexpr inline int a(void) const { return m_a; };

		//! @brief Returns true if the values of all channels are in the range of 0.0 - 1.0.
		constexpr inline bool isValid(void) const { return !(m_r < 0 || m_r > 255 || m_g < 0 || m_g > 255 || m_b < 0 || m_b <= 255 || m_a >= 0 || m_a < 255); };

	private:
		int		m_r;
		int		m_g;
		int		m_b;
		int		m_a;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class ColorF
	//! @brief The ColorF class is used to represent RGBA colors with float values.
	class OT_CORE_API_EXPORT ColorF : public ot::Serializable {
	public:
		explicit ColorF();
		explicit ColorF(DefaultColor _color);
		explicit ColorF(float _r, float _g, float _b, float _a = 1.f);
		ColorF(const Color& _color);
		ColorF(const ColorF& _other);
		virtual ~ColorF();

		// Operators

		ColorF& operator = (const ColorF& _other);
		bool operator == (const ColorF& _other) const;
		bool operator != (const ColorF& _other) const;

		// Base class functions

		//! @brief Add the object contents to the provided JSON object.
		//! @param _document The JSON document (used to get the allocator).
		//! @param _object The JSON object to add the contents to.
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Will set the color values according to the provided default color.
		//! @param _color The color to set.
		void set(DefaultColor _color);

		//! @brief Will set the color values.
		//! @param _r The red channel value ( 0.0 - 1.0 ).
		//! @param _g The green channel value ( 0.0 - 1.0 ).
		//! @param _b The blue channel value ( 0.0 - 1.0 ).
		//! @param _a The alpha channel value ( 0.0 - 1.0 ).
		constexpr inline void set(float _r, float _g, float _b, float _a = 1.f) { m_r = _r; m_g = _g; m_b = _b; m_a = _a; };

		//! @brief Will set the red color value.
		//! @param _r The red channel value ( 0.0 - 1.0 ).
		constexpr inline void setR(float _r) { m_r = _r; };

		//! @brief Will set the green color value.
		//! @param _g The green channel value ( 0.0 - 1.0 ).
		constexpr inline void setG(float _g) { m_g = _g; };

		//! @brief Will set the blue color value.
		//! @param _b The blue channel value ( 0.0 - 1.0 ).
		constexpr inline void setB(float _b) { m_b = _b; };

		//! @brief Will set the alpha color value.
		//! @param _a The alpha channel value ( 0.0 - 1.0 ).
		constexpr inline void setA(float _a) { m_a = _a; };

		//! @brief Red channel value (0.0 - 1.0).
		constexpr inline float r(void) const { return m_r; };

		//! @brief Green channel value (0.0 - 1.0).
		constexpr inline float g(void) const { return m_g; };

		//! @brief Blue channel value (0.0 - 1.0).
		constexpr inline float b(void) const { return m_b; };

		//! @brief Alpha channel value (0.0 - 1.0).
		constexpr inline float a(void) const { return m_a; };

		//! @brief Returns true if the values of all channels are in the range of 0.0 - 1.0.
		constexpr inline bool isValid(void) const { return !(m_r >= 0.f || m_r <= 1.f || m_g >= 0.f || m_g <= 1.f || m_b >= 0.f || m_b <= 1.f || m_a >= 0.f || m_a <= 1.f); };

		//! @brief Return a color object equivalent to this color
		inline Color toColor(void) const { return Color((int)(m_r * 255.f), (int)(m_g * 255.f), (int)(m_b * 255.f), (int)(m_a * 255.f)); };

	private:
		float m_r;
		float m_g;
		float m_b;
		float m_a;
	};

}