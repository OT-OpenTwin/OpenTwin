#include "OTGui/PainterRainbowIterator.h"

ot::PainterRainbowIterator::PainterRainbowIterator()
	: m_currentColourIndex(static_cast<uint32_t>(ot::ColorStyleValueEntry::RainbowFirst))
{}

std::unique_ptr<ot::StyleRefPainter2D> ot::PainterRainbowIterator::getNextPainter()
{
	ot::ColorStyleValueEntry styleEntry = static_cast<ot::ColorStyleValueEntry>(m_currentColourIndex);

	if (m_currentColourIndex < static_cast<uint32_t>(ot::ColorStyleValueEntry::RainbowLast))
	{
		m_currentColourIndex++;
	}
	else
	{
		m_currentColourIndex = static_cast<uint32_t>(ot::ColorStyleValueEntry::RainbowFirst); //Restart the rainbow
	}

	std::unique_ptr<ot::StyleRefPainter2D>stylePainter(new ot::StyleRefPainter2D(styleEntry));
	return stylePainter;
}
