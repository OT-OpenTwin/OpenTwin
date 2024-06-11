//! @file Dialog.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/DialogCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtWidgets/qdialog.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT Dialog : public QDialog, public ot::QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(Dialog)
	public:
		enum DialogResult {
			Ok,
			Yes,
			No,
			Retry,
			Cancel
		};

		enum class DialogState {
			NoState = 0x00,
			MousePressed = 0x01
		};
		typedef Flags<DialogState> DialogStateFlags;

		Dialog(QWidget* _parent = (QWidget*)nullptr);
		Dialog(const DialogCfg& _config, QWidget* _parent = (QWidget*)nullptr);
		virtual ~Dialog();
		
		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) { return this; }

		//! @brief Center the dialog on parent and call exec
		DialogResult showDialog(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setDialogFlag(DialogCfg::DialogFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setDialogFlags(DialogCfg::DialogFlags _flags) { m_flags = _flags; };
		DialogCfg::DialogFlags dialogFlags(void) const { return m_flags; };

		DialogResult dialogResult(void) const { return m_result; };

		void setDialogName(const std::string& _name) { m_dialogName = _name; };
		const std::string& dialogName(void) const { return m_dialogName; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public slots

	public Q_SLOTS:
		void close(DialogResult _result);
		void closeOk(void);
		void closeYes(void);
		void closeNo(void);
		void closeRetry(void);
		void closeCancel(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void mouseMoveEvent(QMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QMouseEvent* _event) override;

	private:
		DialogStateFlags m_state;
		QPoint m_lastMousePos;
		DialogCfg::DialogFlags m_flags;
		DialogResult m_result;
		std::string m_dialogName;
	};
}

OT_ADD_FLAG_FUNCTIONS(ot::Dialog::DialogState)