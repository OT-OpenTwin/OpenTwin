// @otlicense

// OpenTwin header
#include "OTWidgets/DelayedShowHideHandler.h"

ot::DelayedShowHideHandler::DelayedShowHideHandler() {
	this->connect(&m_showHandler, &DelayedCallHandler::timeout, this, &DelayedShowHideHandler::slotShow);
	this->connect(&m_hideHandler, &DelayedCallHandler::timeout, this, &DelayedShowHideHandler::slotHide);
}

ot::DelayedShowHideHandler::~DelayedShowHideHandler() {

}

void ot::DelayedShowHideHandler::show(void) {
	this->stop();
	m_showHandler.callDelayed();
}

void ot::DelayedShowHideHandler::hide(void) {
	this->stop();
	m_hideHandler.callDelayed();
}

void ot::DelayedShowHideHandler::stop(void) {
	m_showHandler.stop();
	m_showHandler.stop();
}

void ot::DelayedShowHideHandler::slotShow(void) {
	Q_EMIT showRequest();
}

void ot::DelayedShowHideHandler::slotHide(void) {
	Q_EMIT hideRequest();
}
