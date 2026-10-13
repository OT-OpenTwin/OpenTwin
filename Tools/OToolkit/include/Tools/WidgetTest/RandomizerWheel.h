// @otlicense

#pragma once

#include "Tools/WidgetTest/RandomizerWheelItem.h"

#include <QWidget>
#include <QPropertyAnimation>
#include <QColor>
#include <QFont>
#include <QVector>
#include <QString>
#include <QtMultimedia/qsoundeffect.h>

class RandomizerWheel : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal rotationAngle READ rotationAngle WRITE setRotationAngle)

public:
    explicit RandomizerWheel(QWidget* _parent = nullptr);

	void addItem(const QString& _text, const QVariant& _data = QVariant());
	void addItem(RandomizerWheelItem* _item);
	void addItems(const std::vector<std::pair<QString, QVariant>>& _textDataPairs);
    void addItems(const QVector<RandomizerWheelItem*>& _items);
    int getItemCount() const { return m_items.size(); };
    
    void spinToIndex(int _index);

	int getCurrentIndex() const;

    qreal rotationAngle() const { return m_rotationAngle; }
    void setRotationAngle(qreal _angle);

    void setTickVolume(float _volume) { m_tickEffect.setVolume(_volume); };

Q_SIGNALS:
    void spinFinished(RandomizerWheelItem* _selectedItem);

protected:
    void paintEvent(QPaintEvent* _event) override;

private Q_SLOTS:
	void slotSpinFinished();

private:
    qreal normalizeAngle(qreal _angle) const;

    QVector<RandomizerWheelItem*> m_items;
    qreal m_rotationAngle = 0.0;
    int m_lastIndex = -1;

    QSoundEffect m_tickEffect;

    QPropertyAnimation* m_animation = nullptr;
};
