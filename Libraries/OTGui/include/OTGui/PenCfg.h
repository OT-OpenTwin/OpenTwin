// @otlicense
// File: PenCfg.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/ColorStyleTypes.h"

namespace ot {

	class PenFCfg;
	class Painter2D;

	//! @brief Line style.
	enum class LineStyle {
		NoLine,
		SolidLine,
		DashLine,
		DotLine,
		DashDotLine,
		DashDotDotLine
	};

	//! @brief Line cap style.
	enum class LineCapStyle {
		FlatCap,
		SquareCap,
		RoundCap
	};

	//! @brief Line join style.
	enum class LineJoinStyle {
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

	//! @class PenCfg
	//! @brief The PenCfg is used to configure a line pen that will be used in the frontend.
	class OT_GUI_API_EXPORT PenCfg : public ot::Serializable {
	public:
		//! @brief Default constructor.
		//! Creates a black fill painter.
		explicit PenCfg();

		//! @brief Assignment constructor.
		//! @param _color Initial color.
		explicit PenCfg(DefaultColor _color);

		//! @brief Assignment constructor.
		//! @param _color Initial color.
		explicit PenCfg(const Color& _color);

		//! @brief Assignment constructor.
		//! @param _painter Initial painter (if 0, default painter will be set [/ref PenCfg() "See default constructor"]).
		explicit PenCfg(Painter2D* _painter);

		//! @brief Assignment constructor.
		//! @param _width Line width.
		//! @param _color Initial color.
		explicit PenCfg(int _width, DefaultColor _color);

		//! @brief Assignment constructor.
		//! @param _width Line width.
		//! @param _color Initial color.
		explicit PenCfg(int _width, const Color& _color);

		//! @brief Assignment constructor.
		//! @param _width Line width 
		//! @param _painter Initial painter (if 0, default painter will be set [/ref PenCfg() "See default constructor"]).
		explicit PenCfg(int _width, Painter2D* _painter);

		//! @brief Copy constructor.
		//! @param _other The other style.
		PenCfg(const PenCfg& _other);

		//! @brief Destructor.
		~PenCfg();

		//! @brief Assignment operator.
		//! @param _other The other style.
		PenCfg& operator = (const PenCfg& _other);

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
		void setColor(DefaultColor _color);

		//! @brief Set the color.
		//! The current painter will be replaced by a FillPainter2D.
		void setColor(const Color& _color);
		
		//! @brief Set the color as a style reference color.
		//! The current painter will be replaced by a StyleRefPainter2D.
		void setColor(ColorStyleValueEntry _color);
		
		//! @brief Set the painter.
		//! The pen takes ownership of the painter.
		//! @param _painter The painter to set.
		void setPainter(Painter2D* _painter);

		//! @brief Painter.
		inline const Painter2D* getPainter(void) const { return m_painter; };

		//! @brief Replaces the current painter with a default painter and returns the old painter.
		//! The caller takes ownership of the painter.
		Painter2D* takePainter();

		//! @brief Set the line width.
		//! @param _w Width to set.
		inline void setWidth(int _w) { m_width = _w; };

		//! @brief Line width.
		inline int getWidth() const { return m_width; };

		//! @brief Set the line style.
		//! @param _style Style to set.
		inline void setStyle(LineStyle _style) { m_style = _style; };

		//! @brief Line style.
		inline LineStyle getStyle() const { return m_style; };

		//! @brief Set the line cap style.
		//! @param _cap Cap style to set.
		inline void setCap(LineCapStyle _cap) { m_cap = _cap; };

		//! @brief Cap style.
		inline LineCapStyle getCap() const { return m_cap; };

		//! @brief Set the line join style.
		//! @param _style Join style to set.
		inline void setJoinStyle(LineJoinStyle _join) { m_join = _join; };

		//! @brief Line join style.
		inline LineJoinStyle getJoinStyle() const { return m_join; };

		PenFCfg toPenFCfg() const;

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

	//! @class PenFCfg
	//! @brief The PenCfg is used to configure a line pen that will be used in the frontend.
	class OT_GUI_API_EXPORT PenFCfg : public ot::Serializable {
	public:
		//! @brief Default constructor.
		//! Creates a black fill painter.
		explicit PenFCfg();

		//! @brief Assignment constructor.
		//! @param _color Initial color.
		explicit PenFCfg(DefaultColor _color);

		//! @brief Assignment constructor.
		//! @param _color Initial color.
		explicit PenFCfg(const Color& _color);

		//! @brief Assignment constructor.
		//! @param _painter Initial painter (if 0, default painter will be set [/ref PenFCfg() "See default constructor"]).
		explicit PenFCfg(Painter2D* _painter);

		//! @brief Assignment constructor.
		//! @param _width Line width.
		//! @param _color Initial color.
		explicit PenFCfg(double _width, DefaultColor _color);

		//! @brief Assignment constructor.
		//! @param _width Line width.
		//! @param _color Initial color.
		explicit PenFCfg(double _width, const Color& _color);

		//! @brief Assignment constructor.
		//! @param _width Line width 
		//! @param _painter Initial painter (if 0, default painter will be set [/ref PenFCfg() "See default constructor"]).
		explicit PenFCfg(double _width, Painter2D* _painter);

		//! @brief Copy constructor.
		//! @param _other The other style.
		PenFCfg(const PenFCfg& _other);

		//! @brief Destructor.
		~PenFCfg();

		//! @brief Assignment operator.
		//! @param _other The other style.
		PenFCfg& operator = (const PenFCfg& _other);

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
		void setColor(DefaultColor _color);

		//! @brief Set the color.
		//! The current painter will be replaced by a FillPainter2D.
		void setColor(const Color& _color);

		//! @brief Set the color as a style reference color.
		//! The current painter will be replaced by a StyleRefPainter2D.
		void setColor(ColorStyleValueEntry _color);

		//! @brief Set the painter.
		//! The pen takes ownership of the painter.
		//! @param _painter The painter to set.
		void setPainter(Painter2D* _painter);

		//! @brief Painter.
		inline const Painter2D* getPainter() const { return m_painter; };

		//! @brief Replaces the current painter with a default painter and returns the old painter.
		//! The caller takes ownership of the painter.
		Painter2D* takePainter();

		//! @brief Set the line width.
		//! @param _w Width to set.
		inline void setWidth(double _w) { m_width = _w; };

		//! @brief Line width.
		inline double getWidth() const { return m_width; };

		//! @brief Set the line style.
		//! @param _style Style to set.
		inline void setStyle(LineStyle _style) { m_style = _style; };

		//! @brief Line style.
		inline LineStyle getStyle() const { return m_style; };

		//! @brief Set the line cap style.
		//! @param _cap Cap style to set.
		inline void setCap(LineCapStyle _cap) { m_cap = _cap; };

		//! @brief Cap style.
		inline LineCapStyle getCap() const { return m_cap; };

		//! @brief Set the line join style.
		//! @param _style Join style to set.
		inline void setJoinStyle(LineJoinStyle _join) { m_join = _join; };

		//! @brief Line join style.
		inline LineJoinStyle getJoinStyle() const { return m_join; };

		PenCfg toPenCfg() const;

	private:
		Painter2D* m_painter; //! @brief Painter.
		double m_width; //! @brief Line width.
		LineStyle m_style; //! @brief Line style.
		LineCapStyle m_cap; //! @brief Line cap style.
		LineJoinStyle m_join; //! @brief Line join style.
	};

}