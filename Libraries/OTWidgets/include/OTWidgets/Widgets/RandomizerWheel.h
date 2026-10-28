// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Widgets/WidgetBase.h"
#include "OTWidgets/Widgets/RandomizerWheelItem.h"

// Qt header
#include <QtWidgets/qlabel.h>

class QPropertyAnimation;

namespace ot
{

	class OT_WIDGETS_API_EXPORT RandomizerWheel : public QWidget, public WidgetBase
	{
		Q_OBJECT
		Q_PROPERTY(qreal rotationAngle READ getRotationAngle WRITE setRotationAngle)

	public:
		explicit RandomizerWheel(QWidget* _parent = nullptr);

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		void addItem(const QString& _text, const QVariant& _data = QVariant());
		void addItem(RandomizerWheelItem* _item);
		void addItems(const std::vector<std::pair<QString, QVariant>>& _textDataPairs);
		void addItems(const QVector<RandomizerWheelItem*>& _items);
		int getItemCount() const { return m_items.size(); };

		void spinToIndex(int _index);

		int getCurrentIndex() const;

		void setRotationAngle(qreal _angle);
		qreal getRotationAngle() const { return m_rotationAngle; };

	Q_SIGNALS:
		void spinFinished(RandomizerWheelItem* _selectedItem);
		void currentIndexChanged(int _index);

	protected:
		void paintEvent(QPaintEvent* _event) override;

	private Q_SLOTS:
		void slotSpinFinished();

	private:
		qreal normalizeAngle(qreal _angle) const;

		QVector<RandomizerWheelItem*> m_items;
		qreal m_rotationAngle = 0.0;
		int m_lastIndex = -1;

		QPropertyAnimation* m_animation = nullptr;
	};
}