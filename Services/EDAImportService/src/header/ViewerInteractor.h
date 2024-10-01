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

// OpenCascade includes
#include <AIS_ViewController.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TCollection_AsciiString.hxx>

class AIS_InteractiveContext;
class V3d_View;

//! Manages input events.
class ViewerInteractor : public Standard_Transient, public AIS_ViewController
{
public:

    // OCCT RTTI
    DEFINE_STANDARD_RTTI_INLINE(ViewerInteractor, Standard_Transient)

public:

    //! Ctor.
    //! \param[in] view the V3d view instance.
    //! \param[in] ctx  the interactive context.
    ViewerInteractor(const Handle(V3d_View)& view,
        const Handle(AIS_InteractiveContext)& ctx);

    //! Dtor.
    virtual ~ViewerInteractor();

public:

    //! Return interactive context.
    const Handle(AIS_InteractiveContext)&
        GetContext() const { return m_ctx; }

    //! Handle mouse button press/release event.
    virtual bool UpdateMouseButtons(const Graphic3d_Vec2i& thePoint,
        Aspect_VKeyMouse       theButtons,
        Aspect_VKeyFlags       theModifiers,
        bool                   theIsEmulated) Standard_OVERRIDE;

    //! Release key.
    virtual void KeyDown(Aspect_VKey theKey,
        double theTime,
        double thePressure = 1.0) Standard_OVERRIDE;

    //! Release key.
    virtual void KeyUp(Aspect_VKey theKey,
        double theTime) Standard_OVERRIDE;

    //! Redraw the View on an Expose Event
    virtual void ProcessExpose();

    //! Handle redraw.
    virtual void handleViewRedraw(const Handle(AIS_InteractiveContext)& theCtx,
        const Handle(V3d_View)& theView) Standard_OVERRIDE;

    //! Resize View.
    virtual void ProcessConfigure();

    //! Handle KeyPress event.
    void ProcessKeyPress(Aspect_VKey theKey);

private:

    Handle(V3d_View)               m_view; //!< 3D view.
    Handle(AIS_InteractiveContext) m_ctx;  //!< Interactive context.

};
