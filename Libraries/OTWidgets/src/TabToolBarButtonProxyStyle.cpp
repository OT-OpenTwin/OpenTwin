// @otlicense

// OpenTwin header
#include "OTWidgets/TabToolBarButtonProxyStyle.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtWidgets/qstyleoption.h>

void ot::TabToolBarButtonProxyStyle::drawControl(ControlElement _element, const QStyleOption* _opt, QPainter* _painter, const QWidget* _widget) const {
    if (_element == CE_ToolButtonLabel) {
        if (const QStyleOptionToolButton* toolbutton = static_cast<const QStyleOptionToolButton*>(_opt)) {
            QRect rect = toolbutton->rect;
            int shiftX = 0;
            int shiftY = 0;
            if (toolbutton->state & (State_Sunken | State_On)) {
                shiftX = proxy()->pixelMetric(PM_ButtonShiftHorizontal, toolbutton, _widget);
                shiftY = proxy()->pixelMetric(PM_ButtonShiftVertical, toolbutton, _widget);
            }
            // Arrow type always overrules and is always shown
            bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
            if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty())
                || toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly) {
                int alignment = Qt::AlignTop | Qt::AlignHCenter | Qt::TextShowMnemonic;
                if (!proxy()->styleHint(SH_UnderlineShortcut, _opt, _widget))
                    alignment |= Qt::TextHideMnemonic;
                rect.translate(shiftX, shiftY);
                _painter->setFont(toolbutton->font);
                proxy()->drawItemText(_painter, rect, alignment, toolbutton->palette,
                    _opt->state & State_Enabled, toolbutton->text,
                    QPalette::ButtonText);
            }
            else {
                QPixmap pm;
                QSize pmSize = toolbutton->iconSize;
                if (!toolbutton->icon.isNull()) {
                    QIcon::State state = toolbutton->state & State_On ? QIcon::On : QIcon::Off;
                    QIcon::Mode mode;
                    if (!(toolbutton->state & State_Enabled))
                        mode = QIcon::Disabled;
                    else if ((_opt->state & State_MouseOver) && (_opt->state & State_AutoRaise))
                        mode = QIcon::Active;
                    else
                        mode = QIcon::Normal;
                    pm = toolbutton->icon.pixmap(toolbutton->rect.size().boundedTo(toolbutton->iconSize),
                        mode, state);
                    pmSize = pm.size();
                }

                if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly) {
                    _painter->setFont(toolbutton->font);
                    QRect pr = rect,
                        tr = rect;
                    int alignment = Qt::TextShowMnemonic;
                    if (!proxy()->styleHint(SH_UnderlineShortcut, _opt, _widget))
                        alignment |= Qt::TextHideMnemonic;

                    if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                        pr.setHeight(pmSize.height() + 6);
                        tr.adjust(0, pr.height() - 1, 0, -2);
                        pr.translate(shiftX, shiftY);
                        if (!hasArrow)
                            proxy()->drawItemPixmap(_painter, pr, Qt::AlignTop | Qt::AlignHCenter, pm);
                        alignment |= Qt::AlignTop | Qt::AlignHCenter;
                    }
                    else {
                        pr.setWidth(pmSize.width() + 8);
                        tr.adjust(pr.width(), 0, 0, 0);
                        pr.translate(shiftX, shiftY);
                        if (!hasArrow)
                            proxy()->drawItemPixmap(_painter, QStyle::visualRect(_opt->direction, rect, pr), Qt::AlignTop | Qt::AlignHCenter, pm);
                        alignment |= Qt::AlignTop | Qt::AlignHCenter;
                    }
                    tr.translate(shiftX, shiftY);
                    proxy()->drawItemText(_painter, QStyle::visualRect(_opt->direction, rect, tr), alignment, toolbutton->palette,
                        toolbutton->state & State_Enabled, toolbutton->text,
                        QPalette::ButtonText);
                }
                else {
                    rect.translate(shiftX, shiftY);
                    if (!hasArrow)
                        proxy()->drawItemPixmap(_painter, rect, Qt::AlignTop | Qt::AlignHCenter, pm);
                }
            }
        }
        return;
    }
    return QProxyStyle::drawControl(_element, _opt, _painter, _widget);
}

void ot::TabToolBarButtonProxyStyle::drawComplexControl(ComplexControl _control, const QStyleOptionComplex* _opt, QPainter* _painter, const QWidget* _widget) const {
    if (_control == CC_ToolButton) {
        if (const QStyleOptionToolButton* toolbutton
            = qstyleoption_cast<const QStyleOptionToolButton*>(_opt)) {
            QRect button, menuarea;
            button = proxy()->subControlRect(_control, toolbutton, SC_ToolButton, _widget);
            menuarea = proxy()->subControlRect(_control, toolbutton, SC_ToolButtonMenu, _widget);

            State bflags = toolbutton->state & ~State_Sunken;

            if (bflags & State_AutoRaise) {
                if (!(bflags & State_MouseOver) || !(bflags & State_Enabled)) {
                    bflags &= ~State_Raised;
                }
            }
            State mflags = bflags;
            if (toolbutton->state & State_Sunken) {
                if (toolbutton->activeSubControls & SC_ToolButton)
                    bflags |= State_Sunken;
                mflags |= State_Sunken;
            }

            QStyleOption tool(0);
            tool.palette = toolbutton->palette;
            if (toolbutton->subControls & SC_ToolButton) {
                if (bflags & (State_Sunken | State_On | State_Raised)) {
                    tool.rect = button;
                    tool.state = bflags;
                    proxy()->drawPrimitive(PE_PanelButtonTool, &tool, _painter, _widget);
                }
            }

            if (toolbutton->state & State_HasFocus) {
                QStyleOptionFocusRect fr;
                fr.QStyleOption::operator=(*toolbutton);
                fr.rect.adjust(3, 3, -3, -3);
                if (toolbutton->features & QStyleOptionToolButton::MenuButtonPopup)
                    fr.rect.adjust(0, 0, -proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator,
                        toolbutton, _widget), 0);
                proxy()->drawPrimitive(PE_FrameFocusRect, &fr, _painter, _widget);
            }
            QStyleOptionToolButton label = *toolbutton;
            label.state = bflags;
            int fw = proxy()->pixelMetric(PM_DefaultFrameWidth, _opt, _widget);
            label.rect = button.adjusted(fw, fw, -fw, -fw);
            proxy()->drawControl(CE_ToolButtonLabel, &label, _painter, _widget);

            if (toolbutton->subControls & SC_ToolButtonMenu) {
                tool.rect = menuarea;
                tool.state = mflags;
                if (mflags & (State_Sunken | State_On | State_Raised))
                    proxy()->drawPrimitive(PE_IndicatorButtonDropDown, &tool, _painter, _widget);
                proxy()->drawPrimitive(PE_IndicatorArrowDown, &tool, _painter, _widget);
            }
            else if (toolbutton->features & QStyleOptionToolButton::HasMenu) {
                int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, toolbutton, _widget);
                QRect ir = toolbutton->rect;
                QStyleOptionToolButton newBtn = *toolbutton;
                newBtn.rect = QRect(ir.center().x() + 1 - (mbi - 6) / 2, ir.y() + ir.height() - mbi + 4, mbi - 6, mbi - 6);
                proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, _painter, _widget);
            }
        }
        return;
    }
    return QProxyStyle::drawComplexControl(_control, _opt, _painter, _widget);
}
