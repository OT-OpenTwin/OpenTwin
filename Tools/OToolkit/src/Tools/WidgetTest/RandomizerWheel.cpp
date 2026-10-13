#include "Tools/WidgetTest/RandomizerWheel.h"

#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QPaintEvent>
#include <QtMath>

#include <algorithm>
#include <cmath>

RandomizerWheel::RandomizerWheel(QWidget* _parent)
	: QWidget(_parent), m_tickEffect(this)
{
    setMinimumSize(300, 300);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_animation = new QPropertyAnimation(this, "rotationAngle", this);
    m_animation->setDuration(4500);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_animation, &QPropertyAnimation::finished, this, &RandomizerWheel::slotSpinFinished);

    m_tickEffect.setSource(QUrl(QStringLiteral("qrc:/tick.wav")));
}

void RandomizerWheel::addItem(const QString& _option, const QVariant& _data)
{
	addItem(new RandomizerWheelItem(_option, _data));
}

void RandomizerWheel::addItem(RandomizerWheelItem* _item)
{
    m_items.append(_item);
    update();
}

void RandomizerWheel::addItems(const std::vector<std::pair<QString, QVariant>>& _textDataPairs)
{
	QVector<RandomizerWheelItem*> items;
	items.reserve(_textDataPairs.size());
    for (const auto& pair : _textDataPairs)
    {
        items.append(new RandomizerWheelItem(pair.first, pair.second));
	}
	addItems(items);
}

void RandomizerWheel::addItems(const QVector<RandomizerWheelItem*>& _items)
{
	m_items.append(_items);
	update();
}

void RandomizerWheel::setRotationAngle(qreal _angle)
{
    m_rotationAngle = _angle;
    update();

    const int currentIndex = getCurrentIndex();

    if (currentIndex != m_lastIndex && currentIndex >= 0)
    {
        m_lastIndex = currentIndex;
        m_tickEffect.play();
    }
}

qreal RandomizerWheel::normalizeAngle(qreal _angle) const
{
    _angle = std::fmod(_angle, 360.0);

    if (_angle < 0.0)
    {
        _angle += 360.0;
    }

    return _angle;
}

void RandomizerWheel::spinToIndex(int _index)
{
    if (m_items.isEmpty() ||
        _index < 0 ||
        _index >= m_items.size())
    {
        return;
    }

    m_animation->stop();

    const qreal sectorAngle = 360.0 / m_items.size();

    // Center the selected sector beneath the fixed top pointer.
    const qreal targetAngle =
        normalizeAngle(-(_index + 0.5) * sectorAngle);

    const qreal currentAngle = normalizeAngle(m_rotationAngle);
    const qreal remainingAngle =
        normalizeAngle(targetAngle - currentAngle);

    // Complete five full turns before reaching the selected sector.
    const qreal finalAngle =
        m_rotationAngle + 5.0 * 360.0 + remainingAngle;

    m_animation->setStartValue(m_rotationAngle);
    m_animation->setEndValue(finalAngle);
    m_animation->start();
}

int RandomizerWheel::getCurrentIndex() const
{
    if (m_items.isEmpty())
    {
        return -1;
    }

    const qreal sectorAngle = 360.0 / m_items.size();
    const qreal normalizedAngle = normalizeAngle(m_rotationAngle);

    // Convert the fixed top pointer into the wheel's local coordinate system.
    const qreal pointerAngle = normalizeAngle(270.0 - normalizedAngle);

    // The first sector starts at -90 degrees (270 degrees normalized).
    const qreal relativeAngle = normalizeAngle(pointerAngle - 270.0);

    return static_cast<int>(std::floor(relativeAngle / sectorAngle)) % m_items.size();
}

void RandomizerWheel::slotSpinFinished()
{
    const int selectedIndex = getCurrentIndex();
    if (selectedIndex < 0 || selectedIndex >= m_items.size())
    {
        return;
	}

    Q_EMIT spinFinished(m_items.at(selectedIndex));
}

void RandomizerWheel::paintEvent(QPaintEvent* _event)
{
    Q_UNUSED(_event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    painter.fillRect(rect(), palette().window());

    if (m_items.isEmpty())
    {
        painter.setPen(palette().text().color());
        painter.drawText(rect(), Qt::AlignCenter, "No options");
        return;
    }

    const qreal side = qMin(width(), height());
    const qreal radius = side * 0.43;
    const QPointF center(width() / 2.0, height() / 2.0);

    painter.save();
    painter.translate(center);
    painter.rotate(m_rotationAngle);

    const qreal sectorAngle = 360.0 / m_items.size();

    const QVector<QColor> colors = {
        QColor(38, 75, 115),
        QColor(48, 91, 137),
        QColor(43, 82, 125),
        QColor(57, 103, 150)
    };

    QFont optionFont = font();
    optionFont.setPixelSize(
        qBound(9, static_cast<int>(side * 0.027), 14));
    optionFont.setBold(true);
    painter.setFont(optionFont);

    for (int i = 0; i < m_items.size(); ++i)
    {
        const qreal startAngle = -90.0 + i * sectorAngle;
        const qreal endAngle = startAngle + sectorAngle;

        QPainterPath sector;
        sector.moveTo(0.0, 0.0);

        constexpr int arcSteps = 12;

        for (int step = 0; step <= arcSteps; ++step)
        {
            const qreal angle = qDegreesToRadians(startAngle + (endAngle - startAngle) * step / arcSteps);

            sector.lineTo(radius * qCos(angle), radius * qSin(angle));
        }

        sector.closeSubpath();

        painter.setPen(QPen(QColor(25, 29, 35), 1.5));
        painter.setBrush(colors.at(i % colors.size()));
        painter.drawPath(sector);

        // Place the text along the radial direction of the sector.
        const qreal middleAngle = startAngle + sectorAngle / 2.0;

        painter.save();
        painter.rotate(middleAngle);

        const qreal textRadius = radius * 0.56;
        const qreal textWidth = radius * 0.58;
        const qreal textHeight = qMax(18.0, radius * 0.35);

        painter.translate(textRadius, 0.0);

        // Keep text upright on the lower half of the wheel.
        if (normalizeAngle(middleAngle) > 90.0 && normalizeAngle(middleAngle) < 270.0)
        {
            painter.rotate(180.0);
        }

        painter.setPen(Qt::white);

        const QRectF textRect(-textWidth / 2.0, -textHeight / 2.0, textWidth, textHeight);

        painter.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, m_items.at(i)->getText());

        painter.restore();
    }

    // Outer wheel border.
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(210, 215, 225), 2.0));
    painter.drawEllipse(QPointF(0.0, 0.0), radius, radius);

    painter.restore();

    // Fixed pointer, drawn after rotating the wheel.
    const qreal pointerSize = side * 0.045;

    QPolygonF pointer;
    pointer << QPointF(center.x(), center.y() - radius + pointerSize * 0.15)
        << QPointF(center.x() - pointerSize, center.y() - radius - pointerSize * 1.5)
        << QPointF(center.x() + pointerSize, center.y() - radius - pointerSize * 1.5);

    painter.setPen(QPen(QColor(25, 29, 35), 1.5));
    painter.setBrush(QColor(245, 190, 65));
    painter.drawPolygon(pointer);
}
