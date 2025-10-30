// @otlicense

// OpenTwin header
#include "OTGui/GraphicsConnectionCalculationData.h"

std::list<ot::GraphicsConnectionCalculationData> ot::GraphicsConnectionCalculationData::getAllYXLines(void) {
	static std::list<ot::GraphicsConnectionCalculationData> xyLines({
		GraphicsConnectionCalculationData(ConnectionDirection::Left, ConnectionDirection::Left, Alignment::TopRight),
		GraphicsConnectionCalculationData(ConnectionDirection::Left, ConnectionDirection::Left, Alignment::BottomRight),

		GraphicsConnectionCalculationData(ConnectionDirection::Right, ConnectionDirection::Right, Alignment::TopLeft),
		GraphicsConnectionCalculationData(ConnectionDirection::Right, ConnectionDirection::Right, Alignment::BottomLeft),
		
		GraphicsConnectionCalculationData(ConnectionDirection::Up, ConnectionDirection::Up, Alignment::TopLeft),
		GraphicsConnectionCalculationData(ConnectionDirection::Up, ConnectionDirection::Up, Alignment::TopRight),
		
		GraphicsConnectionCalculationData(ConnectionDirection::Down, ConnectionDirection::Down, Alignment::BottomLeft),
		GraphicsConnectionCalculationData(ConnectionDirection::Down, ConnectionDirection::Down, Alignment::BottomRight),

		GraphicsConnectionCalculationData(ConnectionDirection::Left, ConnectionDirection::Up, Alignment::TopLeft),
		GraphicsConnectionCalculationData(ConnectionDirection::Left, ConnectionDirection::Up, Alignment::TopRight),
		GraphicsConnectionCalculationData(ConnectionDirection::Left, ConnectionDirection::Up, Alignment::BottomRight),

		GraphicsConnectionCalculationData(ConnectionDirection::Left, ConnectionDirection::Down, Alignment::TopRight),
		GraphicsConnectionCalculationData(ConnectionDirection::Left, ConnectionDirection::Down, Alignment::BottomRight),
		GraphicsConnectionCalculationData(ConnectionDirection::Left, ConnectionDirection::Down, Alignment::BottomLeft),

		GraphicsConnectionCalculationData(ConnectionDirection::Right, ConnectionDirection::Up, Alignment::TopLeft),
		GraphicsConnectionCalculationData(ConnectionDirection::Right, ConnectionDirection::Up, Alignment::TopRight),
		GraphicsConnectionCalculationData(ConnectionDirection::Right, ConnectionDirection::Up, Alignment::BottomLeft),

		GraphicsConnectionCalculationData(ConnectionDirection::Right, ConnectionDirection::Down, Alignment::TopLeft),
		GraphicsConnectionCalculationData(ConnectionDirection::Right, ConnectionDirection::Down, Alignment::BottomRight),
		GraphicsConnectionCalculationData(ConnectionDirection::Right, ConnectionDirection::Down, Alignment::BottomLeft),

		GraphicsConnectionCalculationData(ConnectionDirection::Up, ConnectionDirection::Left, Alignment::TopRight),

		GraphicsConnectionCalculationData(ConnectionDirection::Up, ConnectionDirection::Right, Alignment::TopLeft),
		
		GraphicsConnectionCalculationData(ConnectionDirection::Down, ConnectionDirection::Left, Alignment::BottomRight),
		
		GraphicsConnectionCalculationData(ConnectionDirection::Down, ConnectionDirection::Right, Alignment::BottomLeft)
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
