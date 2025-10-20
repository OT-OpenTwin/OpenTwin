//! @file SvgImagePainter.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/ImagePainter.h"

// Qt header
#include <QtCore/qbytearray.h>

class QSvgRenderer;

namespace ot {

	class OT_WIDGETS_API_EXPORT SvgImagePainter : public ImagePainter {
		OT_DECL_NODEFAULT(SvgImagePainter)
		OT_DECL_NOMOVE(SvgImagePainter)
	public:
		//! @brief Constructor.
		//! @param _svgData The svg data to be rendered.
		SvgImagePainter(const QByteArray& _svgData);
		SvgImagePainter(const SvgImagePainter& _other);
		virtual ~SvgImagePainter();

		SvgImagePainter& operator=(const SvgImagePainter&) = delete;

		virtual ImagePainter* createCopy() const override { return new SvgImagePainter(*this); };

		virtual void paintImage(QPainter* _painter, const QRect& _bounds, bool _maintainAspectRatio) const override;
		virtual void paintImage(QPainter* _painter, const QRectF& _bounds, bool _maintainAspectRatio) const override;

		virtual QSize getDefaultImageSize() const override;
		virtual QSizeF getDefaultImageSizeF() const override;

		bool isValid() const;

		const QByteArray& getSvgData() const { return m_svgData; }

	private:
		QByteArray m_svgData;
		QSvgRenderer* m_svgRenderer;
	};

}