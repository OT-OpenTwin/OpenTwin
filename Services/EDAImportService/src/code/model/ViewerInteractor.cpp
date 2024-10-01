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
#include "ViewerInteractor.h"

// OpenCascade includes
#include <Aspect_Grid.hxx>
#include <AIS_AnimationCamera.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <V3d_View.hxx>

//-----------------------------------------------------------------------------

ViewerInteractor::ViewerInteractor(const Handle(V3d_View)& view,
    const Handle(AIS_InteractiveContext)& ctx)
    : m_view(view),
    m_ctx(ctx)
{}

//-----------------------------------------------------------------------------

ViewerInteractor::~ViewerInteractor()
{}

//-----------------------------------------------------------------------------

bool ViewerInteractor::UpdateMouseButtons(const Graphic3d_Vec2i& point,
    Aspect_VKeyMouse       buttons,
    Aspect_VKeyFlags       modifiers,
    bool                   isEmulated)
{
    return AIS_ViewController::UpdateMouseButtons(point, buttons, modifiers, isEmulated);
}

//-----------------------------------------------------------------------------

void ViewerInteractor::ProcessExpose()
{
    if (!m_view.IsNull())
    {
        m_view->Invalidate();
        FlushViewEvents(m_ctx, m_view, true);
    }
}

//-----------------------------------------------------------------------------

void ViewerInteractor::handleViewRedraw(const Handle(AIS_InteractiveContext)& ctx,
    const Handle(V3d_View)& view)
{
    AIS_ViewController::handleViewRedraw(ctx, view);
}

//-----------------------------------------------------------------------------

void ViewerInteractor::ProcessConfigure()
{
    if (!m_view.IsNull())
    {
        m_view->MustBeResized();
        FlushViewEvents(m_ctx, m_view, true);
    }
}

//-----------------------------------------------------------------------------

void ViewerInteractor::KeyDown(Aspect_VKey key,
    double      time,
    double      pressure)
{
    AIS_ViewController::KeyDown(key, time, pressure);
}

//-----------------------------------------------------------------------------

void ViewerInteractor::KeyUp(Aspect_VKey key,
    double      time)
{
    const unsigned int modifOld = myKeys.Modifiers();
    //
    AIS_ViewController::KeyUp(key, time);
    //
    const unsigned int modifNew = myKeys.Modifiers();

    ProcessKeyPress(key | modifNew);
}

//-----------------------------------------------------------------------------

void ViewerInteractor::ProcessKeyPress(Aspect_VKey key)
{
    if (m_ctx.IsNull() || m_view.IsNull())
    {
        return;
    }

    switch (key)
    {
    case Aspect_VKey_F:
    {
        if (m_ctx->NbSelected() > 0)
        {
            m_ctx->FitSelected(m_view);
        }
        else
        {
            m_view->FitAll();
        }
        break;
    }
    case Aspect_VKey_S:
    case Aspect_VKey_W:
    {
        const int dm = (key == Aspect_VKey_S) ? AIS_Shaded : AIS_WireFrame;

        if (m_ctx->NbSelected() == 0)
        {
            m_ctx->SetDisplayMode(dm, false);
            m_ctx->UpdateCurrentViewer();
        }
        else
        {
            for (m_ctx->InitSelected(); m_ctx->MoreSelected(); m_ctx->NextSelected())
            {
                m_ctx->SetDisplayMode(m_ctx->SelectedInteractive(), dm, false);
            }
            m_ctx->UpdateCurrentViewer();
        }
        break;
    }
    case Aspect_VKey_Backspace: // Axonometry.
    {
        m_view->SetProj(V3d_XposYnegZpos);
        m_view->Redraw();
        break;
    }
    case Aspect_VKey_T:
    {
        m_view->SetProj(V3d_TypeOfOrientation_Zup_Top);
        m_view->Redraw();
        break;
    }
    case Aspect_VKey_B:
    {
        m_view->SetProj(V3d_TypeOfOrientation_Zup_Bottom);
        m_view->Redraw();
        break;
    }
    case Aspect_VKey_L:
    {
        m_view->SetProj(V3d_TypeOfOrientation_Zup_Left);
        m_view->Redraw();
        break;
    }
    case Aspect_VKey_R:
    {
        m_view->SetProj(V3d_TypeOfOrientation_Zup_Right);
        m_view->Redraw();
        break;
    }
    default: break;
    }
}
