#pragma once

namespace osg
{
	class Node;
	class Switch;
}

#include "SceneNodeBase.h"
#include "TextVisualiser.h"
#include "Model.h"

#include <string>
#include <map>
class Model;

class SceneNodeMultiVisualisation : public SceneNodeBase
{
public:
	SceneNodeMultiVisualisation() = default;
	virtual ~SceneNodeMultiVisualisation();

	void setViewChange(const ot::ViewChangedStates& _state, const ot::WidgetViewBase::ViewType& _viewType);

	virtual void setSelected(bool _selection) override;

	void setStorage(const std::string& _projectName) { projectName = _projectName; }

	std::string getProjectName(void) { return projectName; }

	Model* getModel(void) { return model; };
	void setModel(Model* _model) { model = _model; };

	virtual bool isItem1D(void) { return false; };
	virtual bool isItem3D(void) { return false; };

private:
	std::string projectName;
	Model* model = nullptr;
};

