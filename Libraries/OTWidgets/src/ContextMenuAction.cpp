// @otlicense

// OpenTwin header
#include "OTWidgets/IconManager.h"
#include "OTWidgets/ContextMenuAction.h"

ot::ContextMenuAction::ContextMenuAction(QObject* _parent)
    : QAction(_parent)
{

}

ot::ContextMenuAction::ContextMenuAction(const MenuButtonCfg& _config, QObject* _parent)
    : QAction(_parent)
{
	this->setFromConfiguration(_config);
}

ot::ContextMenuAction::~ContextMenuAction() {
    
}

void ot::ContextMenuAction::setFromConfiguration(const MenuButtonCfg& _config) {
    m_name = _config.getName();
    this->setText(QString::fromStdString(_config.getText()));
    this->setToolTip(QString::fromStdString(_config.getToolTip()));

    // Set icon if needed
    if (_config.getIconPath().empty()) {
        this->setIcon(QIcon());
    }
    else {
        this->setIcon(IconManager::getIcon(QString::fromStdString(_config.getIconPath())));
    }
}
