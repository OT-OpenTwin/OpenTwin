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

void ProjectHelperBatchHelperCustomItem::createFromPath(TreeWidget* _tree, TreeWidgetItem* _parent, const QString& _path, ProjectHelperBatchHelperCustomItem* _slotItem) {
	QString path = _path;
	path.replace('\\', '/');
	while (path.endsWith('/')) {
		path.chop(1);
	}

	tryCreate(_tree, _parent, _slotItem, "Build All", path, "/Scripts/BuildAndTest/BuildAll.bat", false);
	tryCreate(_tree, _parent, _slotItem, "Build Documentation", path, "/Scripts/BuildAndTest/BuildDocumentation.bat", false);
	tryCreate(_tree, _parent, _slotItem, "Build Sphinx Documentation", path, "/Documentation/Developer/build.bat", false);
	tryCreate(_tree, _parent, _slotItem, "Clean All", path, "/Scripts/BuildAndTest/CleanAll.bat", false);
	tryCreate(_tree, _parent, _slotItem, "Create Deployment", path, "/Scripts/BuildAndTest/CreateDeployment.bat", true);
	tryCreate(_tree, _parent, _slotItem, "Create Frontend Installer", path, "/Scripts/BuildAndTest/CreateFrontendInstaller.bat", false);
	tryCreate(_tree, _parent, _slotItem, "Rebuild All", path, "/Scripts/BuildAndTest/RebuildAll.bat", false);
	tryCreate(_tree, _parent, _slotItem, "Shutdown All", path, "/Scripts/BuildAndTest/ShutdownAll.bat", true);
	tryCreate(_tree, _parent, _slotItem, "Test All", path, "/Scripts/BuildAndTest/TestAll.bat", false);
	tryCreate(_tree, _parent, _slotItem, "Update Deployment", path, "/Scripts/BuildAndTest/UpdateDeploymentLibrariesOnly.bat", true);
}

void ProjectHelperBatchHelperCustomItem::referenceDestroyed(ProjectHelperBatchHelperItem* _reference) {
	ProjectHelperBatchHelperCustomItem* actualRef = dynamic_cast<ProjectHelperBatchHelperCustomItem*>(_reference);
	if (!actualRef) {
		return;
	}
	if (m_runButtonRef == actualRef->m_runButton) {
		m_runButtonRef = nullptr;
	}
	if (m_stopButtonRef == actualRef->m_stopButton) {
		m_stopButtonRef = nullptr;
	}
}

void ProjectHelperBatchHelperCustomItem::slotRun() {
	if (m_runner) {
		PHBH_LOGW("Script is already running: \"" + m_name + "\"");
		return;
	}
	m_runner = ScriptRunner::runScript(m_name, m_path, {}, m_rootPath);
	if (m_runner) {
		setButtonState(true);
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

	setButtonState(false);
}

void ProjectHelperBatchHelperCustomItem::slotRunDetached() {
	if (m_runner) {
		PHBH_LOGW("Script is already running: \"" + m_name + "\"");
		return;
	}

	if (ScriptRunner::runDetached(m_name, m_path, {}, m_rootPath)) {
		PHBH_LOG("Detached script executed successfully: \"" + m_path + "\"");
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
	
	setButtonState(false);
}

ProjectHelperBatchHelperItem* ProjectHelperBatchHelperCustomItem::createFavItemImpl(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent) {
	ProjectHelperBatchHelperCustomItem* itm = createItem(_tree, _parent, this, m_name, m_rootPath, m_path, m_detached);
	if (itm) {
		m_runButtonRef = itm->m_runButton;
		m_stopButtonRef = itm->m_stopButton;
		itm->setButtonState(m_runner);
	}
	return itm;
}

ProjectHelperBatchHelperCustomItem::ProjectHelperBatchHelperCustomItem() {
	setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
	setCheckState(0, Qt::Unchecked);
}

void ProjectHelperBatchHelperCustomItem::tryCreate(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, ProjectHelperBatchHelperCustomItem* _slotItem, const QString& _text, const QString& _rootPath, const QString& _subPath, bool _detached) {
	QFile file(_rootPath + _subPath);
	if (file.exists()) {
		createItem(_tree, _parent, _slotItem, _text, _rootPath, _rootPath + _subPath, _detached);
	}
	else {
		PHBH_LOGW("Batch file not found: \"" + _rootPath + _subPath + "\"");
	}
}

ProjectHelperBatchHelperCustomItem* ProjectHelperBatchHelperCustomItem::createItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, ProjectHelperBatchHelperCustomItem* _slotItem, const QString& _text, const QString& _rootPath, const QString& _scriptPath, bool _detached) {
	ProjectHelperBatchHelperCustomItem* item = new ProjectHelperBatchHelperCustomItem;
	item->m_name = _text;
	item->m_path = _scriptPath;
	item->m_rootPath = _rootPath;
	item->m_detached = _detached;
	item->setText(0, _text);

	ProjectHelperBatchHelperCustomItem* actualSlotItm = _slotItem;
	if (!actualSlotItm) {
		actualSlotItm = item;
	}

	_parent->addChild(item);

	if (_detached) {
		item->m_runButton = createScriptButton(_tree, item, 1, "Run Detached", _scriptPath, _detached);
		item->connect(item->m_runButton, &PushButton::clicked, actualSlotItm, &ProjectHelperBatchHelperCustomItem::slotRunDetached);
	}
	else {
		item->m_runButton = createScriptButton(_tree, item, 1, "Run", _scriptPath, _detached);
		item->connect(item->m_runButton, &PushButton::clicked, actualSlotItm, &ProjectHelperBatchHelperCustomItem::slotRun);

		item->m_stopButton = createScriptButton(_tree, item, 2, "Stop", "Stop script execution", _detached);
		item->connect(item->m_stopButton, &PushButton::clicked, actualSlotItm, &ProjectHelperBatchHelperCustomItem::slotStop);
	}

	item->setButtonState(false);

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

void ProjectHelperBatchHelperCustomItem::setButtonState(bool _running) {
	if (m_runButton) {
		m_runButton->setEnabled(!_running);
	}
	if (m_runButtonRef) {
		m_runButtonRef->setEnabled(!_running);
	}
	if (m_stopButton) {
		m_stopButton->setEnabled(_running);
	}
	if (m_stopButtonRef) {
		m_stopButtonRef->setEnabled(_running);
	}
}
