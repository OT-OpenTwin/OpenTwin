// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/DialogCfg.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qdialog.h>

namespace ot {

	class PushButton;

	//! @class Dialog
	//! @brief Base class for creating customizable dialog windows.
	//! The Dialog class is used as a base class for dialogs.
	//! The key features are:
	//! - Customizable dialog display using ShowFlags.
	//! - State tracking using DialogStateFlags.
	//! - Handling of dialog results with predefined result types.
	//! - Automatic button generation and signal-slot connections for user interactions.
	//! 
	class OT_WIDGETS_API_EXPORT Dialog : public QDialog, public ot::WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(Dialog)
		OT_DECL_NOMOVE(Dialog)
	public:
		//! @brief Defines possible dialog results.
		enum DialogResult {
			Ok,      //! @brief Represents an OK result.
			Confirm, //! @brief Represents a Confirm result.
			Yes,     //! @brief Represents a Yes result.
			No,      //! @brief Represents a No result.
			Retry,   //! @brief Represents a Retry result.
			Cancel   //! @brief Represents a Cancel result.
		};

		//! @brief Flags for controlling how the dialog is displayed.
		enum ShowFlag {
			DefaultShow = 0 << 0,    //! @brief Default behavior.
			CenterOnParent = 1 << 0, //! @brief Positions this dialog in the middle of the parent widget.
			FitOnScreen = 1 << 1,    //! @brief Ensures the dialog fits on any screen.
			IdealFit = CenterOnParent | FitOnScreen //! @brief Combines centering and fitting.
		};
		typedef Flags<ShowFlag> ShowFlags;

		//! @brief Defines different states the dialog can be in.
		enum class DialogState {
			NoState = 0 << 0,      //! @brief Default state.
			MousePressed = 1 << 0, //! @brief The mouse is pressed.
			Closing = 1 << 1       //! @brief The dialog is closing.
		};
		typedef Flags<DialogState> DialogStateFlags;

		//! @brief Constructs a Dialog instance.
		//! @param _parent The parent widget.
		Dialog(QWidget* _parent = (QWidget*)nullptr);

		//! @brief Constructs a Dialog instance with a configuration.
		//! @param _config The dialog configuration.
		//! @param _parent The parent widget.
		Dialog(const DialogCfg& _config, QWidget* _parent = (QWidget*)nullptr);

		//! @brief Destroys the Dialog instance.
		virtual ~Dialog();

		//! @brief Returns a pointer to the root widget of this object.
		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		//! @brief Displays the dialog with the specified show flags.
		//! @param _showFlags Flags determining how the dialog is displayed.
		//! @return The result of the dialog after it is closed.
		DialogResult showDialog(const ShowFlags& _showFlags = ShowFlags(IdealFit));

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Returns true if the result is either Ok or Confirm.
		bool isSuccess() const { return m_result == Dialog::Ok || m_result == Dialog::Confirm; };

		//! @brief Sets a dialog flag.
		//! @param _flag The flag to set.
		//! @param _active Whether the flag should be active.
		void setDialogFlag(DialogCfg::DialogFlag _flag, bool _active = true) { m_flags.setFlag(_flag, _active); };

		//! @brief Sets multiple dialog flags.
		//! @param _flags The flags to set.
		void setDialogFlags(DialogCfg::DialogFlags _flags) { m_flags = _flags; };

		//! @brief Retrieves the dialog flags.
		//! @return The current dialog flags.
		DialogCfg::DialogFlags dialogFlags() const { return m_flags; };

		//! @brief Sets the dialog result.
		//! @param _result The result to set.
		void setDialogResult(DialogResult _result) { m_result = _result; };

		//! @brief Retrieves the dialog result.
		//! @return The current dialog result.
		DialogResult dialogResult() const { return m_result; };

		//! @brief Sets the dialog name.
		//! @param _name The name to set.
		void setDialogName(const std::string& _name) { m_dialogName = _name; };

		//! @brief Retrieves the dialog name.
		//! @return The current dialog name.
		const std::string& dialogName() const { return m_dialogName; };

		//! @brief Retrieves the dialog state flags.
		//! @return The current dialog state flags.
		const DialogStateFlags& getDialogState() const { return m_state; };

		//! @brief Generates default push buttons based on provided mappings.
		//! @param _buttonTextToResultList List of button texts and their corresponding results.
		//! @param _layout If provided, buttons will be added to the layout in the given order.
		//! @return The list of generated buttons.
		std::list<PushButton*> generateDefaultButtons(const std::list<std::pair<QString, DialogResult>>& _buttonTextToResultList, QLayout* _layout = (QLayout*)nullptr);

		//! @brief Generates default push buttons based on provided button results.
		//! @param _buttonResults List of button results. Default text will be used for the results.
		//! @param _layout If provided, buttons will be added to the layout in the given order.
		//! @return The list of generated buttons.
		std::list<PushButton*> generateDefaultButtons(const std::initializer_list<DialogResult>& _buttonResults, QLayout* _layout = (QLayout*)nullptr);

	Q_SIGNALS:
		//! @brief Emitted just before closing the dialog window.
		//! Continue close is set to true by default.
		//! Set to false if the dialog window should remain open.
		void closingDialog(bool& _continueClose);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public slots

	public Q_SLOTS:
		void closeDialog(DialogResult _result);
		void closeOk();
		void closeConfirm();
		void closeYes();
		void closeNo();
		void closeRetry();
		void closeCancel();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected

	protected:
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void mouseMoveEvent(QMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QMouseEvent* _event) override;
		virtual void closeEvent(QCloseEvent* _event) override;

		//! @brief Determines whether the dialog may close.
		//! @return True if the dialog may close, otherwise false.
		virtual bool mayCloseDialogWindow();

	private:
		//! @brief The current dialog state flags.
		DialogStateFlags m_state;

		//! @brief The last recorded mouse position.
		QPoint m_lastMousePos;

		//! @brief The dialog flags.
		DialogCfg::DialogFlags m_flags;

		//! @brief The current dialog result.
		DialogResult m_result;

		//! @brief The name of the dialog.
		std::string m_dialogName;
	};
}

OT_ADD_FLAG_FUNCTIONS(ot::Dialog::ShowFlag)
OT_ADD_FLAG_FUNCTIONS(ot::Dialog::DialogState)