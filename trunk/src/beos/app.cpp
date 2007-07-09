/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/app.cpp
// Purpose:     wxApp
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/08/04
// RCS-ID:      $Id: app.cpp,v 1.3 2005/01/21 21:03:59 ABX Exp $
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "app.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/gdicmn.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/cursor.h"
    #include "wx/icon.h"
    #include "wx/palette.h"
    #include "wx/dc.h"
    #include "wx/dialog.h"
    #include "wx/msgdlg.h"
    #include "wx/intl.h"
    #include "wx/dynarray.h"
    #include "wx/wxchar.h"
    #include "wx/icon.h"
    #include "wx/log.h"
#endif

#include "wx/apptrait.h"
#include "wx/filename.h"
#include "wx/module.h"
#include "wx/dynlib.h"
#include "wx/ptr_scpd.h"
#include "wx/evtloop.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

// We don't support OLE
#undef wxUSE_OLE
#define  wxUSE_OLE 0

#include <Application.h>
#include <string.h>
#include <ctype.h>

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxEventLoopPtr
// ----------------------------------------------------------------------------

// this defines wxEventLoopPtr

extern wxList WXDLLEXPORT wxPendingDelete;

// NB: all "NoRedraw" classes must have the same names as the "normal" classes
//     with NR suffix - wxWindow::MSWCreate() supposes this
const wxChar *wxCanvasClassName        = wxT("wxWindowClass");
const wxChar *wxCanvasClassNameNR      = wxT("wxWindowClassNR");
const wxChar *wxMDIFrameClassName      = wxT("wxMDIFrameClass");
const wxChar *wxMDIFrameClassNameNoRedraw = wxT("wxMDIFrameClassNR");
const wxChar *wxMDIChildFrameClassName = wxT("wxMDIChildFrameClass");
const wxChar *wxMDIChildFrameClassNameNoRedraw = wxT("wxMDIChildFrameClassNR");

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// ===========================================================================
// wxGUIAppTraits implementation
// ===========================================================================

// private class which we use to pass parameters from BeforeChildWaitLoop() to
// AfterChildWaitLoop()
struct ChildWaitLoopData
{
    ChildWaitLoopData(wxWindowDisabler *wd_, wxWindow *winActive_)
    {
        wd = wd_;
        winActive = winActive_;
    }

    wxWindowDisabler *wd;
    wxWindow *winActive;
};

// ===========================================================================
// wxApp implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxWin macros
// ---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler)

BEGIN_EVENT_TABLE(wxApp, wxEvtHandler)
    EVT_IDLE(wxApp::OnIdle)
END_EVENT_TABLE()

// class to ensure that wxAppBase::CleanUp() is called if our Initialize()
// fails
class wxCallBaseCleanup
{
public:
    wxCallBaseCleanup(wxApp *app) : m_app(app) { }
    ~wxCallBaseCleanup() { if ( m_app ) m_app->wxAppBase::CleanUp(); }

    void Dismiss() { m_app = NULL; }

private:
    wxApp *m_app;
};

//// Initialize
bool wxApp::Initialize(int& argc, wxChar **argv)
{
    if ( !wxAppBase::Initialize(argc, argv) )
        return false;
    return true;
}

// ---------------------------------------------------------------------------
// RegisterWindowClasses
// ---------------------------------------------------------------------------

// TODO we should only register classes really used by the app. For this it
//      would be enough to just delay the class registration until an attempt
//      to create a window of this class is made.

// ---------------------------------------------------------------------------
// UnregisterWindowClasses
// ---------------------------------------------------------------------------

void wxApp::CleanUp()
{
	// all objects pending for deletion must be deleted first, otherwise we
	// would crash when they use wxWinHandleHash (and UnregisterWindowClasses()
	// call wouldn't succeed as long as any windows still exist), so call the
	// base class method first and only then do our clean up
	wxAppBase::CleanUp();
	
	// for an EXE the classes are unregistered when it terminates but DLL may
	// be loaded several times (load/unload/load) into the same process in
	// which case the registration will fail after the first time if we don't
	// unregister the classes now
}

// ----------------------------------------------------------------------------
// wxApp ctor/dtor
// ----------------------------------------------------------------------------

wxApp::wxApp()
{
	m_mainLoop =  new wxEventLoop;
}

int wxApp::MainLoop()
{
	if(m_mainLoop)
		return m_mainLoop->Run();
	else 
		return 0;
}

wxApp::~wxApp()
{
}

// ----------------------------------------------------------------------------
// wxApp idle handling
// ----------------------------------------------------------------------------

void wxApp::WakeUpIdle()
{
	be_app->SetPulseRate(10000);
}

bool wxApp::OnInitGui()
{
//knorr!!
	return true;
}

void wxApp::Exit()
{
}

bool wxApp::HandlePropertyChange(WXEvent *event)
{
	return false;
}

// ----------------------------------------------------------------------------
// other wxApp event hanlders
// ----------------------------------------------------------------------------

// Default behaviour: close the application with prompts. The
// user can veto the close, and therefore the end session.

// ----------------------------------------------------------------------------
// miscellaneous
// ----------------------------------------------------------------------------

/* static */

// Yield to incoming messages

bool wxApp::Yield(bool onlyIfNeeded)
{
	return true;
}

#if wxUSE_EXCEPTIONS

// ----------------------------------------------------------------------------
// exception handling
// ----------------------------------------------------------------------------

#endif // wxUSE_EXCEPTIONS
