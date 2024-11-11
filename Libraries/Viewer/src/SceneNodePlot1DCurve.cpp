#include "stdafx.h"

// OpenTwin header
#include "DataBase.h"
#include "SceneNodePlot1DCurve.h"

SceneNodePlot1DCurve::SceneNodePlot1DCurve() :
	m_modelEntityVersion(0)
{

}

SceneNodePlot1DCurve::~SceneNodePlot1DCurve() {

}

void SceneNodePlot1DCurve::setTransparent(bool _transparent)
{
	if (_transparent == isTransparent()) return;

	SceneNodeBase::setTransparent(_transparent);
}

void SceneNodePlot1DCurve::setWireframe(bool _wireframe)
{
	if (_wireframe == isWireframe()) return;

	SceneNodeBase::setWireframe(_wireframe);
}

void SceneNodePlot1DCurve::setVisible(bool _visible)
{
	if (_visible == isVisible()) return;

	SceneNodeBase::setVisible(_visible);
}

void SceneNodePlot1DCurve::setHighlighted(bool _highlighted)
{
}
