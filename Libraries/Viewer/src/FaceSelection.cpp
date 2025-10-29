// @otlicense

#include "stdafx.h"
#include "FaceSelection.h"

bool operator==(const FaceSelection& left, const FaceSelection& right)
{
	return (left.getSelectedItem() == right.getSelectedItem() && left.getFaceId() == right.getFaceId());
}