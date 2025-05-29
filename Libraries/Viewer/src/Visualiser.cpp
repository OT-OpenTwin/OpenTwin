#include "stdafx.h"
#include "FrontendAPI.h"
#include "Visualiser.h"
#include "SceneNodeBase.h"
#include "OTSystem/OTAssert.h"

Visualiser::Visualiser(SceneNodeBase* _sceneNode, ot::WidgetViewBase::ViewType _viewType)
	: m_node(_sceneNode), m_viewType(_viewType)
{
	OTAssertNullptr(m_node);
}

Visualiser::~Visualiser() 
{
	if (m_viewIsOpen) 
	{
		FrontendAPI::instance()->closeView(m_node->getName(), m_viewType);
	}
}
