// @otlicense

/*
 *	File:		rbeWrapperDatatypes.h
 *	Package:	rbeWrapper
 *
 *  Created on: September 17, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 Alexander Kuester
 *	This file is part of the RubberbandEngine package.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

#ifdef RBE_OSG_WRAPPER_PROJ_BUILD
#define RBE_OSG_API_EXPORT __declspec(dllexport)
#else
#define RBE_OSG_API_EXPORT __declspec(dllimport)
#endif // RBE_OSG_WRAPPER_PROJ_BUILD
