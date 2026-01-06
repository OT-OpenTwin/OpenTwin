// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Size2D.h"
#include "OTGui/PenCfg.h"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsDecorationCfg : public Serializable {
	public:
		enum Symbol : int32_t {
			NoSymbol,
			Triangle,
			FilledTriange,
			Arrow,
			VArrow,
			Circle,
			FilledCircle,
			Square,
			FilledSquare,
			Diamond,
			FilledDiamond,
			Cross
		};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Static functions

		static std::string symbolToString(Symbol _symbol);
		static Symbol stringToSymbol(const std::string& _symbol);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructors / Destructor / Assignment

		GraphicsDecorationCfg(Symbol _symbol = Symbol::NoSymbol, bool _reverse = false);
		GraphicsDecorationCfg(const GraphicsDecorationCfg& _other);
		GraphicsDecorationCfg(GraphicsDecorationCfg&& _other) noexcept;
		GraphicsDecorationCfg(const ConstJsonObject& _jsonObject);
		virtual ~GraphicsDecorationCfg();

		GraphicsDecorationCfg& operator = (const GraphicsDecorationCfg& _other);
		GraphicsDecorationCfg& operator = (GraphicsDecorationCfg&& _other) noexcept;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setSymbol(Symbol _symbol) { m_symbol = _symbol; };
		Symbol getSymbol() const { return m_symbol; };

		void setReverse(bool _reverse) { m_reverse = _reverse; };
		bool getReverse() const { return m_reverse; };

		void setSize(const Size2DD& _size) { m_size = _size; };
		void setSize(double _width, double _height) { m_size = Size2DD(_width, _height); };
		const Size2DD& getSize() const { return m_size; };

		void setOutlinePen(const PenFCfg& _pen) { m_outlinePen = _pen; };
		const PenFCfg& getOutlinePen() const { return m_outlinePen; };

		//! @brief Set the fill painter.
		//! @param _painter Pointer to the painter to set. The decoration takes ownership of the painter.
		void setFillPainter(Painter2D* _painter);
		const Painter2D* getFillPainter() const { return m_fillPainter; };

	private:
		Symbol m_symbol;
		bool m_reverse;
		Size2DD m_size;

		PenFCfg m_outlinePen;
		Painter2D* m_fillPainter;

	};

}
