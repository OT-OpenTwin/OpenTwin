#include "CurveColourSetter.h"

CurveColourSetter::CurveColourSetter(ot::Plot1DCurveCfg& _curveCfg)
{
	m_useRainBowItterator = m_painterRainbowIterator.initiateWithOtherPainter(_curveCfg.getLinePen().getPainter());
	m_painterRainbowIterator.initiateWithOtherPainter(_curveCfg.getLinePen().getPainter());
}

CurveColourSetter::CurveColourSetter(ot::Plot1DCurveCfg& _curveCfg, uint32_t _rainbowBufferSize)
	:CurveColourSetter(_curveCfg)
{
	m_rainbowPainterBuffer.reserve(_rainbowBufferSize);

	for (int i = 0; i < _rainbowBufferSize; i++)
	{
		m_rainbowPainterBuffer.push_back(m_painterRainbowIterator.getNextPainter().release());
	}
}



void CurveColourSetter::setPainter(ot::Plot1DCurveCfg& _curveCfg, bool _hasSingleDatapoint)
{
	if (_hasSingleDatapoint)
	{
		_curveCfg.setPointSymbol(ot::Plot1DCurveCfg::Circle);
		_curveCfg.setLinePenStyle(ot::LineStyle::NoLine);

		ot::Painter2D* painter = nullptr;
		if (m_useRainBowItterator)
		{
			painter = m_painterRainbowIterator.getNextPainter().release();
		}
		else
		{
			painter = _curveCfg.getLinePen().getPainter()->createCopy();
		}
		_curveCfg.setPointFillPainter(painter);
		_curveCfg.setPointOutlinePen(painter);

	}
	else
	{
		ot::Painter2D* painter = nullptr;
		if (m_useRainBowItterator)
		{
			painter = m_painterRainbowIterator.getNextPainter().release();
		}
		else
		{
			painter = _curveCfg.getLinePen().getPainter()->createCopy();
		}
		ot::PenFCfg pen = _curveCfg.getLinePen();
		pen.setPainter(painter);
		_curveCfg.setLinePen(pen);
	}
}

void CurveColourSetter::setPainter(ot::Plot1DCurveCfg& _curveCfg, uint32_t _bufferIndex, bool _hasSingleDatapoint)
{
	if (_hasSingleDatapoint)
	{
		_curveCfg.setPointSymbol(ot::Plot1DCurveCfg::Circle);
		_curveCfg.setLinePenStyle(ot::LineStyle::NoLine);

		ot::Painter2D* painter = nullptr;
		if (m_useRainBowItterator)
		{
			painter = m_rainbowPainterBuffer[_bufferIndex]->createCopy();
		}
		else
		{
			painter = _curveCfg.getLinePen().getPainter()->createCopy();
		}
		_curveCfg.setPointFillPainter(painter);
		_curveCfg.setPointOutlinePen(painter);

	}
	else
	{
		ot::Painter2D* painter = nullptr;
		if (m_useRainBowItterator)
		{
			painter = m_rainbowPainterBuffer[_bufferIndex]->createCopy();
		}
		else
		{
			painter = _curveCfg.getLinePen().getPainter()->createCopy();
		}
		ot::PenFCfg pen = _curveCfg.getLinePen();
		pen.setPainter(painter);
		_curveCfg.setLinePen(pen);
	}
}
