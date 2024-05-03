//! @file LineStyle.h
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

	//! @class LineStyle
	//! @brief The LineStyle class is used to describe how a line should look like.
	class OT_GUI_API_EXPORT LineStyle : public ot::Serializable {
	public:
		//! @brief Default constructor.
		//! Creates a black fill painter.
		LineStyle();

		//! @brief Assignment constructor.
		//! @param _painter Initial painter (if 0, default painter will be set [/ref LineStyle() "See default constructor"]).
		LineStyle(Painter2D* _painter);

		//! @brief Assignemnt constructor.
		//! @param _width Line width 
		//! @param _painter Initial painter (if 0, default painter will be set [/ref LineStyle() "See default constructor"]).
		LineStyle(double _width, Painter2D* _painter);

		//! @brief Copy constructor.
		//! @param _other The other style.
		LineStyle(const LineStyle& _other);

		//! @brief Destructor.
		~LineStyle();

		//! @brief Assignment operator.
		//! @param _other The other style.
		LineStyle& operator = (const LineStyle& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Set the painter.
		//! The LineStyle takes ownership of the painter.
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

	private:
		Painter2D* m_painter; //! @brief Painter.
		double m_width; //! @brief Line width.
	};

}