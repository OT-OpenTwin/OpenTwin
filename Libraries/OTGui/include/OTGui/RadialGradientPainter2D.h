//! @file RadialGradientPainter2D.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTCore/Point2D.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/GradientPainter2D.h"
#include "OTGui/GradientPainterStop2D.h"

#define OT_SimpleFactoryJsonKeyValue_RadialGradientPainter2DCfg "OT_P2DCRGrad"

namespace ot {

	class OT_GUI_API_EXPORT RadialGradientPainter2D : public ot::GradientPainter2D {
		OT_DECL_NOCOPY(RadialGradientPainter2D)
	public:
		RadialGradientPainter2D();
		virtual ~RadialGradientPainter2D();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_RadialGradientPainter2DCfg); };

		//! @brief Set the center point for the gradient
		void setCenterPoint(const ot::Point2DD& _center) { m_center = _center; };
		const ot::Point2DD& centerPoint(void) { return m_center; }

		void setCenterRadius(double _r) { m_centerRadius = _r; };
		double centerRadius(void) const { return m_centerRadius; };

		//! @brief Set the focal point for the gradient
		void setFocalPoint(const ot::Point2DD& _focal);
		const ot::Point2DD& focalPoint(void) const { return m_focal; };

		//! @brief Set the focal radius
		//! Note that the focal point needs to be set
		void setFocalRadius(double _r) { m_focalRadius = _r; };
		double focalRadius(void) const { return m_focalRadius; };

		bool isFocalPointSet(void) const { return m_focalSet; };

	private:
		ot::Point2DD m_center;
		double m_centerRadius;
		ot::Point2DD m_focal;
		double m_focalRadius;
		bool m_focalSet;
	};

}