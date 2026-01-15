// @otlicense

// OpenTwin header
#include "ProjectHelperBatchHelper.h"
#include "ProjectHelperBatchHelperItem.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/TreeWidgetItem.h"
#include "OTWidgets/TreeWidgetFilter.h"

// Qt header
#include <QtCore/qdiriterator.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qheaderview.h>

using namespace ot;

ProjectHelperBatchHelper::ProjectHelperBatchHelper() : m_rootWidget(nullptr) {
	m_rootWidget = new QWidget;
	QVBoxLayout* rootLayout = new QVBoxLayout(m_rootWidget);

	QHBoxLayout* topLayout = new QHBoxLayout;
	rootLayout->addLayout(topLayout);

	topLayout->addWidget(new Label("Root path:", m_rootWidget));
	m_rootPathEdit = new LineEdit(m_rootWidget);
	topLayout->addWidget(m_rootPathEdit, 1);

	PushButton* refreshButton = new PushButton("Refresh", m_rootWidget);
	connect(refreshButton, &PushButton::clicked, this, &ProjectHelperBatchHelper::slotRefreshData);
	topLayout->addWidget(refreshButton);

	m_tree = new TreeWidgetFilter(m_rootWidget);
	m_tree->getTreeWidget()->setColumnCount(static_cast<int>(ProjectHelperBatchHelperItem::Columns::ColumnCount) + 1);
	QStringList headerLabels;
	headerLabels.push_back("Name");
	for (int i = 1; i <= static_cast<int>(ProjectHelperBatchHelperItem::Columns::ColumnCount); i++) {
		headerLabels.push_back("");
	}
	m_tree->getTreeWidget()->setHeaderLabels(headerLabels);
	m_tree->getTreeWidget()->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_tree->getTreeWidget()->header()->setSectionResizeMode(static_cast<int>(ProjectHelperBatchHelperItem::Columns::ColumnCount), QHeaderView::Stretch);
	rootLayout->addWidget(m_tree->getQWidget(), 1);
}

ProjectHelperBatchHelper::~ProjectHelperBatchHelper() {

}

void ProjectHelperBatchHelper::setRootPath(const QString& _path) {
	m_rootPathEdit->setText(_path);
}

QString ProjectHelperBatchHelper::getRootPath() const {
	return m_rootPathEdit->text();
}

void ProjectHelperBatchHelper::refreshDir(const QString& _rootPath, const QString& _childName) {
	TreeWidgetItem* rootItm = new TreeWidgetItem;
	rootItm->setText(0, _childName);
	m_tree->getTreeWidget()->addTopLevelItem(rootItm);

	QDirIterator dirIt(_rootPath + _childName, QDir::Dirs | QDir::NoDotAndDotDot);
	while (dirIt.hasNext()) {
		dirIt.next();
		QString pth = dirIt.filePath();
		pth.replace('\\', '/');
		if (!pth.endsWith('/')) {
			pth.append('/');
		}
		ProjectHelperBatchHelperItem::createFromPath(m_tree->getTreeWidget(), rootItm, pth);
	}

	rootItm->setExpanded(true);
}

void ProjectHelperBatchHelper::slotRefreshData() {
	QString rootPath = m_rootPathEdit->text();
	if (rootPath.isEmpty()) {
		return;
	}
	rootPath.replace('\\', '/');
	if (!rootPath.endsWith('/')) {
		rootPath.append('/');
	}

	TreeWidget* tree = m_tree->getTreeWidget();

	tree->clear();
	refreshDir(rootPath, "Libraries");
	refreshDir(rootPath, "Services");
	refreshDir(rootPath, "Tools");
}