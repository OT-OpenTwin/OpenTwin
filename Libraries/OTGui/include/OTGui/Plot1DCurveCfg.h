// @otlicense

//! @file Plot1DCurveCfg.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/BasicEntityInformation.h"
#include "OTGui/PenCfg.h"
#include "OTGui/QueryInformation.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT Plot1DCurveCfg : public BasicEntityInformation {
	public:
		enum Symbol {
			NoSymbol,
			Circle,
			Square,
			Diamond,
			TriangleUp,
			TriangleDown,
			TriangleLeft,
			TriangleRight,
			Cross,
			XCross,
			HLine,
			VLine,
			Star6,
			Star8,
			Hexagon
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Static helper

		static std::string toString(Symbol _symbol);
		static Symbol stringToSymbol(const std::string& _symbol);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor / Destructor

		Plot1DCurveCfg();
		Plot1DCurveCfg(UID _id, UID _version, const std::string& _name);
		Plot1DCurveCfg(const Plot1DCurveCfg& _other);
		Plot1DCurveCfg(Plot1DCurveCfg&& _other) noexcept;
		virtual ~Plot1DCurveCfg();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Opearator

		Plot1DCurveCfg& operator = (Plot1DCurveCfg&& _other) noexcept;
		Plot1DCurveCfg& operator = (const Plot1DCurveCfg& _other);
		Plot1DCurveCfg& operator = (const BasicEntityInformation& _other);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setNavigationId(UID _id) { m_navigationId = _id; };
		UID getNavigationId() const { return m_navigationId; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& getTitle() const { return m_title; };

		void setVisible(bool _isVisible) { m_visible = _isVisible; };
		bool getVisible() const { return m_visible; };

		void setDimmed(bool _isDimmed) { m_dimmed = _isDimmed; };
		bool getDimmed() const { return m_dimmed; };

		void setLinePen(const PenFCfg& _pen) { m_linePen = _pen; };
		void setLinePenWidth(double _width) { m_linePen.setWidth(_width); };
		void setLinePenColor(DefaultColor _color) { m_linePen.setColor(_color); };
		void setLinePenColor(const Color& _color) { m_linePen.setColor(_color); };
		void setLinePenColor(ColorStyleValueEntry _styleReference) { m_linePen.setColor(_styleReference); };
		void setLinePenStyle(LineStyle _style) { m_linePen.setStyle(_style); };

		//! @brief Sets the painter for the line pen.
		//! The pen takes ownership of the painter.
		//! @param _painter Painter to set.
		void setLinePenPainter(Painter2D* _painter) { m_linePen.setPainter(_painter); };

		const PenFCfg& getLinePen() const { return m_linePen; };

		void setPointSymbol(Symbol _symbol) { m_pointSymbol = _symbol; };
		Symbol getPointSymbol() const { return m_pointSymbol; };

		void setPointSize(int _size) { m_pointSize = _size; };
		int getPointSize() const { return m_pointSize; };

		void setPointInterval(int _interval) { m_pointInterval = _interval; };
		int getPointInterval() const { return m_pointInterval; };

		void setPointOutlinePen(const PenFCfg& _pen) { m_pointOulinePen = _pen; };
		void setPointOutlinePenWidth(double _width) { m_pointOulinePen.setWidth(_width); };
		void setPointOutlinePenColor(DefaultColor _color) { m_pointOulinePen.setColor(_color); };
		void setPointOutlinePenColor(const Color& _color) { m_pointOulinePen.setColor(_color); };
		void setPointOutlinePenColor(ColorStyleValueEntry _styleReference) { m_pointOulinePen.setColor(_styleReference); };
		void setPointOutlinePenStyle(LineStyle _style) { m_pointOulinePen.setStyle(_style); };

		//! @brief Sets the painter for the point outline pen.
		//! The pen takes ownership of the painter.
		//! @param _painter Painter to set.
		void setPointOutlinePenPainter(Painter2D* _painter) { m_pointOulinePen.setPainter(_painter); };
		const PenFCfg& getPointOutlinePen() const { return m_pointOulinePen; };

		void setPointFillColor(DefaultColor _color);
		void setPointFillColor(const Color& _color);
		void setPointFillColor(ColorStyleValueEntry _styleReference);

		//! @brief Sets the painter for the point fill.
		//! The curve takes ownership of the painter.
		//! @param _painter Painter to set.
		void setPointFillPainter(Painter2D* _painter);

		//! @brief Returns the painter for the point fill.
		//! The curve keeps ownership of the painter.
		const Painter2D* getPointFillPainter() const { return m_pointFillPainter; };

		void setQueryInformation(QueryInformation _queryInformation) { m_queryInformation = _queryInformation; };
		const QueryInformation& getQueryInformation() const { return m_queryInformation; };

	private:
		UID m_navigationId;

		std::string m_title;

		bool m_visible;
		bool m_dimmed;

		PenFCfg m_linePen;
		
		int m_pointSize;
		int m_pointInterval;
		Symbol m_pointSymbol;
		PenFCfg m_pointOulinePen;
		Painter2D* m_pointFillPainter;

		QueryInformation m_queryInformation;
	};

}