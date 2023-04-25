/*
 *	File:		aApplication.cpp
 *	Package:	akGui
 *
 *  Created on: October 23, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akGui/aApplication.h>

static int argc = 0;

ak::aApplication::aApplication() : QApplication(argc, nullptr) {}

ak::aApplication::~aApplication() {}
