#include "stdafx.h"
#include "Notifier.h"
#include "Visualiser.h"
#include "SceneNodeBase.h"
#include "OTCore/OTAssert.h"

Visualiser::Visualiser(SceneNodeBase* _sceneNode, ot::WidgetViewBase::ViewType _viewType)
	: m_node(_sceneNode), m_visible(true), m_viewIsOpen(false), m_viewType(_viewType)
{
	OTAssertNullptr(m_node);
}

Visualiser::~Visualiser() {
	if (m_viewIsOpen) {
		getNotifier()->closeView(m_node->getName(), m_viewType);
	}
}
