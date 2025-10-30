// @otlicense
// File: DelayedShowHideHandler.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
