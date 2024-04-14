//! @file Splitter.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qsplitter.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT Splitter : public QSplitter {
		Q_OBJECT
		OT_DECL_NOCOPY(Splitter)
	public:
		Splitter(QWidget* _parent = (QWidget*)nullptr);
		Splitter(Qt::Orientation _orientation, QWidget* _parent = (QWidget*)nullptr);
		virtual ~Splitter();

	private Q_SLOTS:
		void slotGlobalColorStyleChanged(const ColorStyle& _style);
	};

}