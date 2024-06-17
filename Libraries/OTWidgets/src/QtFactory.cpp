//! @file Painter2DFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/CheckerboardPainter2D.h"
#include "OTGui/LinearGradientPainter2D.h"
#include "OTGui/RadialGradientPainter2D.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtGui/qpixmap.h>
#include <QtGui/qpainter.h>

QFont ot::QtFactory::toQFont(const Font& _f) {
    QFont font;
    font.setPixelSize(_f.size());
    font.setBold(_f.isBold());
    font.setItalic(_f.isItalic());
    font.setFamily(QString::fromStdString(_f.family()));

    return font;
}

ot::Font ot::QtFactory::toFont(const QFont& _font) {
    Font font;
    font.setSize(_font.pixelSize());
    font.setBold(_font.bold());
    font.setItalic(_font.italic());
    font.setFamily(_font.family().toStdString());

    return font;
}

QPen ot::QtFactory::toQPen(const Outline& _outline) {
    if (_outline.width() <= 0) return QPen(Qt::NoPen);
    else return QPen(toQBrush(_outline.painter()), (qreal)_outline.width(), toQPenStyle(_outline.style()), toQPenCapStyle(_outline.cap()), toQPenJoinStyle(_outline.joinStyle())); 
}

QPen ot::QtFactory::toQPen(const OutlineF& _outline) {
    if (_outline.width() <= 0.) return QPen(Qt::NoPen);
    else return QPen(toQBrush(_outline.painter()), _outline.width(), toQPenStyle(_outline.style()), toQPenCapStyle(_outline.cap()), toQPenJoinStyle(_outline.joinStyle())); 
}

Qt::Alignment ot::QtFactory::toQAlignment(ot::Alignment _alignment) {
	switch (_alignment)
	{
	case ot::AlignCenter: return Qt::AlignCenter;
	case ot::AlignTop: return Qt::AlignTop | Qt::AlignHCenter;
	case ot::AlignTopRight: return Qt::AlignTop | Qt::AlignRight;
	case ot::AlignRight: return Qt::AlignRight | Qt::AlignVCenter;
	case ot::AlignBottomRight: return Qt::AlignBottom | Qt::AlignRight;
	case ot::AlignBottom: return Qt::AlignBottom | Qt::AlignHCenter;
	case ot::AlignBottomLeft: return Qt::AlignBottom | Qt::AlignLeft;
	case ot::AlignLeft: return Qt::AlignLeft | Qt::AlignVCenter;
	case ot::AlignTopLeft: return Qt::AlignTop | Qt::AlignLeft;
	default:
		OT_LOG_EA("Unknown alignment");
		return Qt::AlignCenter;
	}
}

QGradient::Spread ot::QtFactory::toQGradientSpread(ot::GradientSpread _spread) {
	switch (_spread)
	{
	case ot::PadSpread: return QGradient::PadSpread;
	case ot::RepeatSpread: return QGradient::RepeatSpread;
	case ot::ReflectSpread: return QGradient::ReflectSpread;
	default:
		OT_LOG_EA("Unknown gradient spread");
		return QGradient::PadSpread;
	}
}

