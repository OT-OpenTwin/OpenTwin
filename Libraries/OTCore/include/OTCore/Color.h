/*
 *  Color.h
 *
 *  Created on: 24/02/2023
 *	Author: Alexander Kuester
 *  Copyright (c) 2023, OpenTwin
 */

#pragma once

// OpenTwin header
#include "OTCore/CoreAPIExport.h"
#include "OTCore/Serializable.h"

namespace ot {

	class OT_CORE_API_EXPORT Color : public ot::Serializable {
	public:
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
			Turquoise,
			Violet,
			White,
			Yellow
		};

		explicit Color();
		explicit Color(DefaultColor _color);
		explicit Color(float _r, float _g, float _b, float _a = 1.f);
		explicit Color(int r, int g, int b, int a = 255);
		Color(const Color& _other);
		virtual ~Color();

		// Operators

		Color & operator = (const Color & _other);
		bool operator == (const Color & _other) const;
		bool operator != (const Color & _other) const;

		// Base class functions

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Will set the color values according to the provided default color
		//! @param _color The color to set
		void set(DefaultColor _color);

		//! @brief Will set the color values
		//! @param _r The red channel value ( 0.0 - 1.0 )
		//! @param _g The green channel value ( 0.0 - 1.0 )
		//! @param _b The blue channel value ( 0.0 - 1.0 )
		//! @param _a The alpha channel value ( 0.0 - 1.0 )
		void set(float _r, float _g, float _b, float _a = 1.f) { m_r = _r; m_g = _g; m_b = _b; m_a = _a; }

		//! @brief Will set the red color value
		//! @param _r The red channel value ( 0.0 - 1.0 )
		void setR(float _r) { m_r = _r; }

		//! @brief Will set the green color value
		//! @param _g The green channel value ( 0.0 - 1.0 )
		void setG(float _g) { m_g = _g; }

		//! @brief Will set the blue color value
		//! @param _b The blue channel value ( 0.0 - 1.0 )
		void setB(float _b) { m_b = _b; }

		//! @brief Will set the alpha color value
		//! @param _a The alpha channel value ( 0.0 - 1.0 )
		void setA(float _a) { m_a = _a; }

		//! @brief Will set the color values
		//! @param _r The red channel value ( 0 - 255 )
		//! @param _g The green channel value ( 0 - 255 )
		//! @param _b The blue channel value ( 0 - 255 )
		//! @param _a The alpha channel value ( 0 - 255 )
		void set(int _r, int _g, int _b, int _a = 255);

		//! @brief Will set the red color value
		//! @param _r The red channel value ( 0 - 255 )
		void setR(int _r);

		//! @brief Will set the green color value
		//! @param _g The green channel value ( 0 - 255 )
		void setG(int _g);

		//! @brief Will set the blue color value
		//! @param _b The blue channel value ( 0 - 255 )
		void setB(int _b);

		//! @brief Will set the alpha color value
		//! @param _a The alpha channel value ( 0 - 255 )
		void setA(int _a);

		//! @brief Returns the red channel value (0.0 - 1.0)
		float r(void) const { return m_r; }

		//! @brief Returns the green channel value (0.0 - 1.0)
		float g(void) const { return m_g; }

		//! @brief Returns the blue channel value (0.0 - 1.0)
		float b(void) const { return m_b; }

		//! @brief Returns the alpha channel value (0.0 - 1.0)
		float a(void) const { return m_a; }

		//! @brief Returns the red channel value (0 - 255)
		inline int rInt(void) const;

		//! @brief Returns the green channel value (0 - 255)
		inline int gInt(void) const;

		//! @brief Returns the blue channel value (0 - 255)
		inline int bInt(void) const;

		//! @brief Returns the alpha channel value (0 - 255)
		inline int aInt(void) const;

		//! @brief Returns true if the values of all channels are in the range of 0.0 - 1.0
		bool isValid(void) const { return m_r >= 0.f && m_r <= 1.f && m_g >= 0.f && m_g <= 1.f && m_b >= 0.f && m_b <= 1.f && m_a >= 0.f && m_a <= 1.f; };

	private:
		float		m_r;
		float		m_g;
		float		m_b;
		float		m_a;
	};

}