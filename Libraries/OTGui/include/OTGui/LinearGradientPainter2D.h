//! @file LinearGradientPainter2D.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/Point2D.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/GradientPainter2D.h"
#include "OTGui/GradientPainterStop2D.h"

#define OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg "OT_P2DCLGrad"

namespace ot {

	class OT_GUI_API_EXPORT LinearGradientPainter2D : public ot::GradientPainter2D {
		OT_DECL_NOCOPY(LinearGradientPainter2D)
	public:
		LinearGradientPainter2D();
		LinearGradientPainter2D(const std::vector<GradientPainterStop2D>& _stops);
		virtual ~LinearGradientPainter2D();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_LinearGradientPainter2DCfg); };
		
		virtual std::string generateQss(void) const override;

		virtual ot::Color getDefaultColor(void) const override;

		//! @brief Set the starting point for the gradient.
		//! The painted area is between 0.0 and 1.0.
		void setStart(const ot::Point2DD& _start) { m_start = _start; };
		const ot::Point2DD& start(void) const { return m_start; }

		//! @brief Set the final stop point for the gradient.
		//! The painted area is between 0.0 and 1.0.
		void setFinalStop(const ot::Point2DD& _finalStop) { m_finalStop = _finalStop; };
		const ot::Point2DD& finalStop(void) const { return m_finalStop; };

	private:
		ot::Point2DD m_start;
		ot::Point2DD m_finalStop;
	};

}