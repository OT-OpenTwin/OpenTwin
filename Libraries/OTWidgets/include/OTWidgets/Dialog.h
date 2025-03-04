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

		enum ShowFlag {
			DefaultShow    = 0 << 0,
			CenterOnParent = 1 << 0, //! \brief Positions this dialog in the middle of the parent widget.
			FitOnScreen    = 1 << 1, //! \brief Ensures the dialog fits on any screen.
			IdealFit       = CenterOnParent | FitOnScreen
		};
		typedef Flags<ShowFlag> ShowFlags;

		enum class DialogState {
			NoState        = 0 << 0,
			MousePressed   = 1 << 0,
			Closing        = 1 << 1
		};
		typedef Flags<DialogState> DialogStateFlags;

		Dialog(QWidget* _parent = (QWidget*)nullptr);
		Dialog(const DialogCfg& _config, QWidget* _parent = (QWidget*)nullptr);
		virtual ~Dialog();
		
		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		//! @brief Center the dialog on parent and call exec
		DialogResult showDialog(const ShowFlags& _showFlags = ShowFlags(IdealFit));

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setDialogFlag(DialogCfg::DialogFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };
		void setDialogFlags(DialogCfg::DialogFlags _flags) { m_flags = _flags; };
		DialogCfg::DialogFlags dialogFlags(void) const { return m_flags; };

		void setDialogResult(DialogResult _result) { m_result = _result; };
		DialogResult dialogResult(void) const { return m_result; };

		void setDialogName(const std::string& _name) { m_dialogName = _name; };
		const std::string& dialogName(void) const { return m_dialogName; };

		const DialogStateFlags& getDialogState(void) const { return m_state; };

	Q_SIGNALS:
		//! @brief Is emitted just before closing the dialog window.
		//! Continue close is set to true by default.
		//! Set to false if the dialog window should remain open otherwise keep unchanged.
		void closingDialog(bool& _continueClose);

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
		virtual void closeEvent(QCloseEvent* _event) override;

		//! @brief Is called before closing the dialog.
		//! By default if will emit the closingDialog signal and if continue close was set to false the close will cancel.
		virtual bool mayCloseDialogWindow(void);

	private:
		DialogStateFlags m_state;
		QPoint m_lastMousePos;
		DialogCfg::DialogFlags m_flags;
		DialogResult m_result;
		std::string m_dialogName;
	};
}

OT_ADD_FLAG_FUNCTIONS(ot::Dialog::ShowFlag)
OT_ADD_FLAG_FUNCTIONS(ot::Dialog::DialogState)