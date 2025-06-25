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
		void setLinePenColor(const Color& _color) { m_linePen.setColor(_color); };
		const PenFCfg& getLinePen() const { return m_linePen; };

		void setPointSymbol(Symbol _symbol) { m_pointsSymbol = _symbol; };
		Symbol getPointSymbol() const { return m_pointsSymbol; };

		void setPointsSize(int _size) { m_pointsSize = _size; };
		int getPointsSize() const { return m_pointsSize; };

		void setPointsOutlinePen(const PenFCfg& _pen) { m_pointsOulinePen = _pen; };
		void setPointsOutlinePenWidth(double _width) { m_pointsOulinePen.setWidth(_width); };
		void setPointsOutlinePenColor(const Color& _color) { m_pointsOulinePen.setColor(_color); };
		const PenFCfg& getPointsOutlinePen() const { return m_pointsOulinePen; };

		void setPointsFillColor(const Color& _color);
		void setPointsFillPainter(Painter2D* _painter);
		const Painter2D* getPointsFillPainter() const { return m_pointsFillPainter; };

		void setQueryInformation(QueryInformation _queryInformation);
		const QueryInformation& getQueryInformation() const;

	private:
		UID m_navigationId;

		std::string m_title;

		bool m_visible;
		bool m_dimmed;

		PenFCfg m_linePen;
		
		int m_pointsSize;
		Symbol m_pointsSymbol;
		PenFCfg m_pointsOulinePen;
		Painter2D* m_pointsFillPainter;

		QueryInformation m_queryInformation;
	};

}