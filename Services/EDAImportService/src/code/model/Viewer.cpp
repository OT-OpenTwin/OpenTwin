// @otlicense-ignore

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

// Own include
#include "Viewer.h"

// OpenCascade includes
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Handle.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

namespace {
    //! Adjust the style of local selection.
    //! \param[in] context the AIS context.
    void AdjustSelectionStyle(const Handle(AIS_InteractiveContext)& context)
    {
        // Initialize style for sub-shape selection.
        Handle(Prs3d_Drawer) selDrawer = new Prs3d_Drawer;
        //
        selDrawer->SetLink(context->DefaultDrawer());
        selDrawer->SetFaceBoundaryDraw(true);
        selDrawer->SetDisplayMode(1); // Shaded
        selDrawer->SetTransparency(0.9f);
        selDrawer->SetZLayer(Graphic3d_ZLayerId_Topmost);
        selDrawer->SetColor(Quantity_NOC_GOLD);
        selDrawer->SetBasicFillAreaAspect(new Graphic3d_AspectFillArea3d());

        // Adjust fill area aspect.
        const Handle(Graphic3d_AspectFillArea3d)&
            fillArea = selDrawer->BasicFillAreaAspect();
        //
        fillArea->SetInteriorColor(Quantity_NOC_GOLD);
        fillArea->SetBackInteriorColor(Quantity_NOC_GOLD);
        //
        fillArea->ChangeFrontMaterial().SetMaterialName(Graphic3d_NOM_NEON_GNC);
        fillArea->ChangeFrontMaterial().SetTransparency(0.4f);
        fillArea->ChangeBackMaterial().SetMaterialName(Graphic3d_NOM_NEON_GNC);
        fillArea->ChangeBackMaterial().SetTransparency(0.4f);

        selDrawer->UnFreeBoundaryAspect()->SetWidth(1.0);

        // Update AIS context.
        context->SetHighlightStyle(Prs3d_TypeOfHighlight_LocalSelected, selDrawer);
    }
}

//-----------------------------------------------------------------------------

Viewer::Viewer(const int left,
    const int top,
    const int width,
    const int height)
    : m_hWnd(NULL),
    m_bQuit(false)
{
    // Register the window class once
    static HINSTANCE APP_INSTANCE = NULL;
    if (APP_INSTANCE == NULL)
    {
        APP_INSTANCE = GetModuleHandleW(NULL);
        m_hInstance = APP_INSTANCE;

        WNDCLASSW WC;
        WC.cbClsExtra = 0;
        WC.cbWndExtra = 0;
        WC.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        WC.hCursor = LoadCursor(NULL, IDC_ARROW);
        WC.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        WC.hInstance = APP_INSTANCE;
        WC.lpfnWndProc = (WNDPROC)wndProcProxy;
        WC.lpszClassName = L"OpenGLClass";
        WC.lpszMenuName = 0;
        WC.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

        if (!RegisterClassW(&WC))
        {
            return;
        }
    }

    // Set coordinates for window's area rectangle.
    RECT Rect;
    SetRect(&Rect,
        left, top,
        left + width, top + height);

    // Adjust window rectangle.
    AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, false);

    // Create window.
    m_hWnd = CreateWindow(L"OpenGLClass",
        L"Quaoar >>> 3D",
        WS_OVERLAPPEDWINDOW,
        Rect.left, Rect.top, // Adjusted x, y positions
        Rect.right - Rect.left, Rect.bottom - Rect.top, // Adjusted width and height
        NULL, NULL,
        m_hInstance,
        this);

    // Check if window has been created successfully.
    if (m_hWnd == NULL)
    {
        return;
    }

    // Show window finally.
    ShowWindow(m_hWnd, TRUE);

    HANDLE windowHandle = (HANDLE)m_hWnd;

    this->init(windowHandle);
}

//-----------------------------------------------------------------------------

void Viewer::AddShape(const TopoDS_Shape& shape)
{
    m_shapes.push_back(shape);
}

//-----------------------------------------------------------------------------

