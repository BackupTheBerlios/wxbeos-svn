/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/control.cpp
// Purpose:     wxControl class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by: Wlodzimierz ABX Skiba - native implementation
// Created:     10/13/04
// RCS-ID:      $Id: control.cpp,v 1.15 2005/07/01 19:36:56 ABX Exp $
// Copyright:   (c) William Osborne, Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "control.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_CONTROLS

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/dcclient.h"
    #include "wx/log.h"
    #include "wx/settings.h"
#endif

#include "wx/control.h"
#include "wx/toplevel.h"
#include "wx/button.h"
#include "wx/checkbox.h"
#include "wx/tglbtn.h"
#include "wx/radiobut.h"
#include "wx/slider.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxControl, wxWindow)

BEGIN_EVENT_TABLE(wxControl, wxWindow)
    EVT_ERASE_BACKGROUND(wxControl::OnEraseBackground)
END_EVENT_TABLE()

// ============================================================================
// wxControl implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxControl ctor/dtor
// ----------------------------------------------------------------------------

void wxControl::Init()
{
    m_palmControl = false;
    m_palmField = false;
}

wxControl::~wxControl()
{
    SetLabel(wxEmptyString);
    m_isBeingDeleted = true;

    DestroyChildren();
}

// ----------------------------------------------------------------------------
// control window creation
// ----------------------------------------------------------------------------

bool wxControl::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& wxVALIDATOR_PARAM(validator),
                       const wxString& name)
{
    if ( !wxWindow::Create(parent, id, pos, size, style, name) )
        return false;

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    return true;
}

// ----------------------------------------------------------------------------
// various accessors
// ----------------------------------------------------------------------------

void* wxControl::GetObjectPtr() const
{
}

wxBorder wxControl::GetDefaultBorder() const
{
    // we want to automatically give controls a sunken style (confusingly,
    // it may not really mean sunken at all as we map it to WS_EX_CLIENTEDGE
    // which is not sunken at all under Windows XP -- rather, just the default)
    return wxBORDER_SUNKEN;
}

void wxControl::SetIntValue(int val)
{
}

void wxControl::SetBoolValue(bool val)
{
    SetIntValue(val?1:0);
}

bool wxControl::GetBoolValue() const
{
}

wxSize wxControl::DoGetBestSize() const
{
    return wxSize(16, 16);
}

void wxControl::DoGetPosition( int *x, int *y ) const
{
    int ox = 0, oy = 0;
    AdjustForParentClientOrigin(ox, oy);
}

void wxControl::DoGetSize( int *width, int *height ) const
{
}

void wxControl::DoMoveWindow(int x, int y, int width, int height)
{
    wxRect area = GetRect();
}

bool wxControl::Enable(bool enable)
{
}

bool wxControl::IsEnabled() const
{
}

bool wxControl::IsShown() const
{
}

bool wxControl::Show( bool show )
{
    return false;
}

void wxControl::SetFieldLabel(const wxString& label)
{
}

void wxControl::SetControlLabel(const wxString& label)
{
}

void wxControl::SetLabel(const wxString& label)
{
}

wxString wxControl::GetFieldLabel()
{
	return wxEmptyString;
}

wxString wxControl::GetControlLabel()
{
	return wxEmptyString;
}

wxString wxControl::GetLabel()
{
	return wxEmptyString;
}

/* static */ wxVisualAttributes
wxControl::GetClassDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    wxVisualAttributes attrs;

    // old school (i.e. not "common") controls use the standard dialog font
    // by default
    attrs.font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

    // most, or at least many, of the controls use the same colours as the
    // buttons -- others will have to override this (and possibly simply call
    // GetCompositeControlsDefaultAttributes() from their versions)
    attrs.colFg = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
    attrs.colBg = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);

    return attrs;
}

// another version for the "composite", i.e. non simple controls
/* static */ wxVisualAttributes
wxControl::GetCompositeControlsDefaultAttributes(wxWindowVariant WXUNUSED(variant))
{
    wxVisualAttributes attrs;
    attrs.font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    attrs.colFg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    attrs.colBg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);

    return attrs;
}

// ----------------------------------------------------------------------------
// message handling
// ----------------------------------------------------------------------------

bool wxControl::ProcessCommand(wxCommandEvent& event)
{
    return GetEventHandler()->ProcessEvent(event);
}

void wxControl::OnEraseBackground(wxEraseEvent& event)
{
}

#endif // wxUSE_CONTROLS