QBrush ot::QtFactory::toQBrush(const ot::Painter2D* _painter) {
	if (!_painter) {
		OT_LOG_W("Painter is 0. Ignoring");
		return QBrush();
	}
	
	if (_painter->getFactoryKey() == OT_FactoryKey_FillPainter2D) {
		const FillPainter2D* painter = dynamic_cast<const FillPainter2D*>(_painter);
		OTAssertNullptr(painter);
		return QBrush(QtFactory::toQColor(painter->getColor()));
	}
	else if (_painter->getFactoryKey() == OT_FactoryKey_LinearGradientPainter2D) {
		const LinearGradientPainter2D* painter = dynamic_cast<const LinearGradientPainter2D*>(_painter);
		OTAssertNullptr(painter);
		
		QGradientStops stops;
		for (auto s : painter->getStops()) {
			stops.append(QGradientStop(s.getPos(), QtFactory::toQColor(s.getColor())));
		}

		QLinearGradient grad(QtFactory::toQPoint(painter->getStart()), QtFactory::toQPoint(painter->getFinalStop()));
		grad.setSpread(QtFactory::toQGradientSpread(painter->getSpread()));
		grad.setCoordinateMode(QGradient::ObjectBoundingMode);
		grad.setStops(stops);
		
		return grad;
	}
	else if (_painter->getFactoryKey() == OT_FactoryKey_RadialGradientPainter2D) {
		const RadialGradientPainter2D* painter = dynamic_cast<const RadialGradientPainter2D*>(_painter);
		OTAssertNullptr(painter);

		QGradientStops stops;
		for (auto s : painter->getStops()) {
			stops.append(QGradientStop(s.getPos(), ot::QtFactory::toQColor(s.getColor())));
		}

		QRadialGradient grad(ot::QtFactory::toQPoint(painter->getCenterPoint()), painter->getCenterRadius());
		if (painter->isFocalPointSet()) {
			grad.setFocalPoint(ot::QtFactory::toQPoint(painter->getFocalPoint()));
			grad.setFocalRadius(painter->getFocalRadius());
		}
		grad.setSpread(ot::QtFactory::toQGradientSpread(painter->getSpread()));
		grad.setCoordinateMode(QGradient::ObjectBoundingMode);
		grad.setStops(stops);

		return grad;
	}
	else if (_painter->getFactoryKey() == OT_FactoryKey_StyleRefPainter2D) {
		const StyleRefPainter2D* painter = dynamic_cast<const StyleRefPainter2D*>(_painter);
		OTAssertNullptr(painter);

		const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();

		if (!cs.hasValue(painter->getReferenceKey())) {
			OT_LOG_W("Failed to create brush from ColorStyleValue reference. Value not found");
			return QBrush();
		}
		else {
			return cs.getValue(painter->getReferenceKey()).brush();
		}
	}
    else if (_painter->getFactoryKey() == OT_FactoryKey_CheckerboardPainter2D) {
        const CheckerboardPainter2D* painter = dynamic_cast<const CheckerboardPainter2D*>(_painter);
        OTAssertNullptr(painter);
        OTAssert(painter->getCellSize().width() > 0, "Invalid cell width");
        OTAssert(painter->getCellSize().height() > 0, "Invalid cell height");

        QBrush primaryBrush = QtFactory::toQBrush(painter->getPrimaryPainter());
        QBrush secondaryBrush = QtFactory::toQBrush(painter->getSecondaryPainter());

        QPixmap checkerboard(painter->getCellSize().width() * 2, painter->getCellSize().height() * 2);
        checkerboard.fill(Qt::transparent);

        QPainter pixmapPainter(&checkerboard);
        
        pixmapPainter.setBrush(primaryBrush);
        pixmapPainter.setPen(Qt::NoPen);
        pixmapPainter.drawRect(0, 0, painter->getCellSize().width(), painter->getCellSize().height()); // TopLeft
        pixmapPainter.drawRect(painter->getCellSize().width(), painter->getCellSize().height(), painter->getCellSize().width(), painter->getCellSize().height()); // BottomRight

        pixmapPainter.setBrush(secondaryBrush);
        pixmapPainter.drawRect(painter->getCellSize().width(), 0, painter->getCellSize().width(), painter->getCellSize().height()); // TopRight
        pixmapPainter.drawRect(0, painter->getCellSize().height(), painter->getCellSize().width(), painter->getCellSize().height()); // BottomLeft
        pixmapPainter.end();

        return QBrush(checkerboard);
    }
	else {
		OT_LOG_EAS("Unknown Painter2D provided \"" + _painter->getFactoryKey() + "\"");
		return QBrush();
	}
}

Qt::PenStyle ot::QtFactory::toQPenStyle(LineStyle _style) {
	switch (_style)
	{
	case ot::NoLine: return Qt::NoPen;
	case ot::SolidLine: return Qt::SolidLine;
	case ot::DashLine: return Qt::DashLine;
	case ot::DotLine: return Qt::DotLine;
	case ot::DashDotLine: return Qt::DashDotLine;
	case ot::DashDotDotLine: return Qt::DashDotDotLine;
	default:
		OT_LOG_E("Unknown line style (" + std::to_string((int)_style) + ")");
		return Qt::NoPen;
	}
}

