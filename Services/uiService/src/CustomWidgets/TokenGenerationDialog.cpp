// @otlicense

// Frontend header
#include "AppBase.h"
#include "Login/Authentication.h"
#include "CustomWidgets/TokenGenerationDialog.h"
#include "Component/ExternalServicesComponent.h"

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTCrypto/Key/GlobalCryptoKeyIDs.h"
#include "OTCrypto/Storage/GlobalCryptoKeyStore.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTWidgets/Widgets/Label.h"
#include "OTWidgets/Widgets/Layout.h"
#include "OTWidgets/Widgets/SpinBox.h"
#include "OTWidgets/Widgets/ComboBox.h"
#include "OTWidgets/Widgets/LineEdit.h"
#include "OTWidgets/Widgets/GroupBox.h"
#include "OTWidgets/Widgets/PushButton.h"

// Qt header
#include <QtCore/qdatetime.h>
#include <QtGui/qclipboard.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qapplication.h>

// std header
#include <thread>

// ###########################################################################################################################################################################################################################################################################################################################

// Constructors / Destructor

ot::TokenGenerationDialog::TokenGenerationDialog(QWidget* parent) : TokenGenerationDialog(Config(), parent) {}

ot::TokenGenerationDialog::TokenGenerationDialog(const Config& _config, QWidget* parent)
	: Dialog(parent), m_updateLifetimeTimer(this)
{
	VBoxLayout* mainLayout = new VBoxLayout(this);

	HBoxLayout* inputLayout = new HBoxLayout();
	mainLayout->addLayout(inputLayout);

	inputLayout->addWidget(new Label("Lifetime:", this));
	m_desiredLifetime = new SpinBox(1, _config.maxLifetimeDays, _config.maxLifetimeDays, this);
	connect(m_desiredLifetime, &SpinBox::valueChanged, this, &TokenGenerationDialog::updateDesiredLifetimePreview);
	inputLayout->addWidget(m_desiredLifetime, 1);

	m_desiredLifetimeUnit = new ComboBox(this);
	m_desiredLifetimeUnit->addItem("Hours", QVariant(static_cast<int32_t>(LifetimeUnit::Hours)));
	m_desiredLifetimeUnit->addItem("Days", QVariant(static_cast<int32_t>(LifetimeUnit::Days)));
	m_desiredLifetimeUnit->setCurrentIndex(1);
	connect(m_desiredLifetimeUnit, qOverload<int>(&ComboBox::currentIndexChanged), this, &TokenGenerationDialog::updateDesiredLifetimePreview);
	inputLayout->addWidget(m_desiredLifetimeUnit);

	HBoxLayout* lifetimeLayout = new HBoxLayout();
	mainLayout->addLayout(lifetimeLayout);
	m_desiredLifetimeLabel = new Label("", this);
	m_desiredLifetimeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lifetimeLayout->addWidget(m_desiredLifetimeLabel, 1);

	m_result.group = new GroupBox("Token", this);
	mainLayout->addWidget(m_result.group);

	VBoxLayout* tokenLayout = new VBoxLayout(m_result.group);
	
	m_result.token = new LineEdit(this);
	tokenLayout->addWidget(m_result.token);

	m_result.clipboardInfo = new Label(" ", this);
	QFont clipFont = m_result.clipboardInfo->font();
	clipFont.setPointSize(clipFont.pointSize() - 1);
	clipFont.setItalic(true);
	m_result.clipboardInfo->setFont(clipFont);
	tokenLayout->addWidget(m_result.clipboardInfo);

	m_result.lifetime = new Label("", this);
	m_result.lifetime->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tokenLayout->addWidget(m_result.lifetime);
	
	HBoxLayout* buttonLayout = new HBoxLayout();
	mainLayout->addLayout(buttonLayout);

	buttonLayout->addStretch(1);
	
	m_generateTokenButton = new PushButton("Generate Token", this);
	buttonLayout->addWidget(m_generateTokenButton);
	connect(m_generateTokenButton, &PushButton::clicked, this, &TokenGenerationDialog::generateToken);

	m_closeButton = new PushButton("Close", this);
	buttonLayout->addWidget(m_closeButton);
	connect(m_closeButton, &PushButton::clicked, this, &TokenGenerationDialog::closeCancel);

	m_updateLifetimeTimer.setInterval(1000);
	m_updateLifetimeTimer.connect(&m_updateLifetimeTimer, &QTimer::timeout, this, &TokenGenerationDialog::updateDesiredLifetimePreview);
	m_updateLifetimeTimer.start();
	updateDesiredLifetimePreview();

	resize(340, 200);
}

