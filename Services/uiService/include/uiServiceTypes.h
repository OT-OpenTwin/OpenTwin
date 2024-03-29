/*
 * uiServiceTypes.h
 *
 *  Created on: September 24, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#pragma once

#include "OTCore/CoreTypes.h"

 // Type definition
typedef ot::UID ViewerUIDtype;
typedef long long ModelIDtype;
typedef ot::UID ModelUIDtype;

// If defined the global session service logic will be used
#define OT_USE_GSS

// If defined the directory service logic will be used
#define OT_USE_DS