Qt::PenCapStyle ot::QtFactory::toQPenCapStyle(LineCapStyle _style) {
	switch (_style)
	{
	case ot::FlatCap: return Qt::FlatCap;
	case ot::SquareCap: return Qt::SquareCap;
	case ot::RoundCap: return Qt::RoundCap;
	default:
		OT_LOG_E("Unknown cap style (" + std::to_string((int)_style) + ")");
		return Qt::SquareCap;
	}
}

Qt::PenJoinStyle ot::QtFactory::toQPenJoinStyle(LineJoinStyle _style) {
	switch (_style)
	{
	case ot::MiterJoin: return Qt::MiterJoin;
	case ot::BevelJoin: return Qt::BevelJoin;
	case ot::RoundJoin: return Qt::RoundJoin;
	case ot::SvgMiterJoin: return Qt::SvgMiterJoin;
	default:
		OT_LOG_E("Unknown join style (" + std::to_string((int)_style) + ")");
		return Qt::BevelJoin;
	}
}

QPainterPath ot::QtFactory::toQPainterPath(const Path2DF& _path) {
	QPainterPath p;
	for (const Path2DF::PathEntry& e : _path.getEntries()) {
		p.moveTo(QtFactory::toQPoint(e.start));

		switch (e.type)
		{
		case Path2D::LineType:
			p.lineTo(QtFactory::toQPoint(e.stop));
			break;

		case Path2D::BerzierType:
			p.cubicTo(QtFactory::toQPoint(e.control1), QtFactory::toQPoint(e.control1), QtFactory::toQPoint(e.control1));
			break;

		default:
			OT_LOG_E("Unknown path2D entry type");
			break;
		}
	}
	return p;
}