ot::TokenGenerationDialog::~TokenGenerationDialog()
{

}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Events

void ot::TokenGenerationDialog::closeEvent(QCloseEvent* _event)
{
	if (m_state.has(State::GeneratingToken))
	{
		m_state.set(State::Stopping);
		_event->ignore();
		return;
	}

	Dialog::closeEvent(_event);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ot::TokenGenerationDialog::generateToken()
{
	if (m_state.has(State::GeneratingToken))
	{
		return;
	}
	m_state.set(State::GeneratingToken);

	setLockedState(true);

	std::thread workerThread(&TokenGenerationDialog::generateTokenWorker, this);
	workerThread.detach();
}

void ot::TokenGenerationDialog::generateTokenCompleted()
{
	setLockedState(false);

	if (m_state.has(State::Success))
	{
		m_result.token->setText(m_result.newToken);
		m_result.lifetime->setText(m_result.newTokenLifetime);
		m_state.remove(State::Success);

		m_generateTokenButton->setEnabled(false);
		copyTokenToClipboard();
	}
	else if (m_state.has(State::Stopping))
	{
		closeCancel();
	}

	m_state.remove(State::GeneratingToken);
}

void ot::TokenGenerationDialog::updateDesiredLifetimePreview()
{
	QDateTime currentDateTime = QDateTime::currentDateTime();
	currentDateTime = currentDateTime.toUTC();
	currentDateTime = currentDateTime.addSecs(getDesiredLifetimeInSeconds());

	m_desiredLifetimeLabel->setText("Expires on the " + currentDateTime.toString("yyyy-MM-dd") + " at " + currentDateTime.toString("hh:mm:ss") + " UTC");
}

void ot::TokenGenerationDialog::copyTokenToClipboard()
{
	QClipboard* clipboard = QApplication::clipboard();
	if (clipboard)
	{
		clipboard->setText(m_result.token->text());
		m_result.clipboardInfo->setText("Token copied to clipboard");
		QTimer::singleShot(3000, this, &TokenGenerationDialog::hideClipboardInfo);
	}
}

void ot::TokenGenerationDialog::hideClipboardInfo()
{
	m_result.clipboardInfo->setText(" ");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper and worker

void ot::TokenGenerationDialog::setLockedState(bool _locked)
{
	if (m_state.has(State::Locked) == _locked)
	{
		return;
	}

	m_updateLifetimeTimer.stop();

	const bool enabledState = !_locked;

	m_desiredLifetime->setEnabled(enabledState);

	m_result.token->setEnabled(enabledState);

	m_generateTokenButton->setEnabled(enabledState);
	m_closeButton->setEnabled(enabledState);

	m_state.set(State::Locked, _locked);

	if (!_locked)
	{
		m_updateLifetimeTimer.start();
		updateDesiredLifetimePreview();
	}
}

void ot::TokenGenerationDialog::generateTokenWorker()
{
	try
	{
		const std::string keyId = GlobalCryptoKeyIDs::localTokenSignature();

		if (!GlobalCryptoKeyStore::hasKey(keyId))
		{
			std::string newId = GlobalCryptoKeyStore::createKey(keyId);
			if (newId != keyId)
			{
				// Failed to store the key with the correct ID, remove the newly created key
				if (!GlobalCryptoKeyStore::removeKey(newId))
				{
					throw Exception::Runtime("Failed to remove newly created key with ID: " + newId);
				}
			}
		}

		int validity = m_desiredLifetime->value();

		std::vector<uint8_t> publicKey = GlobalCryptoKeyStore::getPublicKey(keyId);

		if (publicKey.empty())
		{
			throw Exception::Runtime("Failed to retrieve public key for key ID: " + keyId);
		}

		std::string publicKeyString = ot::String::toBase64Url(publicKey);
		
		JsonDocument requestDoc;
		requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_GenerateAccessToken, requestDoc.GetAllocator()), requestDoc.GetAllocator());
		requestDoc.AddMember(OT_ACTION_PARAM_Key, JsonString(publicKeyString, requestDoc.GetAllocator()), requestDoc.GetAllocator());
		requestDoc.AddMember(OT_ACTION_PARAM_Validity, getDesiredLifetimeInSeconds(), requestDoc.GetAllocator());
		ReturnMessage response = sendToAuth(std::move(requestDoc));

		if (!response.isOk())
		{
			throw Exception::Runtime("Failed to generate token: " + response.getWhat());
		}

		JsonDocument responseDoc;
		if (!responseDoc.fromJson(response.getWhat()))
		{
			throw Exception::Runtime("Failed to parse response from authorization service: " + response.getWhat());
		}

		m_result.newToken = QString::fromStdString(json::getString(responseDoc, OT_ACTION_PARAM_Token));
		QDateTime validUntil = QDateTime::fromSecsSinceEpoch(json::getUInt64(responseDoc, OT_ACTION_PARAM_Expires));
		m_result.newTokenLifetime = "Valid until " + validUntil.toUTC().toString("yyyy-MM-dd hh:mm:ss") + " UTC";

		m_state.set(State::Success);
	}
	catch (const std::exception& _e)
	{
		OT_LOG_E(_e.what());
	}
	catch (...)
	{
		OT_LOG_E("Unknown error occurred while generating token");
	}

	QMetaObject::invokeMethod(this, &TokenGenerationDialog::generateTokenCompleted, Qt::QueuedConnection);
}

