// @otlicense
// File: PixmapImagePainter.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTWidgets/ImagePainter.h"

// Qt header
#include <QtGui/qpixmap.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PixmapImagePainter : public ImagePainter {
		OT_DECL_NODEFAULT(PixmapImagePainter)
		OT_DECL_NOMOVE(PixmapImagePainter)
	public:
		PixmapImagePainter(const QPixmap& _pixmap);
		PixmapImagePainter(const PixmapImagePainter& _other);
		virtual ~PixmapImagePainter();

		PixmapImagePainter& operator=(const PixmapImagePainter&) = delete;

		virtual void paintImage(QPainter* _painter, const QRect& _bounds, bool _maintainAspectRatio) const override;
		virtual void paintImage(QPainter* _painter, const QRectF& _bounds, bool _maintainAspectRatio) const override;

		virtual ImagePainter* createCopy() const override { return new PixmapImagePainter(*this); };

		virtual QSize getDefaultImageSize() const override { return m_pixmap.size(); };
		virtual QSizeF getDefaultImageSizeF() const override { return m_pixmap.size().toSizeF(); };

	private:
		QPixmap m_pixmap;
	};

}