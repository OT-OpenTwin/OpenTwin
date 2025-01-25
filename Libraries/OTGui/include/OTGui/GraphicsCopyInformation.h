//! @file GraphicsCopyInformation.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Point2D.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/CopyInformation.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT GraphicsCopyInformation : public CopyInformation {
	public:
		GraphicsCopyInformation() = default;
		GraphicsCopyInformation(const GraphicsCopyInformation&) = default;
		GraphicsCopyInformation(GraphicsCopyInformation&&) = default;
		virtual ~GraphicsCopyInformation() = default;

		GraphicsCopyInformation& operator = (const GraphicsCopyInformation&) = default;
		GraphicsCopyInformation& operator = (GraphicsCopyInformation&&) = default;

		static std::string getGraphicsCopyInformationType(void) { return "GraphicsCopy"; };
		virtual std::string getCopyType(void) const override { return GraphicsCopyInformation::getGraphicsCopyInformationType(); };

		static int getGraphicsCopyInformationVersion(void) { return 1; };
		virtual int getCopyVersion(void) const override { return GraphicsCopyInformation::getGraphicsCopyInformationVersion(); };

		virtual void addToJsonObject(ot::JsonValue & _object, ot::JsonAllocator & _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;
	};

}