//! Starts message loop.
void Viewer::StartMessageLoop()
{
    for (auto sh : m_shapes)
    {
        Handle(AIS_Shape) shape = new AIS_Shape(sh);
        m_context->Display(shape, true);
        m_context->SetDisplayMode(shape, AIS_Shaded, true);

        // Adjust selection style.
        ::AdjustSelectionStyle(m_context);

        // Activate selection modes.
        m_context->Activate(4, true); // faces
        m_context->Activate(2, true); // edges
    }

    MSG Msg;
    while (!m_bQuit)
    {
        switch (::MsgWaitForMultipleObjectsEx(0, NULL, 12, QS_ALLINPUT, 0))
        {
        case WAIT_OBJECT_0:
        {
            while (::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
            {
                if (Msg.message == WM_QUIT)
                    m_bQuit = true;// return;

                ::TranslateMessage(&Msg);
                ::DispatchMessage(&Msg);
            }
        }
        }
    }
}

//-----------------------------------------------------------------------------

void Viewer::init(const HANDLE& windowHandle)
{
    static Handle(Aspect_DisplayConnection) displayConnection;
    //
    if (displayConnection.IsNull())
        displayConnection = new Aspect_DisplayConnection();

    HWND winHandle = (HWND)windowHandle;
    //
    if (winHandle == NULL)
        return;

    // Create OCCT viewer.
    Handle(OpenGl_GraphicDriver)
        graphicDriver = new OpenGl_GraphicDriver(displayConnection, false);

    m_viewer = new V3d_Viewer(graphicDriver);

    // Lightning.
    Handle(V3d_DirectionalLight) LightDir = new V3d_DirectionalLight(V3d_Zneg, Quantity_Color(Quantity_NOC_GRAY97), 1);
    Handle(V3d_AmbientLight)     LightAmb = new V3d_AmbientLight();
    //
    LightDir->SetDirection(1.0, -2.0, -10.0);
    //
    m_viewer->AddLight(LightDir);
    m_viewer->AddLight(LightAmb);
    m_viewer->SetLightOn(LightDir);
    m_viewer->SetLightOn(LightAmb);

    // AIS context.
    m_context = new AIS_InteractiveContext(m_viewer);

    // Configure some global props.
    const Handle(Prs3d_Drawer)& contextDrawer = m_context->DefaultDrawer();
    //
    if (!contextDrawer.IsNull())
    {
        const Handle(Prs3d_ShadingAspect)& SA = contextDrawer->ShadingAspect();
        const Handle(Graphic3d_AspectFillArea3d)& FA = SA->Aspect();
        contextDrawer->SetFaceBoundaryDraw(true); // Draw edges.
        FA->SetEdgeOff();

        // Fix for inifinite lines has been reduced to 1000 from its default value 500000.
        contextDrawer->SetMaximalParameterValue(1000);
    }

    // Main view creation.
    m_view = m_viewer->CreateView();
    m_view->SetImmediateUpdate(false);

    // Event manager is constructed when both contex and view become available.
    m_evtMgr = new ViewerInteractor(m_view, m_context);

    // Aspect window creation
    m_wntWindow = new WNT_Window(winHandle);
    m_view->SetWindow(m_wntWindow, nullptr);
    //
    if (!m_wntWindow->IsMapped())
    {
        m_wntWindow->Map();
    }
    m_view->MustBeResized();

    // View settings.
    m_view->SetShadingModel(V3d_PHONG);

    // Configure rendering parameters
    Graphic3d_RenderingParams& RenderParams = m_view->ChangeRenderingParams();
    RenderParams.IsAntialiasingEnabled = true;
    RenderParams.NbMsaaSamples = 8; // Anti-aliasing by multi-sampling
    RenderParams.IsShadowEnabled = false;
    RenderParams.CollectedStats = Graphic3d_RenderingParams::PerfCounters_NONE;
}

//-----------------------------------------------------------------------------

LRESULT WINAPI Viewer::wndProcProxy(HWND   hwnd,
    UINT   message,
    WPARAM wparam,
    LPARAM lparam)
{
    if (message == WM_CREATE)
    {
        // Save pointer to our class instance (sent on window create) to window storage.
        CREATESTRUCTW* pCreateStruct = (CREATESTRUCTW*)lparam;
        SetWindowLongPtr(hwnd, int(GWLP_USERDATA), (LONG_PTR)pCreateStruct->lpCreateParams);
    }

    // Get pointer to our class instance.
    Viewer* pThis = (Viewer*)GetWindowLongPtr(hwnd, int(GWLP_USERDATA));
    return (pThis != NULL) ? pThis->wndProc(hwnd, message, wparam, lparam)
        : DefWindowProcW(hwnd, message, wparam, lparam);
}

//-----------------------------------------------------------------------------

//! Window procedure.
LRESULT Viewer::wndProc(HWND   hwnd,
    UINT   message,
    WPARAM wparam,
    LPARAM lparam)
{
    if (m_view.IsNull())
        return DefWindowProc(hwnd, message, wparam, lparam);

    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT aPaint;
        BeginPaint(m_hWnd, &aPaint);
        EndPaint(m_hWnd, &aPaint);
        m_evtMgr->ProcessExpose();
        break;
    }
    case WM_SIZE:
    {
        m_evtMgr->ProcessConfigure();
        break;
    }
    case WM_MOVE:
    case WM_MOVING:
    case WM_SIZING:
    {
        switch (m_view->RenderingParams().StereoMode)
        {
        case Graphic3d_StereoMode_RowInterlaced:
        case Graphic3d_StereoMode_ColumnInterlaced:
        case Graphic3d_StereoMode_ChessBoard:
        {
            // track window moves to reverse stereo pair
            m_view->MustBeResized();
            m_view->Update();
            break;
        }
        default:
            break;
        }
        break;
    }
    case WM_KEYUP:
    case WM_KEYDOWN:
    {
        const Aspect_VKey vkey = WNT_Window::VirtualKeyFromNative((int)wparam);
        //
        if (vkey != Aspect_VKey_UNKNOWN)
        {
            const double timeStamp = m_evtMgr->EventTime();
            if (message == WM_KEYDOWN)
            {
                m_evtMgr->KeyDown(vkey, timeStamp);
            }
            else
            {
                m_evtMgr->KeyUp(vkey, timeStamp);
            }
        }
        break;
    }
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    {
        const Graphic3d_Vec2i pos(LOWORD(lparam), HIWORD(lparam));
        const Aspect_VKeyFlags flags = WNT_Window::MouseKeyFlagsFromEvent(wparam);
        Aspect_VKeyMouse button = Aspect_VKeyMouse_NONE;
        //
        switch (message)
        {
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
            button = Aspect_VKeyMouse_LeftButton;
            break;
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
            button = Aspect_VKeyMouse_MiddleButton;
            break;
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
            button = Aspect_VKeyMouse_RightButton;
            break;
        }
        if (message == WM_LBUTTONDOWN
            || message == WM_MBUTTONDOWN
            || message == WM_RBUTTONDOWN)
        {
            SetFocus(hwnd);
            SetCapture(hwnd);

            if (!m_evtMgr.IsNull())
                m_evtMgr->PressMouseButton(pos, button, flags, false);
        }
        else
        {
            ReleaseCapture();

            if (!m_evtMgr.IsNull())
                m_evtMgr->ReleaseMouseButton(pos, button, flags, false);
        }

        m_evtMgr->FlushViewEvents(m_context, m_view, true);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        const int    delta = GET_WHEEL_DELTA_WPARAM(wparam);
        const double deltaF = double(delta) / double(WHEEL_DELTA);
        //
        const Aspect_VKeyFlags flags = WNT_Window::MouseKeyFlagsFromEvent(wparam);
        //
        Graphic3d_Vec2i pos(int(short(LOWORD(lparam))), int(short(HIWORD(lparam))));
        POINT cursorPnt = { pos.x(), pos.y() };
        if (ScreenToClient(hwnd, &cursorPnt))
        {
            pos.SetValues(cursorPnt.x, cursorPnt.y);
        }

        if (!m_evtMgr.IsNull())
        {
            m_evtMgr->UpdateMouseScroll(Aspect_ScrollDelta(pos, deltaF, flags));
            m_evtMgr->FlushViewEvents(m_context, m_view, true);
        }
        break;
    }
    case WM_MOUSEMOVE:
    {
        Graphic3d_Vec2i pos(LOWORD(lparam), HIWORD(lparam));
        Aspect_VKeyMouse buttons = WNT_Window::MouseButtonsFromEvent(wparam);
        Aspect_VKeyFlags flags = WNT_Window::MouseKeyFlagsFromEvent(wparam);

        // don't make a slide-show from input events - fetch the actual mouse cursor position
        CURSORINFO cursor;
        cursor.cbSize = sizeof(cursor);
        if (::GetCursorInfo(&cursor) != FALSE)
        {
            POINT cursorPnt = { cursor.ptScreenPos.x, cursor.ptScreenPos.y };
            if (ScreenToClient(hwnd, &cursorPnt))
            {
                // as we override mouse position, we need overriding also mouse state
                pos.SetValues(cursorPnt.x, cursorPnt.y);
                buttons = WNT_Window::MouseButtonsAsync();
                flags = WNT_Window::MouseKeyFlagsAsync();
            }
        }

        if (m_wntWindow.IsNull() || (HWND)m_wntWindow->HWindow() != hwnd)
        {
            // mouse move events come also for inactive windows
            break;
        }

        if (!m_evtMgr.IsNull())
        {
            m_evtMgr->UpdateMousePosition(pos, buttons, flags, false);
            m_evtMgr->FlushViewEvents(m_context, m_view, true);
        }
        break;
    }
    default:
    {
        break;
    }

    case WM_DESTROY:
        m_bQuit = true;
    }
    return DefWindowProc(hwnd, message, wparam, lparam);
}
