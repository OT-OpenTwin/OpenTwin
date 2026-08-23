// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ProjectInformation.h"
#include "OTCore/ProjectCompareConfig.h"
#include "OTGui/VersionGraphCfg.h"
#include "OTWidgets/Dialog/Dialog.h"

// std header
#include <map>

class QWidget;
class QHBoxLayout;

namespace ot
{
	class Label;
	class CheckBox;
	class PushButton;
	class VersionGraph;
	class VersionGraphManager;
	class ProjectOverviewWidget;
}

class ProjectCompareSetupDialog : public ot::Dialog
{
	Q_OBJECT
	OT_DECL_NOCOPY(ProjectCompareSetupDialog)
	OT_DECL_NOMOVE(ProjectCompareSetupDialog)
	OT_DECL_NODEFAULT(ProjectCompareSetupDialog)
public:
	ProjectCompareSetupDialog(const ot::DialogCfg& _config, QWidget* _parent);

	ot::ProjectCompareConfig getCompareConfig() const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public slots

public Q_SLOTS:
	void refreshProjects();
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Private slots

private Q_SLOTS:
	void slotBack();
	void slotNext();
	void slotConfirm();
	void updateButtonStates();
	void refreshVersionGraph();
	void updateSummary();

	void slotParseDataChanged();
	void slotParseResultsChanged();

private:

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private data accessors

	bool getParseProperties() const;
	bool getParseData() const;
	bool getParseResults() const;

	std::string getSourceProjectName() const;
	std::string getSourceProjectVersion() const;
	std::string getTargetProjectName() const;
	std::string getTargetProjectVersion() const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private helper

	enum class DialogState  
	{
		ProjectSelection,
		VersionSelection,
		Summary
	};

	QWidget* getCurrentViewWidget();
	void unsetView();
	void switchToProjectSelection();
	void switchToVersionSelection();
	void switchToSummary();

	DialogState m_state;

	QHBoxLayout* m_viewLayout;

	ot::ProjectOverviewWidget* m_projectOverview;
	ot::VersionGraphManager* m_versionGraph;
	
	struct SummaryWidget
	{
		QWidget* root;
		ot::Label* sourceProjectLabel;
		ot::Label* sourceProjectVersionLabel;
		ot::Label* targetProjectLabel;
		ot::Label* targetProjectVersionLabel;
		ot::CheckBox* parseProperties;
		ot::CheckBox* parseData;
		bool previousParseResults = false;
		ot::CheckBox* parseResults;
	};
	SummaryWidget m_summaryWidget;

	ot::ProjectInformation m_selectedProject;
	std::string m_selectedVersion;
	std::map<std::string, ot::VersionGraphCfg> m_versionGraphCache;

	ot::PushButton* m_nextButton;
	ot::PushButton* m_backButton;
	ot::PushButton* m_confirmButton;
};