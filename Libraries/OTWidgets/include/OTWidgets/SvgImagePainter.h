// @otlicense
// File: SvgImagePainter.h
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