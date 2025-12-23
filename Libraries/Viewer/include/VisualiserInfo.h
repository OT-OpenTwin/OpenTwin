// @otlicense

#pragma once

// std heder
#include <list>

class Visualiser;

namespace ot {

	struct VisualiserInfo {
		std::list<Visualiser*> handledVisualisers;
	};

}
