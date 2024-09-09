//! @file GraphicsConnectionCalculationData.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT GraphicsConnectionCalculationData {
		OT_DECL_NODEFAULT(GraphicsConnectionCalculationData)
	public:
		static std::list<GraphicsConnectionCalculationData> getAllYXLines(void);

		GraphicsConnectionCalculationData(ConnectionDirection _originDirection, ConnectionDirection _destinationDirection, Alignment _connectionLineAlignment);
		GraphicsConnectionCalculationData(const GraphicsConnectionCalculationData& _other);
		~GraphicsConnectionCalculationData();

		GraphicsConnectionCalculationData& operator = (const GraphicsConnectionCalculationData& _other) = delete;

		bool operator == (const GraphicsConnectionCalculationData& _other) const;

	private:
		ConnectionDirection m_originDir;
		ConnectionDirection m_destDir;
		Alignment m_alignment;

	};

}