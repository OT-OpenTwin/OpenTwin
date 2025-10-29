// @otlicense

// AK header
#include <akGui/aAction.h>

ak::aAction::aAction(QToolButton::ToolButtonPopupMode _popupMode, QObject * _parent)
	: QAction(_parent), m_popupMode(_popupMode), aObject(otAction) {}

ak::aAction::aAction(const QString & _text, QToolButton::ToolButtonPopupMode _popupMode, QObject * _parent)
	: QAction(_text, _parent), m_popupMode(_popupMode), aObject(otAction) {}

ak::aAction::aAction(const QIcon & _icon, const QString & _text, QToolButton::ToolButtonPopupMode _popupMode, QObject * _parent)
	: QAction(_icon, _text, _parent), m_popupMode(_popupMode), aObject(otAction) {}

ak::aAction::~aAction() { A_OBJECT_DESTROYING }
