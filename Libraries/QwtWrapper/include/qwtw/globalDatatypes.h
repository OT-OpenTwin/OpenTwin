/*
 *	File:		globalDatatypes.h
 *
 *  Created on: May 17, 2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the QwtWrapper project.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

#ifdef QWTW_LIB
#define QWTW_LIB_EXPORT __declspec(dllexport)
#else
#define QWTW_LIB_EXPORT __declspec(dllimport)
#endif // QWTW_LIB
