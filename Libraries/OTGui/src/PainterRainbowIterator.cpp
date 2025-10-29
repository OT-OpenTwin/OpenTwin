// @otlicense

#include "OTGui/PainterRainbowIterator.h"

ot::PainterRainbowIterator::PainterRainbowIterator()
	: m_currentColourIndex(static_cast<uint32_t>(ot::ColorStyleValueEntry::RainbowFirst))
{}

bool ot::PainterRainbowIterator::initiateWithOtherPainter(const ot::Painter2D* _painter)
{
	const ot::StyleRefPainter2D*  stylePainter = dynamic_cast<const ot::StyleRefPainter2D*>(_painter);
	bool isARainbowPainter = false;
	if (stylePainter != nullptr)
	{
		ot::ColorStyleValueEntry styleKey = stylePainter->getReferenceKey();
		if (styleKey >= ot::ColorStyleValueEntry::RainbowFirst && styleKey <= ot::ColorStyleValueEntry::RainbowLast)
		{
			m_currentColourIndex = static_cast<uint32_t>(styleKey);
			isARainbowPainter = true;
		}
	}
	return isARainbowPainter;
}


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