QString ot::QtFactory::toQString(QEvent::Type _type) {
    static const QMap<QEvent::Type, QString> eventTypeMap = {
        { QEvent::None, "None" },
        { QEvent::Timer, "Timer" },
        { QEvent::MouseButtonPress, "MouseButtonPress" },
        { QEvent::MouseButtonRelease, "MouseButtonRelease" },
        { QEvent::MouseButtonDblClick, "MouseButtonDblClick" },
        { QEvent::MouseMove, "MouseMove" },
        { QEvent::KeyPress, "KeyPress" },
        { QEvent::KeyRelease, "KeyRelease" },
        { QEvent::FocusIn, "FocusIn" },
        { QEvent::FocusOut, "FocusOut" },
        { QEvent::FocusAboutToChange, "FocusAboutToChange" },
        { QEvent::Enter, "Enter" },
        { QEvent::Leave, "Leave" },
        { QEvent::Paint, "Paint" },
        { QEvent::Move, "Move" },
        { QEvent::Resize, "Resize" },
        { QEvent::Close, "Close" },
        { QEvent::Quit, "Quit" },
        { QEvent::Show, "Show" },
        { QEvent::Hide, "Hide" },
        { QEvent::Destroy, "Destroy" },
        { QEvent::ShowToParent, "ShowToParent" },
        { QEvent::HideToParent, "HideToParent" },
        { QEvent::Wheel, "Wheel" },
        { QEvent::WindowActivate, "WindowActivate" },
        { QEvent::WindowDeactivate, "WindowDeactivate" },
        { QEvent::WindowStateChange, "WindowStateChange" },
        { QEvent::WindowTitleChange, "WindowTitleChange" },
        { QEvent::WindowIconChange, "WindowIconChange" },
        { QEvent::ApplicationWindowIconChange, "ApplicationWindowIconChange" },
        { QEvent::ApplicationFontChange, "ApplicationFontChange" },
        { QEvent::ApplicationLayoutDirectionChange, "ApplicationLayoutDirectionChange" },
        { QEvent::ApplicationPaletteChange, "ApplicationPaletteChange" },
        { QEvent::Clipboard, "Clipboard" },
        { QEvent::Speech, "Speech" },
        { QEvent::MetaCall, "MetaCall" },
        { QEvent::SockAct, "SockAct" },
        { QEvent::ShortcutOverride, "ShortcutOverride" },
        { QEvent::DeferredDelete, "DeferredDelete" },
        { QEvent::DragEnter, "DragEnter" },
        { QEvent::DragMove, "DragMove" },
        { QEvent::DragLeave, "DragLeave" },
        { QEvent::Drop, "Drop" },
        { QEvent::DragResponse, "DragResponse" },
        { QEvent::ChildAdded, "ChildAdded" },
        { QEvent::ChildPolished, "ChildPolished" },
        { QEvent::ChildRemoved, "ChildRemoved" },
        { QEvent::ShowWindowRequest, "ShowWindowRequest" },
        { QEvent::PolishRequest, "PolishRequest" },
        { QEvent::Polish, "Polish" },
        { QEvent::LayoutRequest, "LayoutRequest" },
        { QEvent::UpdateRequest, "UpdateRequest" },
        { QEvent::UpdateLater, "UpdateLater" },
        { QEvent::EmbeddingControl, "EmbeddingControl" },
        { QEvent::ActivateControl, "ActivateControl" },
        { QEvent::DeactivateControl, "DeactivateControl" },
        { QEvent::ContextMenu, "ContextMenu" },
        { QEvent::InputMethod, "InputMethod" },
        { QEvent::TabletMove, "TabletMove" },
        { QEvent::LocaleChange, "LocaleChange" },
        { QEvent::LanguageChange, "LanguageChange" },
        { QEvent::LayoutDirectionChange, "LayoutDirectionChange" },
        { QEvent::Style, "Style" },
        { QEvent::TabletPress, "TabletPress" },
        { QEvent::TabletRelease, "TabletRelease" },
        { QEvent::OkRequest, "OkRequest" },
        { QEvent::HelpRequest, "HelpRequest" },
        { QEvent::IconDrag, "IconDrag" },
        { QEvent::FontChange, "FontChange" },
        { QEvent::EnabledChange, "EnabledChange" },
        { QEvent::ActivationChange, "ActivationChange" },
        { QEvent::StyleChange, "StyleChange" },
        { QEvent::IconTextChange, "IconTextChange" },
        { QEvent::ModifiedChange, "ModifiedChange" },
        { QEvent::MouseTrackingChange, "MouseTrackingChange" },
        { QEvent::WindowBlocked, "WindowBlocked" },
        { QEvent::WindowUnblocked, "WindowUnblocked" },
        { QEvent::WindowStateChange, "WindowStateChange" },
        { QEvent::ReadOnlyChange, "ReadOnlyChange" },
        { QEvent::ToolTip, "ToolTip" },
        { QEvent::WhatsThis, "WhatsThis" },
        { QEvent::StatusTip, "StatusTip" },
        { QEvent::ActionChanged, "ActionChanged" },
        { QEvent::ActionAdded, "ActionAdded" },
        { QEvent::ActionRemoved, "ActionRemoved" },
        { QEvent::FileOpen, "FileOpen" },
        { QEvent::Shortcut, "Shortcut" },
        { QEvent::WhatsThisClicked, "WhatsThisClicked" },
        { QEvent::ToolBarChange, "ToolBarChange" },
        { QEvent::ApplicationActivate, "ApplicationActivate" },
        { QEvent::ApplicationDeactivate, "ApplicationDeactivate" },
        { QEvent::QueryWhatsThis, "QueryWhatsThis" },
        { QEvent::EnterWhatsThisMode, "EnterWhatsThisMode" },
        { QEvent::LeaveWhatsThisMode, "LeaveWhatsThisMode" },
        { QEvent::ZOrderChange, "ZOrderChange" },
        { QEvent::HoverEnter, "HoverEnter" },
        { QEvent::HoverLeave, "HoverLeave" },
        { QEvent::HoverMove, "HoverMove" },
        { QEvent::AcceptDropsChange, "AcceptDropsChange" },
        { QEvent::ZeroTimerEvent, "ZeroTimerEvent" },
        { QEvent::GraphicsSceneMouseMove, "GraphicsSceneMouseMove" },
        { QEvent::GraphicsSceneMousePress, "GraphicsSceneMousePress" },
        { QEvent::GraphicsSceneMouseRelease, "GraphicsSceneMouseRelease" },
        { QEvent::GraphicsSceneMouseDoubleClick, "GraphicsSceneMouseDoubleClick" },
        { QEvent::GraphicsSceneContextMenu, "GraphicsSceneContextMenu" },
        { QEvent::GraphicsSceneHoverEnter, "GraphicsSceneHoverEnter" },
        { QEvent::GraphicsSceneHoverMove, "GraphicsSceneHoverMove" },
        { QEvent::GraphicsSceneHoverLeave, "GraphicsSceneHoverLeave" },
        { QEvent::GraphicsSceneHelp, "GraphicsSceneHelp" },
        { QEvent::GraphicsSceneDragEnter, "GraphicsSceneDragEnter" },
        { QEvent::GraphicsSceneDragMove, "GraphicsSceneDragMove" },
        { QEvent::GraphicsSceneDragLeave, "GraphicsSceneDragLeave" },
        { QEvent::GraphicsSceneDrop, "GraphicsSceneDrop" },
        { QEvent::GraphicsSceneWheel, "GraphicsSceneWheel" },
        { QEvent::KeyboardLayoutChange, "KeyboardLayoutChange" },
        { QEvent::DynamicPropertyChange, "DynamicPropertyChange" },
        { QEvent::TabletEnterProximity, "TabletEnterProximity" },
        { QEvent::TabletLeaveProximity, "TabletLeaveProximity" },
        { QEvent::NonClientAreaMouseMove, "NonClientAreaMouseMove" },
        { QEvent::NonClientAreaMouseButtonPress, "NonClientAreaMouseButtonPress" },
        { QEvent::NonClientAreaMouseButtonRelease, "NonClientAreaMouseButtonRelease" },
        { QEvent::NonClientAreaMouseButtonDblClick, "NonClientAreaMouseButtonDblClick" },
        { QEvent::MacSizeChange, "MacSizeChange" },
        { QEvent::ContentsRectChange, "ContentsRectChange" },
        { QEvent::MacGLWindowChange, "MacGLWindowChange" },
        { QEvent::FutureCallOut, "FutureCallOut" },
        { QEvent::GraphicsSceneResize, "GraphicsSceneResize" },
        { QEvent::GraphicsSceneMove, "GraphicsSceneMove" },
        { QEvent::CursorChange, "CursorChange" },
        { QEvent::ToolTipChange, "ToolTipChange" },
        { QEvent::NetworkReplyUpdated, "NetworkReplyUpdated" },
        { QEvent::GrabMouse, "GrabMouse" },
        { QEvent::UngrabMouse, "UngrabMouse" },
        { QEvent::GrabKeyboard, "GrabKeyboard" },
        { QEvent::UngrabKeyboard, "UngrabKeyboard" },
        { QEvent::StateMachineSignal, "StateMachineSignal" },
        { QEvent::StateMachineWrapped, "StateMachineWrapped" },
        { QEvent::TouchBegin, "TouchBegin" },
        { QEvent::TouchUpdate, "TouchUpdate" },
        { QEvent::TouchEnd, "TouchEnd" },
        { QEvent::NativeGesture, "NativeGesture" },
        { QEvent::RequestSoftwareInputPanel, "RequestSoftwareInputPanel" },
        { QEvent::CloseSoftwareInputPanel, "CloseSoftwareInputPanel" },
        { QEvent::WinIdChange, "WinIdChange" },
        { QEvent::Gesture, "Gesture" },
        { QEvent::GestureOverride, "GestureOverride" },
        { QEvent::ScrollPrepare, "ScrollPrepare" },
        { QEvent::Scroll, "Scroll" },
        { QEvent::Expose, "Expose" },
        { QEvent::InputMethodQuery, "InputMethodQuery" },
        { QEvent::OrientationChange, "OrientationChange" },
        { QEvent::TouchCancel, "TouchCancel" },
        { QEvent::ThemeChange, "ThemeChange" },
        { QEvent::SockClose, "SockClose" },
        { QEvent::PlatformPanel, "PlatformPanel" },
        { QEvent::StyleAnimationUpdate, "StyleAnimationUpdate" },
        { QEvent::ApplicationStateChange, "ApplicationStateChange" },
        { QEvent::WindowChangeInternal, "WindowChangeInternal" },
        { QEvent::ScreenChangeInternal, "ScreenChangeInternal" },
        { QEvent::PlatformSurface, "PlatformSurface" },
        { QEvent::Pointer, "Pointer" },
        { QEvent::TabletTrackingChange, "TabletTrackingChange" },
        { QEvent::User, "User" },
        { QEvent::MaxUser, "MaxUser" }
    };

    return eventTypeMap.value(_type, "Unknown");
}