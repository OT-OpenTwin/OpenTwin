// @otlicense

// OpenTwin header
#include "AppBase.h"
#include "ScriptRunner.h"
#include "OToolkitAPI/OToolkitAPI.h"
#include "ProjectHelperBatchHelperItem.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/TreeWidget.h"

// Qt header
#include <QtCore/qdiriterator.h>
#include <QtWidgets/qlayout.h>

#define PHBH_LOG(___message) OTOOLKIT_LOG("BatchHelper", ___message)
#define PHBH_LOGW(___message) OTOOLKIT_LOGW("BatchHelper", ___message)
#define PHBH_LOGE(___message) OTOOLKIT_LOGE("BatchHelper", ___message)

using namespace ot;

void ProjectHelperBatchHelperItem::createFromPath(TreeWidget* _tree, TreeWidgetItem* _parent, const QString& _path, const CreateFlags& _flags) {
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
		return;
	}

	QStringList pathParts = _path.split('/', Qt::SkipEmptyParts);
	OTAssert(pathParts.size() > 0, "Invalid path provided");
	ProjectHelperBatchHelperItem* item = new ProjectHelperBatchHelperItem;
	item->m_rootPath = _path;
	item->m_buildPath = buildPath;
	item->m_cleanPath = cleanPath;
	item->m_editPath = editPath;
	item->m_testPath = testPath;

	QString nameTxt = pathParts.last();
	if (_flags.has(CreateFlag::RemoveOTPrefix)) {
		if (nameTxt.startsWith("OT", Qt::CaseSensitive)) {
			nameTxt = nameTxt.mid(2);
		}
	}

	item->setText(static_cast<int>(Columns::Name), nameTxt);
	_parent->addChild(item);

	item->connect(createScriptButton(_tree, item, static_cast<int>(Columns::Edit), "Edit", editPath), &PushButton::clicked, item, &ProjectHelperBatchHelperItem::slotEdit);
	item->connect(createScriptButton(_tree, item, static_cast<int>(Columns::Build), "Build", buildPath), &PushButton::clicked, item, &ProjectHelperBatchHelperItem::slotBuild);
	item->connect(createScriptButton(_tree, item, static_cast<int>(Columns::Rebuild), "Rebuild", buildPath), &PushButton::clicked, item, &ProjectHelperBatchHelperItem::slotRebuild);
	item->connect(createScriptButton(_tree, item, static_cast<int>(Columns::Test), "Test", testPath), &PushButton::clicked, item, &ProjectHelperBatchHelperItem::slotTest);
	item->connect(createScriptButton(_tree, item, static_cast<int>(Columns::Clean), "Clean", cleanPath), &PushButton::clicked, item, &ProjectHelperBatchHelperItem::slotClean);
}

ProjectHelperBatchHelperItem::ProjectHelperBatchHelperItem() {
	setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
	setCheckState(0, Qt::Unchecked);
}

ProjectHelperBatchHelperItem::~ProjectHelperBatchHelperItem() {

}

QString ProjectHelperBatchHelperItem::getScriptName() const {
	QString pth = m_rootPath;
	pth.replace('\\', '/');
	QStringList pathParts = pth.split('/', Qt::SkipEmptyParts);
	if (pathParts.isEmpty()) {
		return QString();
	}
	QStringList nameParts = pathParts.last().split('.', Qt::SkipEmptyParts);
	if (nameParts.isEmpty()) {
		return QString();
	}
	return nameParts.first();
}

void ProjectHelperBatchHelperItem::slotBuild() {
	ScriptRunner* runner = ScriptRunner::runScript(getScriptName() + " - Build", m_buildPath, { "BOTH", "BUILD" }, m_rootPath);
	if (runner) {
		connect(runner, &ScriptRunner::finished, this, &ProjectHelperBatchHelperItem::slotBuildFinished);
		PHBH_LOG("Build batch executed successfully: \"" + m_buildPath + "\"");
	}
	else {
		PHBH_LOGE("Build batch execution failed: \"" + m_buildPath + "\"");
	}
}

void ProjectHelperBatchHelperItem::slotRebuild() {
	ScriptRunner* runner = ScriptRunner::runScript(getScriptName() + " - Rebuild", m_buildPath, { "BOTH", "REBUILD" }, m_rootPath);
	if (runner) {
		connect(runner, &ScriptRunner::finished, this, &ProjectHelperBatchHelperItem::slotBuildFinished);
		PHBH_LOG("Rebuild batch executed successfully: \"" + m_buildPath + "\"");
	}
	else {
		PHBH_LOGE("Rebuild batch execution failed: \"" + m_buildPath + "\"");
	}
}

void ProjectHelperBatchHelperItem::slotClean() {
	if (ScriptRunner::runScript(getScriptName() + " - Clean", m_cleanPath, {}, m_rootPath)) {
		PHBH_LOG("Clear batch executed successfully: \"" + m_cleanPath + "\"");
	}
	else {
		PHBH_LOGE("Clear batch execution failed: \"" + m_cleanPath + "\"");
	}
}

void ProjectHelperBatchHelperItem::slotEdit() {
	if (ScriptRunner::runDetached(getScriptName() + " - Edit", m_editPath, {}, m_rootPath)) {
		PHBH_LOG("Edit batch executed successfully: \"" + m_editPath + "\"");
	}
	else {
		PHBH_LOGE("Edit batch execution failed: \"" + m_editPath + "\"");
	}
}

void ProjectHelperBatchHelperItem::slotTest() {
	if (ScriptRunner::runScript(getScriptName() + " - Test", m_testPath, { "--output-on-failure" }, m_rootPath)) {
		PHBH_LOG("Test batch executed successfully: \"" + m_testPath + "\"");
	}
	else {
		PHBH_LOGE("Test batch execution failed: \"" + m_testPath + "\"");
	}
}

void ProjectHelperBatchHelperItem::slotBuildFinished() {
	QFile dbgFile(m_rootPath + "buildlog_Debug.txt");
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
		PHBH_LOGW("Debug build log not found: \"" + m_rootPath + "buildlog_Debug.txt\"");
	}

	QFile relFile(m_rootPath + "buildlog_Release.txt");
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
		PHBH_LOGW("Release build log not found: \"" + m_rootPath + "buildlog_Release.txt\"");
	}
}

ot::PushButton* ProjectHelperBatchHelperItem::createScriptButton(ot::TreeWidget* _tree, ProjectHelperBatchHelperItem* _item, int _column, const QString& _text, const QString& _path) {
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
