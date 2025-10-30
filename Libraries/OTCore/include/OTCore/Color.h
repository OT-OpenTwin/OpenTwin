// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"

namespace ot {

	class Color;
	class ColorF;

	//! @brief Default colors.
	enum DefaultColor {
		Aqua, //! @brief rgba: 0, 255, 255, 255
		Beige, //! @brief rgba: 245, 245, 220, 255
		Black, //! @brief rgba: 0, 0, 0, 255
		Blue, //! @brief rgba: 0, 0, 255, 255
		Chocolate, //! @brief rgba: 210, 105, 30, 255
		Coral, //! @brief rgba: 255, 127, 80, 255
		Cyan, //! @brief rgba: 0, 255, 255, 255
		DarkGray, //! @brief rgba: 169, 169, 169, 255
		DarkGreen, //! @brief rgba: 0, 100, 0, 255
		DarkOrange, //! @brief rgba: 255, 140, 0, 255
		DarkViolet, //! @brief rgba: 148, 0, 211, 255
		DodgerBlue, //! @brief rgba: 30, 144, 255, 255
		Fuchsia, //! @brief rgba: 255, 0, 255, 255
		Gold, //! @brief rgba: 255, 215, 0, 255
		Gray, //! @brief rgba: 128, 128, 128, 255
		Green, //! @brief rgba: 0, 128, 0, 255
		IndianRed, //! @brief rgba: 205, 92, 92, 255
		Indigo, //! @brief rgba: 75, 0, 130, 255
		Ivory, //! @brief rgba: 255, 255, 240, 255
		Khaki, //! @brief rgba: 240, 230, 140, 255
		Lavender, //! @brief rgba: 230, 230, 250, 255
		Lime, //! @brief rgba: 0, 255, 0, 255
		LightGray, //! @brief rgba: 211, 211, 211, 255
		Maroon, //! @brief rgba: 128, 0, 0, 255
		MidnightBlue, //! @brief rgba: 25, 25, 112, 255
		Mint, //! @brief rgba: 189, 252, 201, 255
		Navy, //! @brief rgba: 0, 0, 128, 255
		Olive, //! @brief rgba: 128, 128, 0, 255
		Orange, //! @brief rgba: 255, 165, 0, 255
		Orchid, //! @brief rgba: 218, 112, 214, 255
		Pink, //! @brief rgba: 255, 192, 203, 255
		Plum, //! @brief rgba: 221, 160, 221, 255
		Purple, //! @brief rgba: 128, 0, 128, 255
		Red, //! @brief rgba: 255, 0, 0, 255
		RoyalBlue, //! @brief rgba: 65, 105, 225, 255
		Salmon, //! @brief rgba: 250, 128, 114, 255
		Sienna, //! @brief rgba: 160, 82, 45, 255
		Silver, //! @brief rgba: 192, 192, 192, 255
		SlateGray, //! @brief rgba: 112, 128, 144, 255
		SkyBlue, //! @brief rgba: 135, 206, 235, 255
		SteelBlue, //! @brief rgba: 70, 130, 180, 255
		Tan, //! @brief rgba: 210, 180, 140, 255
		Teal, //! @brief rgba: 0, 128, 128, 255
		Tomato, //! @brief rgba: 255, 99, 71, 255
		Transparent, //! @brief rgba: 0, 0, 0, 0
		Turquoise, //! @brief rgba: 64, 224, 208, 255
		Violet, //! @brief rgba: 238, 130, 238, 255
		White, //! @brief rgba: 255, 255, 255, 255
		Yellow //! @brief rgba: 255, 255, 0, 255
	};

	//! @class Color
	//! @brief The Color class is used to represent RGBA colors with integer values.
	class OT_CORE_API_EXPORT Color : public ot::Serializable {
	public:
		Color();
		Color(DefaultColor _color);
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

		ColorF toColorF(void) const;

	private:
		int		m_r; //! @brief Red channel.
		int		m_g; //! @brief Green channel.
		int		m_b; //! @brief Blue channel.
		int		m_a; //! @brief Alpha channel.
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class ColorF
	//! @brief The ColorF class is used to represent RGBA colors with float values.
	class OT_CORE_API_EXPORT ColorF : public ot::Serializable {
	public:
		ColorF();
		ColorF(DefaultColor _color);
		explicit ColorF(float _r, float _g, float _b, float _a = 1.f);
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
		Color toColor(void) const;

	private:
		float m_r; //! @brief Red channel.
		float m_g; //! @brief Green channel.
		float m_b; //! @brief Blue channel.
		float m_a; //! @brief Alpha channel.
	};

}