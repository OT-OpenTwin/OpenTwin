// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "OTWidgets/Dialog/Dialog.h"

// Qt header
#include <QtCore/qtimer.h>

namespace ot
{

	class Label;
	class SpinBox;
	class ComboBox;
	class LineEdit;
	class GroupBox;

	class TokenGenerationDialog : public Dialog
	{
		Q_OBJECT
		OT_DECL_NOCOPY(TokenGenerationDialog)
		OT_DECL_NOMOVE(TokenGenerationDialog)
	public:
		struct Config
		{
			int maxLifetimeDays = 30;
		};
		
		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructors / Destructor

		TokenGenerationDialog(QWidget* parent = nullptr);
		TokenGenerationDialog(const Config& _config, QWidget* parent = nullptr);
		virtual ~TokenGenerationDialog();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Events

	protected:
		virtual void closeEvent(QCloseEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots

	private Q_SLOTS:
		void generateToken();
		void generateTokenCompleted();
		void updateDesiredLifetimePreview();

	private:
		enum class LifetimeUnit : int32_t
		{
			Hours,
			Days
		};

		enum class State
		{
			NoState         = 0 << 0,
			GeneratingToken = 1 << 0,
			Stopping        = 1 << 1,
			Locked          = 1 << 2,
			Success         = 1 << 3,
		};
		typedef Flags<State> StateFlags;
		OT_ADD_FRIEND_FLAG_FUNCTIONS(State, StateFlags)
		StateFlags m_state;

		SpinBox* m_desiredLifetime = nullptr;
		ComboBox* m_desiredLifetimeUnit = nullptr;
		Label* m_desiredLifetimeLabel = nullptr;

		struct Result
		{
			GroupBox* group = nullptr;
			LineEdit* token = nullptr;
			Label* lifetime = nullptr;

			QString newToken;
			QString newTokenLifetime;
		};
		Result m_result;
		QTimer m_updateLifetimeTimer;

		PushButton* m_generateTokenButton = nullptr;
		PushButton* m_closeButton = nullptr;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper and worker

		void setLockedState(bool _locked);

		void generateTokenWorker();
		uint64_t getDesiredLifetimeInSeconds() const;
		
		ReturnMessage sendToAuth(JsonDocument&& _request);

	};

}