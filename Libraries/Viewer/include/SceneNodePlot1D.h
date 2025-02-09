#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/Plot1DDataBaseCfg.h"
#include "DataBase.h"
#include "SceneNodeBase.h"

// std header
#include <map>
#include <string>

class Model;

class SceneNodePlot1D : public SceneNodeBase {
	OT_DECL_NOCOPY(SceneNodePlot1D)
public:
	SceneNodePlot1D();
	virtual ~SceneNodePlot1D();

	virtual void setTransparent(bool _transparent) override;
	virtual void setWireframe(bool _wireframe) override;
	virtual void setVisible(bool _visible) override;
	virtual void setHighlighted(bool _highlighted) override;

	bool getSelectChildren(void) override { return false; };

	virtual bool isItem1D(void) override { return true; };
	virtual bool isItem3D(void) override { return false; };

	void setConfig(const ot::Plot1DCfg& _config);
	void setDataBaseConfig(const ot::Plot1DDataBaseCfg& _config);
	const ot::Plot1DDataBaseCfg& getConfig(void) const { return m_config; };

	void setModel(Model* _model) { m_model = _model; };
	Model *getModel(void) { return m_model; };

	void addCurve(const ot::Plot1DCurveCfg& _curve);
	void setCurves(const std::list<ot::Plot1DCurveCfg>& _curves);
	const std::list<ot::Plot1DCurveCfg>& getCurves(void) const;
	size_t getNumberOfCurves(void) const;
	const ot::Plot1DCurveCfg& getCurveInfo(size_t _index) const;

	bool updateCurveEntityVersion(ot::UID _curveEntityID, ot::UID _curveVersion);

private:
	void applyConfigInfo(void);
	bool isAnyChildSelectedAndVisible(SceneNodeBase *root);

	ot::Plot1DDataBaseCfg m_config;
	Model* m_model;
};

