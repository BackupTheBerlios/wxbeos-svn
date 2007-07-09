///////////////////////////////////////////////////////////////////////////////
// Name:        beos/evtloop.cpp
// Purpose:     implements wxEventLoop for BeOS
// Author:      Anton Sokolov
// Modified by:
// Created:     04.07.01
// Copyright:   (c) 2007 Anton Sokolov <abiword@nm.ru>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "evtloop.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/evtloop.h"
#include "wx/app.h"

// ----------------------------------------------------------------------------
// wxEventLoopImpl
// ----------------------------------------------------------------------------
#include <Application.h>

class WXDLLEXPORT wxEventLoopImpl : public BApplication
{
public:
	wxEventLoopImpl();
// ctor
// set/get the exit code
	void SetExitCode(int exitcode) { m_exitcode = exitcode; }
	int GetExitCode() const { return m_exitcode; }
	virtual void ArgvReceived(int32 argc, char **argv);
	virtual void Pulse(void);
private:
// the exit code of the event loop
	int m_exitcode;
};

// ============================================================================
// wxEventLoop implementation
// ============================================================================
wxEventLoopImpl::wxEventLoopImpl()  :
	BApplication("application/wxWidgetsApp")
{
	SetExitCode(0);
	SetPulseRate(10000);
}

void wxEventLoopImpl::ArgvReceived(int32 argc, char **argv)
{
//knorr!!
//now we mut pass arguments to wxApp
	wxTheApp->Initialize((int)argc, argv);
}

void wxEventLoopImpl::Pulse()
{
//knorr!!
//idle event
	wxTheApp->ProcessIdle();
}
// ----------------------------------------------------------------------------
// wxEventLoop running and exiting
// ----------------------------------------------------------------------------

wxEventLoop *wxEventLoopBase::ms_activeLoop = NULL;

wxEventLoop::wxEventLoop()
{
	if(!be_app)
		m_impl = new wxEventLoopImpl();
}

wxEventLoop::~wxEventLoop()
{
    wxASSERT_MSG( !m_impl, _T("should have been deleted in Run()") );
}

int wxEventLoop::Run()
{
//we don't need anything here, just Run() our BApplication
    // event loops are not recursive, you need to create another loop!
	wxCHECK_MSG( !IsRunning(), -1, _T("can't reenter a message loop") );
	
	wxEventLoop *oldLoop = ms_activeLoop;
	ms_activeLoop = this;

//our BApplication
//HACK!!
	if(!m_impl) m_impl = (wxEventLoopImpl*) be_app;
	if(m_impl)
		m_impl->Run();
	else
		return 0;
	
	int exitcode = m_impl->GetExitCode();	
	ms_activeLoop = oldLoop;	
	return exitcode;
}

void wxEventLoop::Exit(int rc)
{
	wxCHECK_RET( IsRunning(), _T("can't call Exit() if not running") );
	m_impl->PostMessage(B_QUIT_REQUESTED);
	m_impl->SetExitCode(rc);
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxEventLoop::Pending() const
{
    if (wxTheApp)
    {
        // We need to remove idle callbacks or gtk_events_pending will
        // never return false.
//        wxTheApp->RemoveIdleTag();
    }

}

bool wxEventLoop::Dispatch()
{
    wxCHECK_MSG( IsRunning(), FALSE, _T("can't call Dispatch() if not running") );
    return TRUE;
}

