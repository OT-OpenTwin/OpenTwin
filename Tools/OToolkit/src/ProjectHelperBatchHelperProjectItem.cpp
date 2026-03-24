// @otlicense

// OpenTwin header
#include "AppBase.h"
#include "ScriptRunner.h"
#include "OToolkitAPI/OToolkitAPI.h"
#include "ProjectHelperBatchHelperProjectItem.h"
#include "OTWidgets/Widgets/PushButton.h"
#include "OTWidgets/Widgets/TreeWidget.h"

// Qt header
#include <QtCore/qdiriterator.h>
#include <QtWidgets/qlayout.h>

#define PHBH_LOG(___message) OTOOLKIT_LOG("BatchHelper", ___message)
#define PHBH_LOGW(___message) OTOOLKIT_LOGW("BatchHelper", ___message)
#define PHBH_LOGE(___message) OTOOLKIT_LOGE("BatchHelper", ___message)

using namespace ot;

ProjectHelperBatchHelperProjectItem* ProjectHelperBatchHelperProjectItem::createFromPath(TreeWidget* _tree, TreeWidgetItem* _parent, const QString& _path, const CreateFlags& _flags, ProjectHelperBatchHelperProjectItem* _slotItem) {
	// Check if project file exists in the given path
	QString projPath;
	QString buildPath;
	QString cleanPath;
	QString editPath;
	QString testPath;

	QDirIterator dirIt(_path, QDir::Files | QDir::NoDotAndDotDot);
	while (dirIt.hasNext()) {
		dirIt.next();
		QString pth = dirIt.filePath().toLower();
		if (pth.endsWith(".sln") || pth.endsWith(".vcxproj") || pth.endsWith("cmakelists.txt")) {
			projPath = dirIt.filePath();
		}
		else if (dirIt.fileName().toLower() == "build.bat") {
			buildPath = dirIt.filePath();
		}
		else if (dirIt.fileName().toLower() == "clean.bat") {
			cleanPath = dirIt.filePath();
		}
		else if (dirIt.fileName().toLower() == "edit.bat") {
			editPath = dirIt.filePath();
		}
		else if (dirIt.fileName().toLower() == "test.bat") {
			testPath = dirIt.filePath();
		}
	}

	if (projPath.isEmpty() || (buildPath.isEmpty() && cleanPath.isEmpty() && editPath.isEmpty() && testPath.isEmpty())) {
		return nullptr;
	}

	QStringList pathParts = _path.split('/', Qt::SkipEmptyParts);
	OTAssert(pathParts.size() > 0, "Invalid path provided");
	QString nameTxt = pathParts.last();
	if (_flags.has(CreateFlag::RemoveOTPrefix)) {
		if (nameTxt.startsWith("OT", Qt::CaseSensitive)) {
			nameTxt = nameTxt.mid(2);
		}
	}

	ProjectHelperBatchHelperProjectItem* item = new ProjectHelperBatchHelperProjectItem(_tree, _parent, nameTxt, _path);
	item->m_createFlags = _flags;
	item->m_buildPath = buildPath;
	item->m_cleanPath = cleanPath;
	item->m_editPath = editPath;
	item->m_testPath = testPath;

	if (!_slotItem) {
		_slotItem = item;
	}

	item->connect(createScriptButton(_tree, item, static_cast<int>(Columns::Edit), "Edit", editPath), &PushButton::clicked, _slotItem, &ProjectHelperBatchHelperProjectItem::slotEdit);
	item->connect(createScriptButton(_tree, item, static_cast<int>(Columns::Build), "Build", buildPath), &PushButton::clicked, _slotItem, &ProjectHelperBatchHelperProjectItem::slotBuild);
	item->connect(createScriptButton(_tree, item, static_cast<int>(Columns::Rebuild), "Rebuild", buildPath), &PushButton::clicked, _slotItem, &ProjectHelperBatchHelperProjectItem::slotRebuild);
	item->connect(createScriptButton(_tree, item, static_cast<int>(Columns::Test), "Test", testPath), &PushButton::clicked, _slotItem, &ProjectHelperBatchHelperProjectItem::slotTest);
	item->connect(createScriptButton(_tree, item, static_cast<int>(Columns::Clean), "Clean", cleanPath), &PushButton::clicked, _slotItem, &ProjectHelperBatchHelperProjectItem::slotClean);

	return item;
}

ProjectHelperBatchHelperProjectItem::~ProjectHelperBatchHelperProjectItem() {

}

void ProjectHelperBatchHelperProjectItem::referenceDestroyed(ProjectHelperBatchHelperItem* _reference) {

}

