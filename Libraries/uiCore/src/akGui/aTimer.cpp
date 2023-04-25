/*
 *	File:		aTimer.cpp
 *	Package:	akGui
 *
 *  Created on: September 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akGui/aTimer.h>

ak::aTimer::aTimer()
	: QTimer(), aObject(otTimer)
{}

ak::aTimer::~aTimer() { A_OBJECT_DESTROYING }