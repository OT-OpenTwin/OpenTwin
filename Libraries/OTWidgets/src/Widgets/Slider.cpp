// @otlicense

// OpenTwin header
#include "OTWidgets/Widgets/Slider.h"

ot::Slider::Slider(QWidget* _parent)
	: Slider(Qt::Horizontal, _parent)
{}

ot::Slider::Slider(Qt::Orientation _orientation, QWidget* _parent)
	: QSlider(_orientation, _parent)
{}