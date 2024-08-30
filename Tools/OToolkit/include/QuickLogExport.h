//! @file QuickLogExport.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/Dialog.h"

namespace ot { class CheckBox; }
namespace ot { class ComboBox; }

class QuickLogExport : public ot::Dialog {
	Q_OBJECT
	OT_DECL_NOCOPY(QuickLogExport)
	OT_DECL_NODEFAULT(QuickLogExport)
public:
	QuickLogExport(const std::list<ot::LogMessage>& _messages);
	virtual ~QuickLogExport();

	bool isAutoClose(void);

private Q_SLOTS:
	void slotExport(void);

private:
	bool messageOkForExport(const ot::LogMessage& _message);

	std::list<ot::LogMessage> m_messages;

	ot::CheckBox* m_autoClose;
	ot::ComboBox* m_user;
	ot::ComboBox* m_project;
};