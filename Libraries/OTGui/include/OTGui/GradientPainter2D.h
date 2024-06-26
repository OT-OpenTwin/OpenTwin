//! @file GradientPainter2D.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTCore/Point2D.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/Painter2D.h"
#include "OTGui/GradientPainterStop2D.h"

namespace ot {

	class OT_GUI_API_EXPORT GradientPainter2D : public ot::Painter2D {
		OT_DECL_NOCOPY(GradientPainter2D)
	public:
		GradientPainter2D();
		GradientPainter2D(const std::vector<GradientPainterStop2D>& _stops);
		virtual ~GradientPainter2D();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		virtual ot::Color getDefaultColor(void) const override;

		virtual bool isEqualTo(const Painter2D* _other) const override;

		inline void addStop(double _pos, const ot::Color& _color) { this->addStop(GradientPainterStop2D(_pos, _color)); };
		void addStop(const GradientPainterStop2D& _stop);
		void addStops(const std::vector<GradientPainterStop2D>& _stops);
		void setStops(const std::vector<GradientPainterStop2D>& _stops) { m_stops = _stops; };
		const std::vector<GradientPainterStop2D>& getStops(void) const { return m_stops; };

		void setSpread(GradientSpread _spread) { m_spread = _spread; };
		GradientSpread getSpread(void) const { return m_spread; };

	protected:
		void addStopsAndSpreadToQss(std::string& _targetString) const;

	private:
		std::vector<GradientPainterStop2D> m_stops;
		GradientSpread m_spread;
	};

}