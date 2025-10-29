// @otlicense

//! @file Painter2DEditDialogEntry.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot {

	class Painter2D;

	class OT_WIDGETS_API_EXPORT Painter2DEditDialogEntry : public QObject {
		Q_OBJECT
	public:
		enum DialogEntryType {
			FillType,
			LinearType,
			RadialType,
			ReferenceType
		};

		Painter2DEditDialogEntry() {};
		virtual ~Painter2DEditDialogEntry();

		virtual DialogEntryType getEntryType() const = 0;
		virtual QWidget* getRootWidget() const = 0;

		//! @brief Creates a new painter.
		//! Caller takes ownership.
		virtual Painter2D* createPainter() const = 0;

	Q_SIGNALS:
		void valueChanged();

	public Q_SLOTS:
		void slotValueChanged();
	};

}