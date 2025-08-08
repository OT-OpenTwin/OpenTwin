#pragma once
#include "OTGui/Plot1DCurveCfg.h"
#include "OTGui/PainterRainbowIterator.h"
class CurveColourSetter
{
public:
	CurveColourSetter(ot::Plot1DCurveCfg& _curveCfg);
	CurveColourSetter(ot::Plot1DCurveCfg& _curveCfg, uint32_t _rainbowBufferSize);
	
	void setPainter(ot::Plot1DCurveCfg& _curveCfg, bool _hasSingleDatapoint);
	void setPainter(ot::Plot1DCurveCfg& _curveCfg, uint32_t _bufferIndex ,bool _hasSingleDatapoint);

private:
	ot::PainterRainbowIterator m_painterRainbowIterator;
	std::vector<ot::StyleRefPainter2D*> m_rainbowPainterBuffer;
	bool m_useRainBowItterator = false;
};
