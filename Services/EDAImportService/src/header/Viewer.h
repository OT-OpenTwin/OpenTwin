//-----------------------------------------------------------------------------
// Created on: 24 August 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev (sergey.slyadnev@gmail.com)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

// Local includes
#include "ViewerInteractor.h"

// OpenCascade includes
#include <TopoDS_Shape.hxx>
#include <WNT_Window.hxx>

// Standard includes
#include <vector>

class V3d_Viewer;
class V3d_View;
class AIS_InteractiveContext;
class AIS_ViewController;

//-----------------------------------------------------------------------------

//! Simple 3D viewer.
class Viewer
{
public:

    Viewer(const int left,
        const int top,
        const int width,
        const int height);

public:

    Viewer& operator<<(const TopoDS_Shape& shape)
    {
        this->AddShape(shape);
        return *this;
    }

    void AddShape(const TopoDS_Shape& shape);

    void StartMessageLoop();

private:

    static LRESULT WINAPI
        wndProcProxy(HWND hwnd,
            UINT message,
            WPARAM wparam,
            LPARAM lparam);

    LRESULT CALLBACK
        wndProc(HWND hwnd,
            UINT message,
            WPARAM wparam,
            LPARAM lparam);

    void init(const HANDLE& windowHandle);

    /* API-related things */
private:

    std::vector<TopoDS_Shape> m_shapes; //!< Shapes to visualize.

    /* OpenCascade's things */
private:

    Handle(V3d_Viewer)             m_viewer;
    Handle(V3d_View)               m_view;
    Handle(AIS_InteractiveContext) m_context;
    Handle(WNT_Window)             m_wntWindow;
    Handle(ViewerInteractor)       m_evtMgr;

    /* Lower-level things */
private:

    HINSTANCE m_hInstance; //!< Handle to the instance of the module.
    HWND      m_hWnd;      //!< Handle to the instance of the window.
    bool      m_bQuit;     //!< Indicates whether user want to quit from window.

};

