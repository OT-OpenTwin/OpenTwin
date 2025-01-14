//! @file GraphicsViewClipboardData.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsCopyInformation.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qmimedata.h>

// std header
#include <list>

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsViewClipboardData : public QMimeData {
	public:
		
		GraphicsViewClipboardData() = default;
		GraphicsViewClipboardData(const GraphicsViewClipboardData&) = default;
		GraphicsViewClipboardData(GraphicsViewClipboardData&&) = default;
		virtual ~GraphicsViewClipboardData() = default;

		GraphicsViewClipboardData& operator = (const GraphicsViewClipboardData&) = default;
		GraphicsViewClipboardData& operator = (GraphicsViewClipboardData&&) = default;

		void setViewOwner(const BasicServiceInformation& _owner) { m_viewOwner = _owner; };
		const BasicServiceInformation& getViewOwner(void) const { return m_viewOwner; };

		void setCopyInfo(const GraphicsCopyInformation& _info) { m_info = _info; };
		const GraphicsCopyInformation& getCopyInfo(void) const { return m_info; };

	private:
		BasicServiceInformation m_viewOwner;
		GraphicsCopyInformation m_info;

	};

}