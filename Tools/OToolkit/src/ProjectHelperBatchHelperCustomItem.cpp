// @otlicense

// OpenTwin header
#include "AppBase.h"
#include "ScriptRunner.h"
#include "OToolkitAPI/OToolkitAPI.h"
#include "ProjectHelperBatchHelperCustomItem.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/TreeWidget.h"

// Qt header
#include <QtCore/qdiriterator.h>
#include <QtWidgets/qlayout.h>

#define PHBH_LOG(___message) OTOOLKIT_LOG("BatchHelper", ___message)
#define PHBH_LOGW(___message) OTOOLKIT_LOGW("BatchHelper", ___message)
#define PHBH_LOGE(___message) OTOOLKIT_LOGE("BatchHelper", ___message)

using namespace ot;

void ProjectHelperBatchHelperCustomItem::createFromPath(TreeWidget* _tree, TreeWidgetItem* _parent, const QString& _path) {
	QString path = _path;
	path.replace('\\', '/');
	while (path.endsWith('/')) {
		path.chop(1);
	}

	tryCreate(_tree, _parent, "Build All", path, "/Scripts/BuildAndTest/BuildAll.bat", false);
	tryCreate(_tree, _parent, "Build Documentation", path, "/Scripts/BuildAndTest/BuildDocumentation.bat", false);
	tryCreate(_tree, _parent, "Build Sphinx Documentation", path, "/Documentation/Developer/build.bat", false);
	tryCreate(_tree, _parent, "Clean All", path, "/Scripts/BuildAndTest/CleanAll.bat", false);
	tryCreate(_tree, _parent, "Create Deployment", path, "/Scripts/BuildAndTest/CreateDeployment.bat", true);
	tryCreate(_tree, _parent, "Create Frontend Installer", path, "/Scripts/BuildAndTest/CreateFrontendInstaller.bat", false);
	tryCreate(_tree, _parent, "Rebuild All", path, "/Scripts/BuildAndTest/RebuildAll.bat", false);
	tryCreate(_tree, _parent, "Shutdown All", path, "/Scripts/BuildAndTest/ShutdownAll.bat", true);
	tryCreate(_tree, _parent, "Test All", path, "/Scripts/BuildAndTest/TestAll.bat", false);
	tryCreate(_tree, _parent, "Update Deployment", path, "/Scripts/BuildAndTest/UpdateDeploymentLibrariesOnly.bat", true);
}

void ProjectHelperBatchHelperCustomItem::slotRun() {
	if (m_runner) {
		PHBH_LOGW("Script is already running: \"" + m_name + "\"");
		return;
	}
	m_runner = ScriptRunner::runScript(m_name, m_path, {}, m_rootPath);
	if (m_runner) {
		m_runButton->setEnabled(false);
		if (m_stopButton) {
			m_stopButton->setEnabled(true);
		}
		connect(m_runner, &ScriptRunner::finished, this, &ProjectHelperBatchHelperCustomItem::slotRunnerFinished);
		PHBH_LOG("Script executed successfully: \"" + m_path + "\"");
	}
	else {
		PHBH_LOGE("Script execution failed: \"" + m_path + "\"");
	}
}

void ProjectHelperBatchHelperCustomItem::slotStop() {
	if (m_runner) {
		m_runner->stop();
		m_runner = nullptr;
	}

	m_runButton->setEnabled(true);
	if (m_stopButton) {
		m_stopButton->setEnabled(false);
	}
}

void ProjectHelperBatchHelperCustomItem::slotRunDetached() {
	if (m_runner) {
		PHBH_LOGW("Script is already running: \"" + m_name + "\"");
		return;
	}

	if (ScriptRunner::runDetached(m_name, m_path, {}, m_rootPath)) {
		PHBH_LOG("Detached script executed successfully: \"" + m_path + "\"");
		m_runButton->setEnabled(false);
	}
	else {
		PHBH_LOGE("Detached script execution failed: \"" + m_path + "\"");
	}
}

void ProjectHelperBatchHelperCustomItem::slotRunnerFinished() {
	if (m_runner) {
		disconnect(m_runner, &ScriptRunner::finished, this, &ProjectHelperBatchHelperCustomItem::slotRunnerFinished);
		m_runner = nullptr;
	}
	
	m_runButton->setEnabled(true);
	if (m_stopButton) {
		m_stopButton->setEnabled(false);
	}
}

void ProjectHelperBatchHelperCustomItem::tryCreate(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _text, const QString& _rootPath, const QString& _subPath, bool _detached) {
	QFile file(_rootPath + _subPath);
	if (file.exists()) {
		createItem(_tree, _parent, _text, _rootPath, _rootPath + _subPath, _detached);
	}
	else {
		PHBH_LOGW("Batch file not found: \"" + _rootPath + _subPath + "\"");
	}
}

ProjectHelperBatchHelperCustomItem* ProjectHelperBatchHelperCustomItem::createItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _text, const QString& _rootPath, const QString& _scriptPath, bool _detached) {
	ProjectHelperBatchHelperCustomItem* item = new ProjectHelperBatchHelperCustomItem;
	item->m_name = _text;
	item->m_path = _scriptPath;
	item->m_rootPath = _rootPath;
	item->setText(0, _text);
	_parent->addChild(item);

	if (_detached) {
		item->m_runButton = createScriptButton(_tree, item, 1, "Run Detached", _scriptPath, _detached);
		item->connect(item->m_runButton, &PushButton::clicked, item, &ProjectHelperBatchHelperCustomItem::slotRunDetached);
	}
	else {
		item->m_runButton = createScriptButton(_tree, item, 1, "Run", _scriptPath, _detached);
		item->connect(item->m_runButton, &PushButton::clicked, item, &ProjectHelperBatchHelperCustomItem::slotRun);

		item->m_stopButton = createScriptButton(_tree, item, 2, "Stop", "Stop script execution", _detached);
		item->m_stopButton->setEnabled(false);
		item->connect(item->m_stopButton, &PushButton::clicked, item, &ProjectHelperBatchHelperCustomItem::slotStop);
	}

	return item;
}

ot::PushButton* ProjectHelperBatchHelperCustomItem::createScriptButton(ot::TreeWidget* _tree, ProjectHelperBatchHelperCustomItem* _item, int _column, const QString& _text, const QString& _path, bool _detached) {
	QWidget* btnWrap = new QWidget;
	QGridLayout* layout = new QGridLayout(btnWrap);
	layout->setContentsMargins(2, 2, 2, 2);
	PushButton* btn = new PushButton(_text, btnWrap);
	layout->addWidget(btn, 0, 0);

	btn->setEnabled(!_path.isEmpty());
	btn->setToolTip(_path.isEmpty() ? "Batch not found" : _path);
	_tree->setItemWidget(_item, _column, btnWrap);
	return btn;
}