void ProjectHelperBatchHelperProjectItem::slotBuild() {
	ScriptRunner* runner = ScriptRunner::runScript(getItemName() + " - Build", m_buildPath, { "BOTH", "BUILD" }, getRootPath());
	if (runner) {
		connect(runner, &ScriptRunner::finished, this, &ProjectHelperBatchHelperProjectItem::slotBuildFinished);
		PHBH_LOG("Build batch executed successfully: \"" + m_buildPath + "\"");
	}
	else {
		PHBH_LOGE("Build batch execution failed: \"" + m_buildPath + "\"");
	}
}

void ProjectHelperBatchHelperProjectItem::slotRebuild() {
	ScriptRunner* runner = ScriptRunner::runScript(getItemName() + " - Rebuild", m_buildPath, { "BOTH", "REBUILD" }, getRootPath());
	if (runner) {
		connect(runner, &ScriptRunner::finished, this, &ProjectHelperBatchHelperProjectItem::slotBuildFinished);
		PHBH_LOG("Rebuild batch executed successfully: \"" + m_buildPath + "\"");
	}
	else {
		PHBH_LOGE("Rebuild batch execution failed: \"" + m_buildPath + "\"");
	}
}

void ProjectHelperBatchHelperProjectItem::slotClean() {
	if (ScriptRunner::runScript(getItemName() + " - Clean", m_cleanPath, {}, getRootPath())) {
		PHBH_LOG("Clear batch executed successfully: \"" + m_cleanPath + "\"");
	}
	else {
		PHBH_LOGE("Clear batch execution failed: \"" + m_cleanPath + "\"");
	}
}

void ProjectHelperBatchHelperProjectItem::slotEdit() {
	if (ScriptRunner::runDetached(getItemName() + " - Edit", m_editPath, {}, getRootPath())) {
		PHBH_LOG("Edit batch executed successfully: \"" + m_editPath + "\"");
	}
	else {
		PHBH_LOGE("Edit batch execution failed: \"" + m_editPath + "\"");
	}
}

void ProjectHelperBatchHelperProjectItem::slotTest() {
	if (ScriptRunner::runScript(getItemName() + " - Test", m_testPath, { "--output-on-failure" }, getRootPath())) {
		PHBH_LOG("Test batch executed successfully: \"" + m_testPath + "\"");
	}
	else {
		PHBH_LOGE("Test batch execution failed: \"" + m_testPath + "\"");
	}
}

void ProjectHelperBatchHelperProjectItem::slotBuildFinished() {
	QFile dbgFile(getRootPath() + "buildlog_Debug.txt");
	if (dbgFile.open(QFile::ReadOnly | QFile::Text)) {
		QTextStream in(&dbgFile);
		QString lastLine;
		while (!in.atEnd()) {
			QString line = in.readLine();
			if (!line.isEmpty()) {
				lastLine = line;
			}
		}
		dbgFile.close();
		if (lastLine.contains("Build successful", Qt::CaseInsensitive)) {
			PHBH_LOG("Debug Build Successful");
		}
		else {
			PHBH_LOG(lastLine);
			PHBH_LOGE("Debug Build Failed");
		}
	}
	else {
		PHBH_LOGW("Debug build log not found: \"" + getRootPath() + "buildlog_Debug.txt\"");
	}

	QFile relFile(getRootPath() + "buildlog_Release.txt");
	if (relFile.open(QFile::ReadOnly | QFile::Text)) {
		QTextStream in(&relFile);
		QString lastLine;
		while (!in.atEnd()) {
			QString line = in.readLine();
			if (!line.isEmpty()) {
				lastLine = line;
			}
		}
		relFile.close();

		if (lastLine.contains("Build successful", Qt::CaseInsensitive)) {
			PHBH_LOG("Release Build Successful");
		}
		else {
			PHBH_LOG(lastLine);
			PHBH_LOGE("Release Build Failed");
		}
	}
	else {
		PHBH_LOGW("Release build log not found: \"" + getRootPath() + "buildlog_Release.txt\"");
	}
}

ProjectHelperBatchHelperItem* ProjectHelperBatchHelperProjectItem::createFavItemImpl(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent) {
	return createFromPath(_tree, _parent, getRootPath(), m_createFlags, this);
}

ProjectHelperBatchHelperProjectItem::ProjectHelperBatchHelperProjectItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _name, const QString& _rootPath)
	: ProjectHelperBatchHelperItem(_tree, _parent, _name, _rootPath)
{
}

ot::PushButton* ProjectHelperBatchHelperProjectItem::createScriptButton(ot::TreeWidget* _tree, ProjectHelperBatchHelperProjectItem* _item, int _column, const QString& _text, const QString& _path) {
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
