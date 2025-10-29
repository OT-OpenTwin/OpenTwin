// @otlicense

// AK header
#include <akGui/aTimer.h>

ak::aTimer::aTimer()
	: QTimer(), aObject(otTimer)
{}

ak::aTimer::~aTimer() { A_OBJECT_DESTROYING }