// @otlicense
// File: ImagePainter.h
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
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qrect.h>
#include <QtCore/qsize.h>

class QPainter;

namespace ot {

	class OT_WIDGETS_API_EXPORTONLY ImagePainter {
		OT_DECL_NOMOVE(ImagePainter)
	public:
		ImagePainter() = default;
		ImagePainter(const ImagePainter&) = default;
		virtual ~ImagePainter() = default;

		ImagePainter& operator=(const ImagePainter&) = default;

		virtual void paintImage(QPainter* _painter, const QRect& _bounds, bool _maintainAspectRatio) const = 0;
		virtual void paintImage(QPainter* _painter, const QRectF& _bounds, bool _maintainAspectRatio) const = 0;

		virtual ImagePainter* createCopy() const = 0;

		virtual QSize getDefaultImageSize() const = 0;
		virtual QSizeF getDefaultImageSizeF() const = 0;

	};

}