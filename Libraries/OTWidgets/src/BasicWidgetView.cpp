// @otlicense

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTWidgets/BasicWidgetView.h"

ot::BasicWidgetView::BasicWidgetView(QWidget* _widget) 
	: WidgetView(WidgetViewBase::CustomView), m_widget(_widget)
{
	OTAssertNullptr(m_widget);
	this->addWidgetToDock(m_widget);
}

ot::BasicWidgetView::~BasicWidgetView() {}