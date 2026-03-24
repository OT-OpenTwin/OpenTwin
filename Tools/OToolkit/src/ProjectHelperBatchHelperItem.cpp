// @otlicense

// OpenTwin header
#include "ProjectHelperBatchHelperItem.h"
#include "OTWidgets/Style/IconManager.h"
#include "OTWidgets/Widgets/TreeWidget.h"
#include "OTWidgets/Widgets/ToolButton.h"
#include "OToolkitAPI/OToolkitAPI.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtCore/qfileinfo.h>
#include <QtGui/qdesktopservices.h>

#define PHBH_LOG(___message) OTOOLKIT_LOG("BatchHelper", ___message)
#define PHBH_LOGW(___message) OTOOLKIT_LOGW("BatchHelper", ___message)
#define PHBH_LOGE(___message) OTOOLKIT_LOGE("BatchHelper", ___message)

ProjectHelperBatchHelperItem::ProjectHelperBatchHelperItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent, const QString& _name, const QString& _rootPath)
    : m_name(_name), m_fullPath(_rootPath)
{
    using namespace ot;

	_parent->addChild(this);

    setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    setCheckState(0, Qt::Unchecked);

	m_fullPath.replace('\\', '/');

	setText(static_cast<int>(BaseColumns::Name), m_name);

	m_rootPath = normalizePath(_rootPath);

    ToolButton* revealBtn = new ToolButton(nullptr);
    revealBtn->setFixedSize(24, 24);
    revealBtn->setIcon(IconManager::getIcon("ContextMenu/Folder.png"));
	revealBtn->setToolTip("Reveal in File Explorer" + (m_fullPath.isEmpty() ? QString() : ":\n" + m_fullPath));
    revealBtn->setEnabled(!m_rootPath.isEmpty());
    connect(revealBtn, &ToolButton::clicked, this, &ProjectHelperBatchHelperItem::slotRevealInExplorer);
    _tree->setItemWidget(this, static_cast<int>(BaseColumns::RevealInExplorer), revealBtn);
}

ProjectHelperBatchHelperItem* ProjectHelperBatchHelperItem::createFavItem(ot::TreeWidget* _tree, ot::TreeWidgetItem* _parent) {
	ProjectHelperBatchHelperItem* itm = createFavItemImpl(_tree, _parent);
	if (itm) {
		itm->m_rootPath = m_rootPath;
		itm->m_fullPath = m_fullPath;
		itm->m_name = m_name;
		itm->setOriginalItemPath(getTreeWidgetItemPath());
		itm->setFlags(itm->flags() & (~(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable)));
	}
	return itm;
}

void ProjectHelperBatchHelperItem::slotRevealInExplorer() {
    const QString pathToReveal = getRootPath();
    if (QDir(pathToReveal).exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(pathToReveal));
    }
    else {
        PHBH_LOGW("Path to reveal does not exist: \"" + pathToReveal + "\"");
    }
}

QString ProjectHelperBatchHelperItem::normalizePath(const QString& _path)
{
    QFileInfo info(_path);

    if (info.exists())
    {
        if (info.isDir())
        {
            return QDir::cleanPath(info.absoluteFilePath());
        }
        else
        {
            // It's a file -> return its containing directory
            return QDir::cleanPath(info.absolutePath());
        }
    }
    else
    {
        // Path may not exist -> fallback heuristic
        // If it has a suffix, assume it's a file path
        if (!info.suffix().isEmpty())
        {
            return QDir::cleanPath(info.path());
        }

        // Otherwise assume it's already a directory
        return QDir::cleanPath(_path);
    }
}
