//! @file SVGWidget.h
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtCore/qobject.h>

class QSvgWidget;

namespace ot {

	class OT_WIDGETS_API_EXPORT SVGWidget : public QObject, public WidgetBase {
		Q_OBJECT
	public:
		SVGWidget(QWidget* _parent = (QWidget*)nullptr) : SVGWidget(QByteArray(), _parent) {};
		SVGWidget(const QByteArray& _svgData, QWidget* _parent = (QWidget*)nullptr);

		virtual QWidget* getQWidget(void) override;
		virtual const QWidget* getQWidget(void) const override;

		void setSvgData(const QByteArray& _data);
		const QByteArray& getSvgData(void) const { return m_data; };

		void setFixedSize(int _width, int _height) { this->setFixedSize(QSize(_width, _height)); };
		void setFixedSize(const QSize& _size);

		void loadFromFile(const QString& _filePath);

	private:
		QSvgWidget* m_widget;
		QByteArray m_data;
	};

}