// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Dialog.h"

class LogServiceDebugInfoDialog : public ot::Dialog {
public:
	LogServiceDebugInfoDialog(const QString& _name, const QString& _url, const QString& _id, const QString& _pid, const QString& _info, QWidget* _parent);
};