// @otlicense

// OpenTwin header
#include "OTWidgets/BlockAndFilter/UserInputSignalBlocker.h"

// Qt header
#include <QtGui/qevent.h>

ot::UserInputSignalBlocker::UserInputSignalBlocker(QObject* _parent) 
    : QObject(_parent)
{}

bool ot::UserInputSignalBlocker::eventFilter(QObject* _watched, QEvent* _event)
{
    if (isWhitelisted(_watched))
    {
		return QObject::eventFilter(_watched, _event);
    }
    else
    {
        return isUserInputEvent(_event);
    }
}

bool ot::UserInputSignalBlocker::isUserInputEvent(QEvent* _event)
{
	auto type = _event->type();
    switch (type)
    {
        // Mouse
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
    case QEvent::Wheel:

        // Keyboard and shortcuts
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::Shortcut:
    case QEvent::ShortcutOverride:

        // Context menus and help
    case QEvent::ContextMenu:
    case QEvent::ToolTip:
    case QEvent::WhatsThis:

        // Drag and drop
    case QEvent::DragEnter:
    case QEvent::DragMove:
    case QEvent::DragLeave:
    case QEvent::Drop:

        // Touch
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    case QEvent::TouchCancel:

        // Tablet / pen
    case QEvent::TabletPress:
    case QEvent::TabletMove:
    case QEvent::TabletRelease:

        // Gestures
    case QEvent::Gesture:
    case QEvent::GestureOverride:
    case QEvent::NativeGesture:

        // Input methods
    case QEvent::InputMethod:

        // Scrolling
    case QEvent::Scroll:
    case QEvent::ScrollPrepare:

        // Graphics scene mouse events
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneContextMenu:

        // Graphics scene drag and drop
    case QEvent::GraphicsSceneDragEnter:
    case QEvent::GraphicsSceneDragMove:
    case QEvent::GraphicsSceneDragLeave:
    case QEvent::GraphicsSceneDrop:
        return true;

    default:
        return false;
    }
}
