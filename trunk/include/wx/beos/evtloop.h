///////////////////////////////////////////////////////////////////////////////
// Name:        wx/beos/evtloop.h
// Purpose:     wxEventLoop class for BeO
// Author:      Anton Sokolov
// Modified by:
// Created:     05.01.07
// Copyleft
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BEOS_EVTLOOP_H_
#define _WX_BEOS_EVTLOOP_H_

// ----------------------------------------------------------------------------
// wxEventLoop
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxEventLoopImpl;

class WXDLLEXPORT wxEventLoop : public wxEventLoopBase
{
public:
    wxEventLoop();
	~wxEventLoop();
    // implement base class pure virtuals
    virtual int Run();
    virtual void Exit(int rc = 0);
    virtual bool Pending() const;
    virtual bool Dispatch();
	virtual bool IsRunning() const { return GetActive() == this; }

    // MSW-specific methods
    // --------------------

    // preprocess a message, return true if processed (i.e. no further
    // dispatching required)
//    virtual bool PreProcessMessage(WXMSG *msg);

    // process a single message
//    virtual void ProcessMessage(WXMSG *msg);

protected:
    // the pointer to the port specific implementation class
	wxEventLoopImpl *m_impl;

    // should we exit the loop?
    bool m_shouldExit;

    // the loop exit code
    int m_exitcode;
};

#endif // _WX_BEOS_EVTLOOP_H_

