//! @file Outline.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/Painter2D.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class OutlineF;

	//! @brief Line style.
	enum LineStyle {
		NoLine,
		SolidLine,
		DashLine,
		DotLine,
		DashDotLine,
		DashDotDotLine
	};

	//! @brief Line cap style.
	enum LineCapStyle {
		FlatCap,
		SquareCap,
		RoundCap
	};

	//! @brief Line join style.
	enum LineJoinStyle {
		MiterJoin,
		BevelJoin,
		RoundJoin,
		SvgMiterJoin
	};
	
	OT_GUI_API_EXPORT std::string toString(LineStyle _style);
	OT_GUI_API_EXPORT LineStyle stringToLineStyle(const std::string& _style);

	OT_GUI_API_EXPORT std::string toString(LineCapStyle _cap);
	OT_GUI_API_EXPORT LineCapStyle stringToCapStyle(const std::string& _cap);

	OT_GUI_API_EXPORT std::string toString(LineJoinStyle _join);
	OT_GUI_API_EXPORT LineJoinStyle stringToJoinStyle(const std::string& _join);

	//! @class Outline
	//! @brief The Outline class is used to describe how a outline should look like.
	class OT_GUI_API_EXPORT Outline : public ot::Serializable {
	public:
		//! @brief Default constructor.
		//! Creates a black fill painter.
		Outline();

		//! @brief Assignment constructor.
		//! @param _color Initial color.
		Outline(const Color& _color);

		//! @brief Assignment constructor.
		//! @param _painter Initial painter (if 0, default painter will be set [/ref Outline() "See default constructor"]).
		Outline(Painter2D* _painter);

		//! @brief Assignment constructor.
		//! @param _width Line width.
		//! @param _color Initial color.
		Outline(int _width, const Color& _color);

		//! @brief Assignment constructor.
		//! @param _width Line width 
		//! @param _painter Initial painter (if 0, default painter will be set [/ref Outline() "See default constructor"]).
		Outline(int _width, Painter2D* _painter);

		//! @brief Copy constructor.
		//! @param _other The other style.
		Outline(const Outline& _other);

		//! @brief Destructor.
		~Outline();

		//! @brief Assignment operator.
		//! @param _other The other style.
		Outline& operator = (const Outline& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Set the color.
		//! The current painter will be replaced by a FillPainter2D.
		void setColor(const Color& _color);

		//! @brief Set the painter.
		//! The outline takes ownership of the painter.
		//! @param _painter The painter to set.
		void setPainter(Painter2D* _painter);

		//! @brief Painter.
		constexpr inline const Painter2D* painter(void) const { return m_painter; };

		//! @brief Replaces the current painter with a default painter and returns the old painter.
		//! The caller takes ownership of the painter.
		Painter2D* takePainter(void);

		//! @brief Set the line width.
		//! @param _w Width to set.
		constexpr inline void setWidth(int _w) { m_width = _w; };

		//! @brief Line width.
		constexpr inline int width(void) const { return m_width; };

		//! @brief Set the line style.
		//! @param _style Style to set.
		constexpr inline void setStyle(LineStyle _style) { m_style = _style; };

		//! @brief Line style.
		constexpr inline LineStyle style(void) const { return m_style; };

		//! @brief Set the line cap style.
		//! @param _cap Cap style to set.
		constexpr inline void setCap(LineCapStyle _cap) { m_cap = _cap; };

		//! @brief Cap style.
		constexpr inline LineCapStyle cap(void) const { return m_cap; };

		//! @brief Set the line join style.
		//! @param _style Join style to set.
		constexpr inline void setJoinStyle(LineJoinStyle _join) { m_join = _join; };

		//! @brief Line join style.
		constexpr inline LineJoinStyle joinStyle(void) const { return m_join; };

		OutlineF toOutlineF(void) const;

	private:
		Painter2D* m_painter; //! @brief Painter.
		int m_width; //! @brief Line width.
		LineStyle m_style; //! @brief Line style.
		LineCapStyle m_cap; //! @brief Line cap style.
		LineJoinStyle m_join; //! @brief Line join style.
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class OutlineF
	//! @brief The OutlineF class is used to describe how a outline should look like.
	class OT_GUI_API_EXPORT OutlineF : public ot::Serializable {
	public:
		//! @brief Default constructor.
		//! Creates a black fill painter.
		OutlineF();

		//! @brief Assignment constructor.
		//! @param _color Initial color.
		OutlineF(const Color& _color);

		//! @brief Assignment constructor.
		//! @param _painter Initial painter (if 0, default painter will be set [/ref OutlineF() "See default constructor"]).
		OutlineF(Painter2D* _painter);

		//! @brief Assignment constructor.
		//! @param _width Line width.
		//! @param _color Initial color.
		OutlineF(double _width, const Color& _color);

		//! @brief Assignment constructor.
		//! @param _width Line width 
		//! @param _painter Initial painter (if 0, default painter will be set [/ref OutlineF() "See default constructor"]).
		OutlineF(double _width, Painter2D* _painter);

		//! @brief Copy constructor.
		//! @param _other The other style.
		OutlineF(const OutlineF& _other);

		//! @brief Destructor.
		~OutlineF();

		//! @brief Assignment operator.
		//! @param _other The other style.
		OutlineF& operator = (const OutlineF& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Set the color.
		//! The current painter will be replaced by a FillPainter2D.
		void setColor(const Color& _color);

		//! @brief Set the painter.
		//! The outline takes ownership of the painter.
		//! @param _painter The painter to set.
		void setPainter(Painter2D* _painter);

		//! @brief Painter.
		constexpr inline const Painter2D* painter(void) const { return m_painter; };

		//! @brief Replaces the current painter with a default painter and returns the old painter.
		//! The caller takes ownership of the painter.
		Painter2D* takePainter(void);

		//! @brief Set the line width.
		//! @param _w Width to set.
		constexpr inline void setWidth(double _w) { m_width = _w; };

		//! @brief Line width.
		constexpr inline double width(void) const { return m_width; };

		//! @brief Set the line style.
		//! @param _style Style to set.
		constexpr inline void setStyle(LineStyle _style) { m_style = _style; };

		//! @brief Line style.
		constexpr inline LineStyle style(void) const { return m_style; };

		//! @brief Set the line cap style.
		//! @param _cap Cap style to set.
		constexpr inline void setCap(LineCapStyle _cap) { m_cap = _cap; };

		//! @brief Cap style.
		constexpr inline LineCapStyle cap(void) const { return m_cap; };

		//! @brief Set the line join style.
		//! @param _style Join style to set.
		constexpr inline void setJoinStyle(LineJoinStyle _join) { m_join = _join; };

		//! @brief Line join style.
		constexpr inline LineJoinStyle joinStyle(void) const { return m_join; };

		Outline toOutline(void) const;

	private:
		Painter2D* m_painter; //! @brief Painter.
		double m_width; //! @brief Line width.
		LineStyle m_style; //! @brief Line style.
		LineCapStyle m_cap; //! @brief Line cap style.
		LineJoinStyle m_join; //! @brief Line join style.
	};

}