uint64_t ot::TokenGenerationDialog::getDesiredLifetimeInSeconds() const
{
	uint64_t value = static_cast<uint64_t>(m_desiredLifetime->value());

	if (m_desiredLifetimeUnit->currentData().toInt() == static_cast<int32_t>(LifetimeUnit::Days))
	{
		value *= 24;
	}

	// Here the value is in hours, so we multiply by 3600 to convert to seconds
	value *= 3600;

	return value;
}

ot::ReturnMessage ot::TokenGenerationDialog::sendToAuth(JsonDocument&& _request)
{
	try
	{
		AppBase* app = AppBase::instance();
		const auto& loginData = app->getCurrentLoginData();
		
		if (!loginData.isValid())
		{
			return ot::ReturnMessage::failed("Invalid login data, cannot send request to authorization service");
		}

		if (loginData.getAuthorizationUrl().empty())
		{
			return ot::ReturnMessage::failed("Authorization URL is empty, cannot send request to authorization service");
		}

		Authentication::addAuthenticationData(loginData, _request);

		std::string responseStr;
		if (!ot::msg::send("", loginData.getAuthorizationUrl(), ot::EXECUTE_ONE_WAY_TLS, _request.toJson(), responseStr, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))
		{
			return ot::ReturnMessage::failed("Failed to send request to authorization service");
		}

		return ReturnMessage::fromJson(responseStr);
	}
	catch (const std::exception& _e)
	{
		return ot::ReturnMessage::failed("Failed to send request to authorization service: " + std::string(_e.what()));
	}
	catch (...)
	{
		return ot::ReturnMessage::failed("Unknown error occurred while sending request to authorization service");
	}
}
