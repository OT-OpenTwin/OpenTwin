#pragma once

namespace osg
{
	class Node;
	class Switch;
}

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/OTClassHelper.h"
#include "DataBase.h"
#include "SceneNodeBase.h"

// std header
#include <string>
#include <map>

class Model;

class SceneNodePlot1DCurve : public SceneNodeBase {
	OT_DECL_NOCOPY(SceneNodePlot1DCurve)
public:
	SceneNodePlot1DCurve();
	virtual ~SceneNodePlot1DCurve();

	virtual void setTransparent(bool _transparent) override;
	virtual void setWireframe(bool _wireframe) override;
	virtual void setVisible(bool _visible) override;
	virtual void setHighlighted(bool _highlighted) override;

	void setModelEntityVersion(ot::UID _version) { m_modelEntityVersion = _version; };
	ot::UID getModelEntityVersion(void) { return m_modelEntityVersion; };

	virtual bool isItem1D(void) { return true; };
	virtual bool isItem3D(void) { return false; };

private:
	ot::UID m_modelEntityVersion;
};

