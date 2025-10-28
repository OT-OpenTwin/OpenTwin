// @otlicense

// OpenTwin header
#include "OTWidgets/CustomValidator.h"

void ot::CustomValidator::fixup(QString& _input) const {
	int pos = 0;
	this->fixup(_input, pos);
}