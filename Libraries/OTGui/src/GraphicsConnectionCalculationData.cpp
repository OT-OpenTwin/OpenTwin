//! @file GraphicsConnectionCalculationData.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsConnectionCalculationData.h"

std::list<ot::GraphicsConnectionCalculationData> ot::GraphicsConnectionCalculationData::getAllYXLines(void) {
	static std::list<ot::GraphicsConnectionCalculationData> xyLines({
		GraphicsConnectionCalculationData(ConnectLeft, ConnectLeft, AlignTopRight),
		GraphicsConnectionCalculationData(ConnectLeft, ConnectLeft, AlignBottomRight),

		GraphicsConnectionCalculationData(ConnectRight, ConnectRight, AlignTopLeft),
		GraphicsConnectionCalculationData(ConnectRight, ConnectRight, AlignBottomLeft),
		
		GraphicsConnectionCalculationData(ConnectUp, ConnectUp, AlignTopLeft),
		GraphicsConnectionCalculationData(ConnectUp, ConnectUp, AlignTopRight),
		
		GraphicsConnectionCalculationData(ConnectDown, ConnectDown, AlignBottomLeft),
		GraphicsConnectionCalculationData(ConnectDown, ConnectDown, AlignBottomRight),

		GraphicsConnectionCalculationData(ConnectLeft, ConnectUp, AlignTopLeft),
		GraphicsConnectionCalculationData(ConnectLeft, ConnectUp, AlignTopRight),
		GraphicsConnectionCalculationData(ConnectLeft, ConnectUp, AlignBottomRight),

		GraphicsConnectionCalculationData(ConnectLeft, ConnectDown, AlignTopRight),
		GraphicsConnectionCalculationData(ConnectLeft, ConnectDown, AlignBottomRight),
		GraphicsConnectionCalculationData(ConnectLeft, ConnectDown, AlignBottomLeft),

		GraphicsConnectionCalculationData(ConnectRight, ConnectUp, AlignTopLeft),
		GraphicsConnectionCalculationData(ConnectRight, ConnectUp, AlignTopRight),
		GraphicsConnectionCalculationData(ConnectRight, ConnectUp, AlignBottomLeft),

		GraphicsConnectionCalculationData(ConnectRight, ConnectDown, AlignTopLeft),
		GraphicsConnectionCalculationData(ConnectRight, ConnectDown, AlignBottomRight),
		GraphicsConnectionCalculationData(ConnectRight, ConnectDown, AlignBottomLeft),

		GraphicsConnectionCalculationData(ConnectUp, ConnectLeft, AlignTopRight),

		GraphicsConnectionCalculationData(ConnectUp, ConnectRight, AlignTopLeft),
		
		GraphicsConnectionCalculationData(ConnectDown, ConnectLeft, AlignBottomRight),
		
		GraphicsConnectionCalculationData(ConnectDown, ConnectRight, AlignBottomLeft)
		});
	return xyLines;
}

ot::GraphicsConnectionCalculationData::GraphicsConnectionCalculationData(ConnectionDirection _originDirection, ConnectionDirection _destinationDirection, Alignment _connectionLineAlignment) 
	: m_originDir(_originDirection), m_destDir(_destinationDirection), m_alignment(_connectionLineAlignment) {}

ot::GraphicsConnectionCalculationData::GraphicsConnectionCalculationData(const GraphicsConnectionCalculationData& _other)
	: m_originDir(_other.m_originDir), m_destDir(_other.m_destDir), m_alignment(_other.m_alignment) {}

ot::GraphicsConnectionCalculationData::~GraphicsConnectionCalculationData() {}

bool ot::GraphicsConnectionCalculationData::operator == (const GraphicsConnectionCalculationData& _other) const {
	return m_originDir == _other.m_originDir && m_destDir == _other.m_destDir && m_alignment == _other.m_alignment;
}
