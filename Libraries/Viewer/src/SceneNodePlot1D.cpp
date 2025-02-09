#include "stdafx.h"

#include "SceneNodePlot1D.h"

#include "DataBase.h"
#include "Model.h"

#include <osg/StateSet>
#include <osg/Node>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/LightModel>

SceneNodePlot1D::SceneNodePlot1D() :
	m_model(nullptr)
{

}

SceneNodePlot1D::~SceneNodePlot1D() {

}

void SceneNodePlot1D::setTransparent(bool _transparent) {
	if (_transparent == isTransparent()) return;

	SceneNodeBase::setTransparent(_transparent);
}

void SceneNodePlot1D::setWireframe(bool _wireframe) {
	if (_wireframe == isWireframe()) return;

	SceneNodeBase::setWireframe(_wireframe);
}

void SceneNodePlot1D::setVisible(bool _visible) {
	if (_visible == isVisible()) return;

	SceneNodeBase::setVisible(_visible);
}

void SceneNodePlot1D::setHighlighted(bool _highlighted) {

}

void SceneNodePlot1D::setConfig(const ot::Plot1DCfg& _config) {
	m_config = _config;
	this->applyConfigInfo();
}

void SceneNodePlot1D::setDataBaseConfig(const ot::Plot1DDataBaseCfg& _config) {
	m_config = _config;
	this->applyConfigInfo();
}

bool SceneNodePlot1D::updateCurveEntityVersion(ot::UID _curveEntityID, ot::UID _curveVersion)
{
	return m_config.updateCurveVersion(_curveEntityID, _curveVersion) && this->isAnyChildSelectedAndVisible(this);
}

void SceneNodePlot1D::addCurve(const ot::Plot1DCurveCfg& _curve) {
	m_config.addCurve(_curve);
}

void SceneNodePlot1D::setCurves(const std::list<ot::Plot1DCurveCfg>& _curves) {
	m_config.setCurves(_curves);
}

const std::list<ot::Plot1DCurveCfg>& SceneNodePlot1D::getCurves(void) const {
	return m_config.getCurves();
}

size_t SceneNodePlot1D::getNumberOfCurves(void) const {
	return m_config.getCurves().size();
}

const ot::Plot1DCurveCfg& SceneNodePlot1D::getCurveInfo(size_t _index) const {
	size_t ix = 0;
	for (const ot::Plot1DCurveCfg& curve : m_config.getCurves()) {
		if (_index == ix++) {
			return curve;
		}
	}
	throw std::out_of_range("Index (" + std::to_string(_index) + ") is out of range");
}

void SceneNodePlot1D::applyConfigInfo(void) {
	this->setName(m_config.getName());
	this->setModelEntityID(m_config.getUid());
	OldTreeIcon icons;
	icons.size = 32;
	icons.visibleIcon = m_config.getOldTreeIcons().getVisibleIcon();
	icons.hiddenIcon = m_config.getOldTreeIcons().getHiddenIcon();
	this->setOldTreeIcons(icons);
}

bool SceneNodePlot1D::isAnyChildSelectedAndVisible(SceneNodeBase *root)
{
	if (root->isSelected() && root->isVisible()) return true;

	bool selected = false;

	for (auto child : root->getChildren())
	{
		selected |= isAnyChildSelectedAndVisible(child);
		if (selected) break;
	}

	return selected;
}
