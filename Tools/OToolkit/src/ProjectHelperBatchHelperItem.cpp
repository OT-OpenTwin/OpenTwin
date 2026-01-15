// @otlicense

// OpenTwin header
#include "ProjectHelperBatchHelperItem.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/TreeWidget.h"

// Qt header
#include <QtCore/qdiriterator.h>

using namespace ot;

void ProjectHelperBatchHelperItem::createFromPath(TreeWidget* _tree, TreeWidgetItem* _parent, const QString& _path) {
	// Check if project file exists in the given path
	QString projPath;
	QString buildPath;
	QString clearPath;
	QString editPath;

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
		else if (dirIt.fileName().toLower() == "clear.bat") {
			clearPath = dirIt.filePath();
		}
		else if (dirIt.fileName().toLower() == "edit.bat") {
			editPath = dirIt.filePath();
		}
	}

	if (projPath.isEmpty() || (buildPath.isEmpty() && clearPath.isEmpty() && editPath.isEmpty())) {
		return;
	}

	QStringList pathParts = _path.split('/', Qt::SkipEmptyParts);
	OTAssert(pathParts.size() > 0, "Invalid path provided");
	ProjectHelperBatchHelperItem* item = new ProjectHelperBatchHelperItem;
	item->m_rootPath = _path;
	item->m_buildPath = buildPath;
	item->m_clearPath = clearPath;
	item->m_editPath = editPath;

	item->setText(static_cast<int>(Columns::Name), pathParts.last());
	_parent->addChild(item);

	PushButton* editButton = new PushButton("Edit", nullptr);
	editButton->setEnabled(!editPath.isEmpty());
	item->connect(editButton, &PushButton::clicked, item, &ProjectHelperBatchHelperItem::slotEdit);
	_tree->setItemWidget(item, static_cast<int>(Columns::Edit), editButton);

	PushButton* buildButton = new PushButton("Build", nullptr);
	buildButton->setEnabled(!buildPath.isEmpty());
	item->connect(buildButton, &PushButton::clicked, item, &ProjectHelperBatchHelperItem::slotBuild);
	_tree->setItemWidget(item, static_cast<int>(Columns::Build), buildButton);

	PushButton* rebuildButton = new PushButton("Rebuild", nullptr);
	rebuildButton->setEnabled(!buildPath.isEmpty());
	item->connect(rebuildButton, &PushButton::clicked, item, &ProjectHelperBatchHelperItem::slotRebuild);
	_tree->setItemWidget(item, static_cast<int>(Columns::Rebuild), rebuildButton);

	PushButton* clearButton = new PushButton("Clear", nullptr);
	clearButton->setEnabled(!clearPath.isEmpty());
	item->connect(clearButton, &PushButton::clicked, item, &ProjectHelperBatchHelperItem::slotClear);
	_tree->setItemWidget(item, static_cast<int>(Columns::Clear), clearButton);
}

ProjectHelperBatchHelperItem::ProjectHelperBatchHelperItem() {

}

ProjectHelperBatchHelperItem::~ProjectHelperBatchHelperItem() {
}

void ProjectHelperBatchHelperItem::slotBuild() {

}

void ProjectHelperBatchHelperItem::slotRebuild() {

}

void ProjectHelperBatchHelperItem::slotClear() {

}

void ProjectHelperBatchHelperItem::slotEdit() {
}
