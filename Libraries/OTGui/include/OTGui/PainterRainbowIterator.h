#pragma once
#include "OTGui/StyleRefPainter2D.h"
#include <memory>
namespace ot
{
	class __declspec(dllexport) PainterRainbowIterator
	{
		public:
			PainterRainbowIterator();
			std::unique_ptr<ot::StyleRefPainter2D> getNextPainter();
	private:
		uint32_t m_currentColourIndex;
	};
}
