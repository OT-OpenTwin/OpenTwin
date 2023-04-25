/*
 *	File:		dllmain.cpp
 *	Package:	-NONE-
 *
 *  Created on: March 25, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2023 OpenTwin
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <Windows.h>

#include <QtCore/qstring.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

