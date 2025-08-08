#pragma once
#include "OTGui/StyleRefPainter2D.h"
#include <memory>
namespace ot
{
	class __declspec(dllexport) PainterRainbowIterator
	{
		public:
			PainterRainbowIterator();
			bool initiateWithOtherPainter(const ot::Painter2D* _painter);
			std::unique_ptr<ot::StyleRefPainter2D> getNextPainter();
	private:
		uint32_t m_currentColourIndex;
	};
}
