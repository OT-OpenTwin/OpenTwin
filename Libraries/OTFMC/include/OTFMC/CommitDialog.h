// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Dialog.h"

namespace ot {
	
	class LineEdit;

	class CommitDialog : public Dialog {
		OT_DECL_NOCOPY(CommitDialog)
		OT_DECL_NOMOVE(CommitDialog)
		OT_DECL_NODEFAULT(CommitDialog)
	public:
		CommitDialog(QWidget* _parent);
		~CommitDialog();

	private Q_SLOTS:
		void slotConfirm();

	private:
		LineEdit* m_commitMessage;

	};
}