/////////////////////////////////////////////////////////////////////////////
// Name:        windows.cpp
// Purpose:     wxWindow
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: window.cpp,v 1.89 2005/09/20 11:18:04 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "window.h"
#endif

#include "wx/setup.h"
#include "wx/menu.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/panel.h"
#include "wx/layout.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/msgdlg.h"
#include "wx/frame.h"
#include "wx/scrolwin.h"
#include "wx/scrolbar.h"
#include "wx/module.h"
#include "wx/menuitem.h"
#include "wx/log.h"
#include "wx/fontutil.h"
#include "wx/renderer.h"
#include "wx/hash.h"

#if  wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
#endif

#include "wx/beos/backview.h"
#include <string.h>

// ----------------------------------------------------------------------------
// global variables for this module
// ----------------------------------------------------------------------------

//static wxWindow* g_captureWindow = NULL;
//static GC g_eraseGC;

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

//#define event_left_is_down(x) ((x)->xbutton.state & Button1Mask)
//#define event_middle_is_down(x) ((x)->xbutton.state & Button2Mask)
//#define event_right_is_down(x) ((x)->xbutton.state & Button3Mask)

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxWindowBeOS, wxWindowBase)

BEGIN_EVENT_TABLE(wxWindowBeOS, wxWindowBase)
    EVT_SYS_COLOUR_CHANGED(wxWindowBeOS::OnSysColourChanged)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// constructors
// ----------------------------------------------------------------------------

void wxWindowBeOS::Init()
{
	// BeOS-specific
	m_mainWindow = (WXWindow) 0;
	m_clientWindow = (WXWindow) 0;
	m_insertIntoMain = FALSE;
	m_updateNcArea = FALSE;
	m_view = NULL;
	
	m_winCaptured = FALSE;
	m_needsInputFocus = FALSE;
	m_isShown = TRUE;
	m_lastTS = 0;
	m_lastButton = 0;
}

// real construction (Init() must have been called before!)
bool wxWindowBeOS::Create(wxWindow *parent, wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
{
	wxCHECK_MSG( parent, FALSE, wxT("can't create wxWindow without parent") );
	
	CreateBase(parent, id, pos, size, style, wxDefaultValidator, name);
	m_view = new BBackView(parent, pos, size, style, name);
	parent->AddChild(this);
    
//    Display *xdisplay = (Display*) wxGlobalDisplay();
//    int xscreen = DefaultScreen( xdisplay );
//    Visual *xvisual = DefaultVisual( xdisplay, xscreen );
//    Colormap cm = DefaultColormap( xdisplay, xscreen );
//
//    m_backgroundColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
//    m_backgroundColour.CalcPixel( (WXColormap) cm );
//
//    m_foregroundColour = *wxBLACK;
//    m_foregroundColour.CalcPixel( (WXColormap) cm );
//
//    Window xparent = (Window) parent->GetClientAreaWindow();
//
//    // Add window's own scrollbars to main window, not to client window
//    if (parent->GetInsertIntoMain())
//    {
//        // wxLogDebug( "Inserted into main: %s", GetName().c_str() );
//        xparent = (Window) parent->GetMainWindow();
//    }
//
//    // Size (not including the border) must be nonzero (or a Value error results)!
//    // Note: The Xlib manual doesn't mention this restriction of XCreateWindow.
//    wxSize size2(size);
//    if (size2.x <= 0)
//        size2.x = 20;
//    if (size2.y <= 0)
//        size2.y = 20;
//
//    wxPoint pos2(pos);
//    if (pos2.x == -1)
//        pos2.x = 0;
//    if (pos2.y == -1)
//        pos2.y = 0;
//
//#if wxUSE_TWO_WINDOWS
//    bool need_two_windows =
//        ((( wxSUNKEN_BORDER | wxRAISED_BORDER | wxSIMPLE_BORDER | wxHSCROLL | wxVSCROLL ) & m_windowStyle) != 0);
//#else
//    bool need_two_windows = FALSE;
//#endif
//
//#if wxUSE_NANOX
//    long xattributes = 0;
//#else
//    XSetWindowAttributes xattributes;
//    long xattributes_mask = 0;
//
//    xattributes_mask |= CWBackPixel;
//    xattributes.background_pixel = m_backgroundColour.GetPixel();
//
//    xattributes_mask |= CWBorderPixel;
//    xattributes.border_pixel = BlackPixel( xdisplay, xscreen );
//
//    xattributes_mask |= CWEventMask;
//#endif
//
//    if (need_two_windows)
//    {
//#if wxUSE_NANOX
//        long backColor, foreColor;
//        backColor = GR_RGB(m_backgroundColour.Red(), m_backgroundColour.Green(), m_backgroundColour.Blue());
//        foreColor = GR_RGB(m_foregroundColour.Red(), m_foregroundColour.Green(), m_foregroundColour.Blue());
//
//        Window xwindow = XCreateWindowWithColor( xdisplay, xparent, pos2.x, pos2.y, size2.x, size2.y,
//                                    0, 0, InputOutput, xvisual, backColor, foreColor);
//        XSelectInput( xdisplay, xwindow,
//          GR_EVENT_MASK_CLOSE_REQ | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
//          ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
//          KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
//                      PropertyChangeMask );
//
//#else
//        // Normal BeOS
//        xattributes.event_mask =
//            ExposureMask | StructureNotifyMask | ColormapChangeMask;
//
//        Window xwindow = XCreateWindow( xdisplay, xparent, pos2.x, pos2.y, size2.x, size2.y,
//            0, DefaultDepth(xdisplay,xscreen), InputOutput, xvisual, xattributes_mask, &xattributes );
//
//#endif
//
//        XSetWindowBackgroundPixmap( xdisplay, xwindow, None );
//
//        m_mainWindow = (WXWindow) xwindow;
//        wxAddWindowToTable( xwindow, (wxWindow*) this );
//
//        XMapWindow( xdisplay, xwindow );
//
//#if !wxUSE_NANOX
//        xattributes.event_mask =
//            ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
//            ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
//            KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
//            PropertyChangeMask | VisibilityChangeMask ;
//
//        if (!HasFlag( wxFULL_REPAINT_ON_RESIZE ))
//        {
//            xattributes_mask |= CWBitGravity;
//            xattributes.bit_gravity = StaticGravity;
//        }
//#endif
//
//        if (HasFlag( wxSUNKEN_BORDER) || HasFlag( wxRAISED_BORDER))
//        {
//            pos2.x = 2;
//            pos2.y = 2;
//            size2.x -= 4;
//            size2.y -= 4;
//        }
//        else if (HasFlag( wxSIMPLE_BORDER ))
//        {
//            pos2.x = 1;
//            pos2.y = 1;
//            size2.x -= 2;
//            size2.y -= 2;
//        }
//        else
//        {
//            pos2.x = 0;
//            pos2.y = 0;
//        }
//
//        // Make again sure the size is nonzero.
//        if (size2.x <= 0)
//            size2.x = 1;
//        if (size2.y <= 0)
//            size2.y = 1;
//
//#if wxUSE_NANOX
//        backColor = GR_RGB(m_backgroundColour.Red(), m_backgroundColour.Green(), m_backgroundColour.Blue());
//        foreColor = GR_RGB(m_foregroundColour.Red(), m_foregroundColour.Green(), m_foregroundColour.Blue());
//
//        xwindow = XCreateWindowWithColor( xdisplay, xwindow, pos2.x, pos2.y, size2.x, size2.y,
//                                    0, 0, InputOutput, xvisual, backColor, foreColor);
//        XSelectInput( xdisplay, xwindow,
//          GR_EVENT_MASK_CLOSE_REQ | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
//          ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
//          KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
//                      PropertyChangeMask );
//
//#else
//        xwindow = XCreateWindow( xdisplay, xwindow, pos2.x, pos2.y, size2.x, size2.y,
//            0, DefaultDepth(xdisplay,xscreen), InputOutput, xvisual, xattributes_mask, &xattributes );
//#endif
//
//        XSetWindowBackgroundPixmap( xdisplay, xwindow, None );
//
//        m_clientWindow = (WXWindow) xwindow;
//        wxAddClientWindowToTable( xwindow, (wxWindow*) this );
//
//        XMapWindow( xdisplay, xwindow );
//    }
//    else
//    {
//        // wxLogDebug( "No two windows needed %s", GetName().c_str() );
//#if wxUSE_NANOX
//        long backColor, foreColor;
//        backColor = GR_RGB(m_backgroundColour.Red(), m_backgroundColour.Green(), m_backgroundColour.Blue());
//        foreColor = GR_RGB(m_foregroundColour.Red(), m_foregroundColour.Green(), m_foregroundColour.Blue());
//
//        Window xwindow = XCreateWindowWithColor( xdisplay, xparent, pos2.x, pos2.y, size2.x, size2.y,
//                                    0, 0, InputOutput, xvisual, backColor, foreColor);
//        XSelectInput( xdisplay, xwindow,
//          GR_EVENT_MASK_CLOSE_REQ | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
//          ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
//          KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
//                      PropertyChangeMask );
//
//#else
//        xattributes.event_mask =
//            ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
//            ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask |
//            KeymapStateMask | FocusChangeMask | ColormapChangeMask | StructureNotifyMask |
//            PropertyChangeMask | VisibilityChangeMask ;
//
//        if (!HasFlag( wxFULL_REPAINT_ON_RESIZE ))
//        {
//            xattributes_mask |= CWBitGravity;
//            xattributes.bit_gravity = NorthWestGravity;
//        }
//
//        Window xwindow = XCreateWindow( xdisplay, xparent, pos2.x, pos2.y, size2.x, size2.y,
//            0, DefaultDepth(xdisplay,xscreen), InputOutput, xvisual, xattributes_mask, &xattributes );
//#endif
//
//        XSetWindowBackgroundPixmap( xdisplay, xwindow, None );
//
//        m_mainWindow = (WXWindow) xwindow;
//        m_clientWindow = m_mainWindow;
//        wxAddWindowToTable( xwindow, (wxWindow*) this );
//
//        XMapWindow( xdisplay, xwindow );
//    }
//
//    // Is a subwindow, so map immediately
//    m_isShown = TRUE;
//
//    // Without this, the cursor may not be restored properly (e.g. in splitter
//    // sample).
//    SetCursor(*wxSTANDARD_CURSOR);
//    SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
//
//    // Don't call this, it can have nasty repercussions for composite controls,
//    // for example
//    //    SetSize(pos.x, pos.y, size.x, size.y);

    return false;
}

// Destructor
wxWindowBeOS::~wxWindowBeOS()
{
	SendDestroyEvent();
	m_isBeingDeleted = TRUE;
	DestroyChildren();
	m_eventHandler = NULL;
}

// ---------------------------------------------------------------------------
// basic operations
// ---------------------------------------------------------------------------

void wxWindowBeOS::SetFocus()
{
//    Window xwindow = (Window) m_clientWindow;

//    wxCHECK_RET( xwindow, wxT("invalid window") );

    // Don't assert; we might be trying to set the focus for a panel
    // with only static controls, so the panel returns false from AcceptsFocus.
    // The app should be not be expected to deal with this.
    if (!AcceptsFocus())
        return;

#if 0
    if (GetName() == "scrollBar")
    {
        char *crash = NULL;
        *crash = 0;
    }
#endif

//    if (wxWindowIsVisible(xwindow))
//    {
//        wxLogTrace( _T("focus"), _T("wxWindowBeOS::SetFocus: %s"), GetClassInfo()->GetClassName());
//        //        XSetInputFocus( wxGlobalDisplay(), xwindow, RevertToParent, CurrentTime );
////        XSetInputFocus( wxGlobalDisplay(), xwindow, RevertToNone, CurrentTime );
//        m_needsInputFocus = FALSE;
//    }
//    else
//    {
//        m_needsInputFocus = TRUE;
//    }
}

// Get the window with the focus
wxWindow *wxWindowBase::DoFindFocus()
{
//    Window xfocus = (Window) 0;
//    int revert = 0;
//
//    XGetInputFocus( wxGlobalDisplay(), &xfocus, &revert);
//    if (xfocus)
//    {
//        wxWindow *win = wxGetWindowFromTable( xfocus );
//        if (!win)
//        {
//            win = wxGetClientWindowFromTable( xfocus );
//        }
//
//        return win;
//    }

    return NULL;
}

// Enabling/disabling handled by event loop, and not sending events
// if disabled.
bool wxWindowBeOS::Enable(bool enable)
{
    if ( !wxWindowBase::Enable(enable) )
        return FALSE;

    return TRUE;
}

bool wxWindowBeOS::Show(bool show)
{
    wxWindowBase::Show(show);

//    Window xwindow = (Window) m_mainWindow;
//    Display *xdisp = wxGlobalDisplay();
//    if (show)
//    {
//        // wxLogDebug( "Mapping window of type %s", GetName().c_str() );
//        XMapWindow(xdisp, xwindow);
//    }
//    else
//    {
//        // wxLogDebug( "Unmapping window of type %s", GetName().c_str() );
//        XUnmapWindow(xdisp, xwindow);
//    }

    return TRUE;
}

// Raise the window to the top of the Z order
void wxWindowBeOS::Raise()
{
//    if (m_mainWindow)
//        XRaiseWindow( wxGlobalDisplay(), (Window) m_mainWindow );
}

// Lower the window to the bottom of the Z order
void wxWindowBeOS::Lower()
{
//    if (m_mainWindow)
//        XLowerWindow( wxGlobalDisplay(), (Window) m_mainWindow );
}

void wxWindowBeOS::DoCaptureMouse()
{
//    if ((g_captureWindow != NULL) && (g_captureWindow != this))
//    {
//        wxASSERT_MSG(FALSE, wxT("Trying to capture before mouse released."));
//
//        // Core dump now
//        int *tmp = NULL;
//        (*tmp) = 1;
//        return;
//    }
//
//    if (m_winCaptured)
//        return;
//
//    Window xwindow = (Window) m_clientWindow;
//
//    wxCHECK_RET( xwindow, wxT("invalid window") );
//
//    g_captureWindow = (wxWindow*) this;
//
//    if (xwindow)
//    {
//        int res = XGrabPointer(wxGlobalDisplay(), xwindow,
//            FALSE,
//            ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask,
//            GrabModeAsync,
//            GrabModeAsync,
//            None,
//            None, /* cursor */ // TODO: This may need to be set to the cursor of this window
//            CurrentTime );
//
//        if (res != GrabSuccess)
//        {
//            wxString msg;
//            msg.Printf(wxT("Failed to grab pointer for window %s"), this->GetClassInfo()->GetClassName());
//            wxLogDebug(msg);
//            if (res == GrabNotViewable)
//                wxLogDebug( wxT("This is not a viewable window - perhaps not shown yet?") );
//
//            g_captureWindow = NULL;
//            return;
//        }
//
//        m_winCaptured = TRUE;
//    }
}

void wxWindowBeOS::DoReleaseMouse()
{
//    g_captureWindow = NULL;

    if ( !m_winCaptured )
        return;

//    Window xwindow = (Window) m_clientWindow;
//
//    if (xwindow)
//    {
//        XUngrabPointer( wxGlobalDisplay(), CurrentTime );
//    }
//
//    // wxLogDebug( "Ungrabbed pointer in %s", GetName().c_str() );

    m_winCaptured = FALSE;
}

bool wxWindowBeOS::SetFont(const wxFont& font)
{
    if ( !wxWindowBase::SetFont(font) )
    {
        // nothing to do
        return FALSE;
    }

    return TRUE;
}

bool wxWindowBeOS::SetCursor(const wxCursor& cursor)
{
    if ( !wxWindowBase::SetCursor(cursor) )
    {
        // no change
        return FALSE;
    }

//    Window xwindow = (Window) m_clientWindow;
//
//    wxCHECK_MSG( xwindow, FALSE, wxT("invalid window") );
//
//    wxCursor cursorToUse;
//    if (m_cursor.Ok())
//        cursorToUse = m_cursor;
//    else
//        cursorToUse = *wxSTANDARD_CURSOR;
//
//    Cursor xcursor = (Cursor) cursorToUse.GetCursor();
//
//    XDefineCursor( wxGlobalDisplay(), xwindow, xcursor );

    return TRUE;
}

// Coordinates relative to the window
void wxWindowBeOS::WarpPointer (int x, int y)
{
//    Window xwindow = (Window) m_clientWindow;
//
//    wxCHECK_RET( xwindow, wxT("invalid window") );
//
//    XWarpPointer( wxGlobalDisplay(), None, xwindow, 0, 0, 0, 0, x, y);
}

// Does a physical scroll
void wxWindowBeOS::ScrollWindow(int dx, int dy, const wxRect *rect)
{
    // No scrolling requested.
    if ((dx == 0) && (dy == 0)) return;

    if (!m_updateRegion.IsEmpty())
    {
        m_updateRegion.Offset( dx, dy );

        int cw = 0;
        int ch = 0;
        GetSize( &cw, &ch );  // GetClientSize() ??
        m_updateRegion.Intersect( 0, 0, cw, ch );
    }

    if (!m_clearRegion.IsEmpty())
    {
        m_clearRegion.Offset( dx, dy );

        int cw = 0;
        int ch = 0;
        GetSize( &cw, &ch );  // GetClientSize() ??
        m_clearRegion.Intersect( 0, 0, cw, ch );
    }

//    Window xwindow = (Window) GetClientAreaWindow();
//
//    wxCHECK_RET( xwindow, wxT("invalid window") );
//
//    Display *xdisplay = wxGlobalDisplay();
//
//    GC xgc = XCreateGC( xdisplay, xwindow, 0, NULL );
//    XSetGraphicsExposures( xdisplay, xgc, True );
//
//    int s_x = 0;
//    int s_y = 0;
//    int cw;
//    int ch;
//    if (rect)
//    {
//        s_x = rect->x;
//        s_y = rect->y;
//
//        cw = rect->width;
//        ch = rect->height;
//    }
//    else
//    {
//        s_x = 0;
//        s_y = 0;
//        GetClientSize( &cw, &ch );
//    }
//
//#if wxUSE_TWO_WINDOWS
//    wxPoint offset( 0,0 );
//#else
//    wxPoint offset = GetClientAreaOrigin();
//    s_x += offset.x;
//    s_y += offset.y;
//#endif
//
//    int w = cw - abs(dx);
//    int h = ch - abs(dy);
//
//    if ((h < 0) || (w < 0))
//    {
//        Refresh();
//    }
//    else
//    {
//        wxRect rect;
//        if (dx < 0) rect.x = cw+dx + offset.x; else rect.x = s_x;
//        if (dy < 0) rect.y = ch+dy + offset.y; else rect.y = s_y;
//        if (dy != 0) rect.width = cw; else rect.width = abs(dx);
//        if (dx != 0) rect.height = ch; else rect.height = abs(dy);
//
//        int d_x = s_x;
//        int d_y = s_y;
//
//        if (dx < 0) s_x += -dx;
//        if (dy < 0) s_y += -dy;
//        if (dx > 0) d_x = dx + offset.x;
//        if (dy > 0) d_y = dy + offset.y;
//
//        XCopyArea( xdisplay, xwindow, xwindow, xgc, s_x, s_y, w, h, d_x, d_y );
//
//        // wxLogDebug( "Copy: s_x %d s_y %d w %d h %d d_x %d d_y %d", s_x, s_y, w, h, d_x, d_y );
//
//        // wxLogDebug( "Update: %d %d %d %d", rect.x, rect.y, rect.width, rect.height );
//
//        m_updateRegion.Union( rect );
//        m_clearRegion.Union( rect );
//    }
//
//    XFreeGC( xdisplay, xgc );
//
//    // Move Clients, but not the scrollbars
//    // FIXME: There may be a better method to move a lot of Windows within BeOS
//    wxScrollBar *sbH = ((wxWindow *) this)->GetScrollbar( wxHORIZONTAL );
//    wxScrollBar *sbV = ((wxWindow *) this)->GetScrollbar( wxVERTICAL );
//    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
//    while ( node )
//    {
//        // Only propagate to non-top-level windows
//        wxWindow *win = node->GetData();
//        if ( win->GetParent() && win != sbH && win != sbV )
//        {
//            wxPoint pos = win->GetPosition();
//            // Add the delta to the old Position
//            pos.x += dx;
//            pos.y += dy;
//            win->SetPosition(pos);
//        }
//        node = node->GetNext();
//    }
}

// ---------------------------------------------------------------------------
// drag and drop
// ---------------------------------------------------------------------------

#if wxUSE_DRAG_AND_DROP

void wxWindowBeOS::SetDropTarget(wxDropTarget * WXUNUSED(pDropTarget))
{
    // TODO
}

#endif

// Old style file-manager drag&drop
void wxWindowBeOS::DragAcceptFiles(bool WXUNUSED(accept))
{
    // TODO
}

// ----------------------------------------------------------------------------
// tooltips
// ----------------------------------------------------------------------------

#if wxUSE_TOOLTIPS

void wxWindowBeOS::DoSetToolTip(wxToolTip * WXUNUSED(tooltip))
{
    // TODO
}

#endif // wxUSE_TOOLTIPS

// ---------------------------------------------------------------------------
// moving and resizing
// ---------------------------------------------------------------------------

bool wxWindowBeOS::PreResize()
{
    return TRUE;
}

// Get total size
void wxWindowBeOS::DoGetSize(int *x, int *y) const
{
	if (m_view)
	{
		BRect bounds = m_view->bounds();
		*x = bounds.IntegerWidth();
		*y = bounds.IntegerHeight();
	}
}

void wxWindowBeOS::DoGetPosition(int *x, int *y) const
{
//    Window window = (Window) m_mainWindow;
//    if (window)
//    {
//        //XSync(wxGlobalDisplay(), False);
//        XWindowAttributes attr;
//        Status status = XGetWindowAttributes(wxGlobalDisplay(), window, & attr);
//        wxASSERT(status);
//
//        if (status)
//        {
//            *x = attr.x;
//            *y = attr.y;
//
//            // We may be faking the client origin. So a window that's really at (0, 30)
//            // may appear (to wxWin apps) to be at (0, 0).
//            if (GetParent())
//            {
//                wxPoint pt(GetParent()->GetClientAreaOrigin());
//                *x -= pt.x;
//                *y -= pt.y;
//            }
//        }
//    }
}

void wxWindowBeOS::DoScreenToClient(int *x, int *y) const
{
//    Display *display = wxGlobalDisplay();
//    Window rootWindow = RootWindowOfScreen(DefaultScreenOfDisplay(display));
//    Window thisWindow = (Window) m_clientWindow;
//
//    Window childWindow;
//    int xx = *x;
//    int yy = *y;
//    XTranslateCoordinates(display, rootWindow, thisWindow, xx, yy, x, y, &childWindow);
}

void wxWindowBeOS::DoClientToScreen(int *x, int *y) const
{
//    Display *display = wxGlobalDisplay();
//    Window rootWindow = RootWindowOfScreen(DefaultScreenOfDisplay(display));
//    Window thisWindow = (Window) m_clientWindow;
//
//    Window childWindow;
//    int xx = *x;
//    int yy = *y;
//    XTranslateCoordinates(display, thisWindow, rootWindow, xx, yy, x, y, &childWindow);
}


// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindowBeOS::DoGetClientSize(int *x, int *y) const
{
//    Window window = (Window) m_mainWindow;
//
//    if (window)
//    {
//        XWindowAttributes attr;
//        Status status = XGetWindowAttributes( wxGlobalDisplay(), window, &attr );
//        wxASSERT(status);
//
//        if (status)
//        {
//            *x = attr.width ;
//            *y = attr.height ;
//        }
//    }
}

void wxWindowBeOS::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
	wxLogDebug("DoSetSize: %s (%ld) %d, %d %dx%d", GetClassInfo()->GetClassName(), GetId(), x, y, width, height);

	BRect bounds;
	if(m_view) 
		bounds = m_view->bounds();
	else
		bounds = BRect(0, 0, 0, 0);

	int new_x = (int)bounds.left;
	int new_y = (int)bounds.top;
	int new_w = bounds.IntegerWidth();
	int new_h = bounds.IntegerHeight();

	if (x != -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
	{
		int yy = 0;
		AdjustForParentClientOrigin( x, yy, sizeFlags);
		new_x = x;
	}
	if (y != -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
	{
		int xx = 0;
		AdjustForParentClientOrigin( xx, y, sizeFlags);
		new_y = y;
	}
	if (width != -1)
	{
		new_w = width;
		if (new_w <= 0)
			new_w = 20;
	}
	if (height != -1)
	{
		new_h = height;
		if (new_h <= 0)
			new_h = 20;
	}
	
	DoMoveWindow( new_x, new_y, new_w, new_h );
}

void wxWindowBeOS::DoSetClientSize(int width, int height)
{
}

void wxWindowBeOS::DoMoveWindow(int x, int y, int width, int height)
{
	if(m_view)
	{
		m_view->MoveTo(x,y);
		m_view->ResizeTo(width, height);
	}
}

void wxWindowBeOS::DoSetSizeHints(int minW, int minH, int maxW, int maxH, int incW, int incH)
{
}

// ---------------------------------------------------------------------------
// text metrics
// ---------------------------------------------------------------------------

int wxWindowBeOS::GetCharHeight() const
{
//    wxCHECK_MSG( m_font.Ok(), 0, wxT("valid window font needed") );
//
//#if wxUSE_UNICODE
//    // There should be an easier way.
//    PangoLayout *layout = pango_layout_new( wxTheApp->GetPangoContext() );
//    pango_layout_set_font_description( layout, GetFont().GetNativeFontInfo()->description );
//    pango_layout_set_text(layout, "H", 1 );
//    int w,h;
//    pango_layout_get_pixel_size(layout, &w, &h);
//    g_object_unref( G_OBJECT( layout ) );
//
//    return h;
//#else
//    WXFontStructPtr pFontStruct = m_font.GetFontStruct(1.0, wxGlobalDisplay());
//
//    int direction, ascent, descent;
//    XCharStruct overall;
//    XTextExtents ((XFontStruct*) pFontStruct, "x", 1, &direction, &ascent,
//        &descent, &overall);
//
//    //  return (overall.ascent + overall.descent);
//    return (ascent + descent);
//#endif
	return 0;
}

int wxWindowBeOS::GetCharWidth() const
{
    wxCHECK_MSG( m_font.Ok(), 0, wxT("valid window font needed") );

#if wxUSE_UNICODE
    // There should be an easier way.
    PangoLayout *layout = pango_layout_new( wxTheApp->GetPangoContext() );
    pango_layout_set_font_description( layout, GetFont().GetNativeFontInfo()->description );
    pango_layout_set_text(layout, "H", 1 );
    int w,h;
    pango_layout_get_pixel_size(layout, &w, &h);
    g_object_unref( G_OBJECT( layout ) );

    return w;
#else
//    WXFontStructPtr pFontStruct = m_font.GetFontStruct(1.0, wxGlobalDisplay());
//
//    int direction, ascent, descent;
//    XCharStruct overall;
//    XTextExtents ((XFontStruct*) pFontStruct, "x", 1, &direction, &ascent,
//        &descent, &overall);
//
//    return overall.width;
#endif
	return 0;
}

void wxWindowBeOS::GetTextExtent(const wxString& string,
                             int *x, int *y,
                             int *descent, int *externalLeading,
                             const wxFont *theFont) const
{
//    wxFont fontToUse = m_font;
//    if (theFont) fontToUse = *theFont;
//
//    wxCHECK_RET( fontToUse.Ok(), wxT("invalid font") );
//
//    if (string.IsEmpty())
//    {
//        if (x) (*x) = 0;
//        if (y) (*y) = 0;
//        return;
//    }
//
//#if wxUSE_UNICODE
//    PangoLayout *layout = pango_layout_new( wxTheApp->GetPangoContext() );
//
//    PangoFontDescription *desc = fontToUse.GetNativeFontInfo()->description;
//    pango_layout_set_font_description(layout, desc);
//
//    const wxCharBuffer data = wxConvUTF8.cWC2MB( string );
//    pango_layout_set_text(layout, (const char*) data, strlen( (const char*) data ));
//
//    PangoLayoutLine *line = (PangoLayoutLine *)pango_layout_get_lines(layout)->data;
//
//
//    PangoRectangle rect;
//    pango_layout_line_get_extents(line, NULL, &rect);
//
//    if (x) (*x) = (wxCoord) (rect.width / PANGO_SCALE);
//    if (y) (*y) = (wxCoord) (rect.height / PANGO_SCALE);
//    if (descent)
//    {
//        // Do something about metrics here
//        (*descent) = 0;
//    }
//    if (externalLeading) (*externalLeading) = 0;  // ??
//
//    g_object_unref( G_OBJECT( layout ) );
//#else
//    WXFontStructPtr pFontStruct = fontToUse.GetFontStruct(1.0, wxGlobalDisplay());
//
//    int direction, ascent, descent2;
//    XCharStruct overall;
//    int slen = string.Len();
//
//    XTextExtents((XFontStruct*) pFontStruct, (char*) string.c_str(), slen,
//                 &direction, &ascent, &descent2, &overall);
//
//    if ( x )
//        *x = (overall.width);
//    if ( y )
//        *y = (ascent + descent2);
//    if (descent)
//        *descent = descent2;
//    if (externalLeading)
//        *externalLeading = 0;
//#endif
}

// ----------------------------------------------------------------------------
// painting
// ----------------------------------------------------------------------------

void wxWindowBeOS::Refresh(bool eraseBack, const wxRect *rect)
{
    if (eraseBack)
    {
        if (rect)
        {
            // Schedule for later Updating in ::Update() or ::OnInternalIdle().
            m_clearRegion.Union( rect->x, rect->y, rect->width, rect->height );
        }
        else
        {
            int height,width;
            GetSize( &width, &height );

            // Schedule for later Updating in ::Update() or ::OnInternalIdle().
            m_clearRegion.Clear();
            m_clearRegion.Union( 0, 0, width, height );
        }
    }

    if (rect)
    {
         // Schedule for later Updating in ::Update() or ::OnInternalIdle().
         m_updateRegion.Union( rect->x, rect->y, rect->width, rect->height );
    }
    else
    {
        int height,width;
        GetSize( &width, &height );

        // Schedule for later Updating in ::Update() or ::OnInternalIdle().
        m_updateRegion.Clear();
        m_updateRegion.Union( 0, 0, width, height );
    }
}

void wxWindowBeOS::Update()
{
    if (m_updateNcArea)
    {
        // wxLogDebug("wxWindowBeOS::UpdateNC: %s", GetClassInfo()->GetClassName());
        // Send nc paint events.
        SendNcPaintEvents();
    }

    if (!m_updateRegion.IsEmpty())
    {
        // wxLogDebug("wxWindowBeOS::Update: %s", GetClassInfo()->GetClassName());
        // Actually send erase events.
        SendEraseEvents();

        // Actually send paint events.
        SendPaintEvents();
    }
}

void wxWindowBeOS::SendEraseEvents()
{
    if (m_clearRegion.IsEmpty()) return;

//    wxClientDC dc( (wxWindow*)this );
//    dc.SetClippingRegion( m_clearRegion );
//
//    wxEraseEvent erase_event( GetId(), &dc );
//    erase_event.SetEventObject( this );
//
//    if (!GetEventHandler()->ProcessEvent(erase_event) )
//    {
//        Display *xdisplay = wxGlobalDisplay();
//        Window xwindow = (Window) GetClientAreaWindow();
//        XSetForeground( xdisplay, g_eraseGC, m_backgroundColour.GetPixel() );
//
//        wxRegionIterator upd( m_clearRegion );
//        while (upd)
//        {
//            XFillRectangle( xdisplay, xwindow, g_eraseGC,
//                            upd.GetX(), upd.GetY(), upd.GetWidth(), upd.GetHeight() );
//            upd ++;
//        }
//    }
//
//    m_clearRegion.Clear();
}

void wxWindowBeOS::SendPaintEvents()
{
    //    wxLogDebug("SendPaintEvents: %s (%ld)", GetClassInfo()->GetClassName(), GetId());

//    m_clipPaintRegion = TRUE;
//
//    wxPaintEvent paint_event( GetId() );
//    paint_event.SetEventObject( this );
//    GetEventHandler()->ProcessEvent( paint_event );
//
//    m_updateRegion.Clear();
//
//    m_clipPaintRegion = FALSE;
}

void wxWindowBeOS::SendNcPaintEvents()
{
//    wxWindow *window = (wxWindow*) this;
//
//    // All this for drawing the small square between the scrollbars.
//    int width = 0;
//    int height = 0;
//    int x = 0;
//    int y = 0;
//    wxScrollBar *sb = window->GetScrollbar( wxHORIZONTAL );
//    if (sb && sb->IsShown())
//    {
//        height = sb->GetSize().y;
//        y = sb->GetPosition().y;
//
//        sb = window->GetScrollbar( wxVERTICAL );
//        if (sb && sb->IsShown())
//        {
//            width = sb->GetSize().x;
//            x = sb->GetPosition().x;
//
//            Display *xdisplay = wxGlobalDisplay();
//            Window xwindow = (Window) GetMainWindow();
//            Colormap cm = (Colormap) wxTheApp->GetMainColormap( wxGetDisplay() );
//            wxColour colour = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
//            colour.CalcPixel( (WXColormap) cm );
//
//            XSetForeground( xdisplay, g_eraseGC, colour.GetPixel() );
//
//            XFillRectangle( xdisplay, xwindow, g_eraseGC, x, y, width, height );
//        }
//    }
//
//    wxNcPaintEvent nc_paint_event( GetId() );
//    nc_paint_event.SetEventObject( this );
//    GetEventHandler()->ProcessEvent( nc_paint_event );
//
//    m_updateNcArea = FALSE;
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

// Responds to colour changes: passes event on to children.
void wxWindowBeOS::OnSysColourChanged(wxSysColourChangedEvent& event)
{
//    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
//    while ( node )
//    {
//        // Only propagate to non-top-level windows
//        wxWindow *win = node->GetData();
//        if ( win->GetParent() )
//        {
//            wxSysColourChangedEvent event2;
//            event.SetEventObject(win);
//            win->GetEventHandler()->ProcessEvent(event2);
//        }
//
//        node = node->GetNext();
//    }
}

// See handler for InFocus case in app.cpp for details.
wxWindow* g_GettingFocus = NULL;

void wxWindowBeOS::OnInternalIdle()
{
    // Update invalidated regions.
    Update();

    // This calls the UI-update mechanism (querying windows for
    // menu/toolbar/control state information)
    if (wxUpdateUIEvent::CanUpdate((wxWindow*) this))
        UpdateWindowUI(wxUPDATE_UI_FROMIDLE);

    // Set the input focus if couldn't do it before
    if (m_needsInputFocus)
    {
#if 0
        wxString msg;
        msg.Printf("Setting focus for %s from OnInternalIdle\n", GetClassInfo()->GetClassName());
        printf(msg.c_str());
#endif
        SetFocus();

        // If it couldn't set the focus now, there's
        // no point in trying again.
        m_needsInputFocus = FALSE;
    }
    g_GettingFocus = NULL;
}

// ----------------------------------------------------------------------------
// function which maintain the global hash table mapping Widgets to wxWidgets
// ----------------------------------------------------------------------------

//static bool DoAddWindowToTable(wxWindowHash *hash, Window w, wxWindow *win)
//{
//    if ( !hash->insert(wxWindowHash::value_type(w, win)).second )
//    {
//        wxLogDebug( wxT("Widget table clash: new widget is 0x%08x, %s"),
//                    (unsigned int)w, win->GetClassInfo()->GetClassName());
//        return FALSE;
//    }
//
//    wxLogTrace( wxT("widget"), wxT("XWindow 0x%08x <-> window %p (%s)"),
//                (unsigned int) w, win, win->GetClassInfo()->GetClassName());
//
//    return TRUE;
//}

//static inline wxWindow *DoGetWindowFromTable(wxWindowHash *hash, Window w)
//{
//    wxWindowHash::iterator i = hash->find(w);
//    return i == hash->end() ? NULL : i->second;
//}
//
//static inline void DoDeleteWindowFromTable(wxWindowHash *hash, Window w)
//{
//    wxLogTrace( wxT("widget"), wxT("XWindow 0x%08x deleted"), (unsigned int) w);
//
//    hash->erase(w);
//}

// ----------------------------------------------------------------------------
// public wrappers
// ----------------------------------------------------------------------------

//bool wxAddWindowToTable(Window w, wxWindow *win)
//{
//    return DoAddWindowToTable(wxWidgetHashTable, w, win);
//}
//
//wxWindow *wxGetWindowFromTable(Window w)
//{
//    return DoGetWindowFromTable(wxWidgetHashTable, w);
//}
//
//void wxDeleteWindowFromTable(Window w)
//{
//    DoDeleteWindowFromTable(wxWidgetHashTable, w);
//}
//
//bool wxAddClientWindowToTable(Window w, wxWindow *win)
//{
//    return DoAddWindowToTable(wxClientWidgetHashTable, w, win);
//}
//
//wxWindow *wxGetClientWindowFromTable(Window w)
//{
//    return DoGetWindowFromTable(wxClientWidgetHashTable, w);
//}
//
//void wxDeleteClientWindowFromTable(Window w)
//{
//    DoDeleteWindowFromTable(wxClientWidgetHashTable, w);
//}

// ----------------------------------------------------------------------------
// BeOS-specific accessors
// ----------------------------------------------------------------------------

WXWindow wxWindowBeOS::GetMainWindow() const
{
    return m_mainWindow;
}

WXWindow wxWindowBeOS::GetClientAreaWindow() const
{
    return m_clientWindow;
}

// ----------------------------------------------------------------------------
// TranslateXXXEvent() functions
// ----------------------------------------------------------------------------

//knorr change XEvent with BMessage
bool wxTranslateMouseEvent(wxMouseEvent& wxevent, wxWindow *win/*, Window window, XEvent *xevent*/)
{
//    switch (XEventGetType(xevent))
//    {
//        case EnterNotify:
//        case LeaveNotify:
//        case ButtonPress:
//        case ButtonRelease:
//        case MotionNotify:
//        {
//            wxEventType eventType = wxEVT_NULL;
//
//            if (XEventGetType(xevent) == EnterNotify)
//            {
//                //if (local_event.xcrossing.mode!=NotifyNormal)
//                //  return ; // Ignore grab events
//                eventType = wxEVT_ENTER_WINDOW;
//                //            canvas->GetEventHandler()->OnSetFocus();
//            }
//            else if (XEventGetType(xevent) == LeaveNotify)
//            {
//                //if (local_event.xcrossingr.mode!=NotifyNormal)
//                //  return ; // Ignore grab events
//                eventType = wxEVT_LEAVE_WINDOW;
//                //            canvas->GetEventHandler()->OnKillFocus();
//            }
//            else if (XEventGetType(xevent) == MotionNotify)
//            {
//                eventType = wxEVT_MOTION;
//            }
//            else if (XEventGetType(xevent) == ButtonPress)
//            {
//                wxevent.SetTimestamp(XButtonEventGetTime(xevent));
//                int button = 0;
//                if (XButtonEventLChanged(xevent))
//                {
//                    eventType = wxEVT_LEFT_DOWN;
//                    button = 1;
//                }
//                else if (XButtonEventMChanged(xevent))
//                {
//                    eventType = wxEVT_MIDDLE_DOWN;
//                    button = 2;
//                }
//                else if (XButtonEventRChanged(xevent))
//                {
//                    eventType = wxEVT_RIGHT_DOWN;
//                    button = 3;
//                }
//
//                // check for a double click
//                // TODO: where can we get this value from?
//                //long dclickTime = XtGetMultiClickTime(wxGlobalDisplay());
//                long dclickTime = 200;
//                long ts = wxevent.GetTimestamp();
//
//                int buttonLast = win->GetLastClickedButton();
//                long lastTS = win->GetLastClickTime();
//                if ( buttonLast && buttonLast == button && (ts - lastTS) < dclickTime )
//                {
//                    // I have a dclick
//                    win->SetLastClick(0, ts);
//                    if ( eventType == wxEVT_LEFT_DOWN )
//                        eventType = wxEVT_LEFT_DCLICK;
//                    else if ( eventType == wxEVT_MIDDLE_DOWN )
//                        eventType = wxEVT_MIDDLE_DCLICK;
//                    else if ( eventType == wxEVT_RIGHT_DOWN )
//                        eventType = wxEVT_RIGHT_DCLICK;
//                }
//                else
//                {
//                    // not fast enough or different button
//                    win->SetLastClick(button, ts);
//                }
//            }
//            else if (XEventGetType(xevent) == ButtonRelease)
//            {
//                if (XButtonEventLChanged(xevent))
//                {
//                    eventType = wxEVT_LEFT_UP;
//                }
//                else if (XButtonEventMChanged(xevent))
//                {
//                    eventType = wxEVT_MIDDLE_UP;
//                }
//                else if (XButtonEventRChanged(xevent))
//                {
//                    eventType = wxEVT_RIGHT_UP;
//                }
//                else return FALSE;
//            }
//            else
//            {
//                return FALSE;
//            }
//
//            wxevent.SetEventType(eventType);
//
//            wxevent.m_x = XButtonEventGetX(xevent);
//            wxevent.m_y = XButtonEventGetY(xevent);
//
//            wxevent.m_leftDown = ((eventType == wxEVT_LEFT_DOWN)
//                || (XButtonEventLIsDown(xevent)
//                && (eventType != wxEVT_LEFT_UP)));
//            wxevent.m_middleDown = ((eventType == wxEVT_MIDDLE_DOWN)
//                || (XButtonEventMIsDown(xevent)
//                && (eventType != wxEVT_MIDDLE_UP)));
//            wxevent.m_rightDown = ((eventType == wxEVT_RIGHT_DOWN)
//                || (XButtonEventRIsDown (xevent)
//                && (eventType != wxEVT_RIGHT_UP)));
//
//            wxevent.m_shiftDown = XButtonEventShiftIsDown(xevent);
//            wxevent.m_controlDown = XButtonEventCtrlIsDown(xevent);
//            wxevent.m_altDown = XButtonEventAltIsDown(xevent);
//            wxevent.m_metaDown = XButtonEventMetaIsDown(xevent);
//
//            wxevent.SetId(win->GetId());
//            wxevent.SetEventObject(win);
//
//            return TRUE;
//        }
//    }
    return FALSE;
}

bool wxTranslateKeyEvent(wxKeyEvent& wxevent, wxWindow *win/*, Window WXUNUSED(win), XEvent *xevent*/, bool isAscii)
{
//    switch (XEventGetType(xevent))
//    {
//    case KeyPress:
//    case KeyRelease:
//        {
//            char buf[20];
//
//            KeySym keySym;
//            (void) XLookupString ((XKeyEvent *) xevent, buf, 20, &keySym, NULL);
//            int id = wxCharCodeXToWX (keySym);
//            // id may be WXK_xxx code - these are outside ASCII range, so we
//            // can't just use toupper() on id.
//            // Only change this if we want the raw key that was pressed,
//            // and don't change it if we want an ASCII value.
//            if (!isAscii && (id >= 'a' && id <= 'z'))
//            {
//                id = id + 'A' - 'a';
//            }
//
//            wxevent.m_shiftDown = XKeyEventShiftIsDown(xevent);
//            wxevent.m_controlDown = XKeyEventCtrlIsDown(xevent);
//            wxevent.m_altDown = XKeyEventAltIsDown(xevent);
//            wxevent.m_metaDown = XKeyEventMetaIsDown(xevent);
//            wxevent.SetEventObject(win);
//            wxevent.m_keyCode = id;
//            wxevent.SetTimestamp(XKeyEventGetTime(xevent));
//
//            wxevent.m_x = XKeyEventGetX(xevent);
//            wxevent.m_y = XKeyEventGetY(xevent);
//
//            return id > -1;
//        }
//    default:
//        break;
//    }
    return FALSE;
}

// ----------------------------------------------------------------------------
// Colour stuff
// ----------------------------------------------------------------------------

bool wxWindowBeOS::SetBackgroundColour(const wxColour& col)
{
    wxWindowBase::SetBackgroundColour(col);

//    Display *xdisplay = (Display*) wxGlobalDisplay();
//    int xscreen = DefaultScreen( xdisplay );
//    Colormap cm = DefaultColormap( xdisplay, xscreen );
//
//    m_backgroundColour.CalcPixel( (WXColormap) cm );
//
//    // We don't set the background colour as we paint
//    // the background ourselves.
//    // XSetWindowBackground( xdisplay, (Window) m_clientWindow, m_backgroundColour.GetPixel() );

    return TRUE;
}

bool wxWindowBeOS::SetForegroundColour(const wxColour& col)
{
//knorr!! SetViewColor!!
    if ( !wxWindowBase::SetForegroundColour(col) )
        return FALSE;

    return TRUE;
}




// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

wxWindow *wxGetActiveWindow()
{
    // TODO
    wxFAIL_MSG(wxT("Not implemented"));
    return NULL;
}

/* static */
wxWindow *wxWindowBase::GetCapture()
{
//    return (wxWindow *)g_captureWindow;
	return NULL;
}


// Find the wxWindow at the current mouse position, returning the mouse
// position.
wxWindow* wxFindWindowAtPointer(wxPoint& pt)
{
    return wxFindWindowAtPoint(wxGetMousePosition());
}

// Get the current mouse position.
wxPoint wxGetMousePosition()
{
//    Display *display = wxGlobalDisplay();
//    Window rootWindow = RootWindowOfScreen (DefaultScreenOfDisplay(display));
//    Window rootReturn, childReturn;
//    int rootX, rootY, winX, winY;
//    unsigned int maskReturn;
//
//    XQueryPointer (display,
//                   rootWindow,
//                   &rootReturn,
//                   &childReturn,
//                   &rootX, &rootY, &winX, &winY, &maskReturn);
//    return wxPoint(rootX, rootY);
	return wxPoint(0, 0);
}


// ----------------------------------------------------------------------------
// wxNoOptimize: switch off size optimization
// ----------------------------------------------------------------------------

int wxNoOptimize::ms_count = 0;


// ----------------------------------------------------------------------------
// wxDCModule
// ----------------------------------------------------------------------------

class wxWinModule : public wxModule
{
public:
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxWinModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxWinModule, wxModule)

bool wxWinModule::OnInit()
{
//    Display *xdisplay = wxGlobalDisplay();
//    int xscreen = DefaultScreen( xdisplay );
//    Window xroot = RootWindow( xdisplay, xscreen );
//    g_eraseGC = XCreateGC( xdisplay, xroot, 0, NULL );
//    XSetFillStyle( xdisplay, g_eraseGC, FillSolid );

    return TRUE;
}

void wxWinModule::OnExit()
{
//    Display *xdisplay = wxGlobalDisplay();
//    XFreeGC( xdisplay, g_eraseGC );
}


void wxWindow::SetScrollPos(int orient, int pos, bool WXUNUSED(refresh))
{
//    wxScrollBar *scrollbar = GetScrollbar(orient);
//    wxCHECK_RET( scrollbar, _T("no scrollbar to set position for") );
//
//    scrollbar->SetThumbPosition(pos);
//
//    // VZ: I think we can safely ignore this as we always refresh it
//    //     automatically whenever the value chanegs
//#if 0
//    if ( refresh )
//        Refresh();
//#endif
}

int wxWindow::GetScrollPos(int orient) const
{
//    wxScrollBar *scrollbar = GetScrollbar(orient);
//    return scrollbar ? scrollbar->GetThumbPosition() : 0;
	return 0;
}

int wxWindow::GetScrollThumb(int orient) const
{
//    wxScrollBar *scrollbar = GetScrollbar(orient);
//    return scrollbar ? scrollbar->GetThumbSize() : 0;
	return 0;
}

int wxWindow::GetScrollRange(int orient) const
{
 //   wxScrollBar *scrollbar = GetScrollbar(orient);
//    return scrollbar ? scrollbar->GetRange() : 0;
	return 0;
}

#if wxUSE_MENUS
bool wxWindow::DoPopupMenu(wxMenu *menu, int x, int y)
{
	return false;
}
#endif // wxUSE_MENUS

void wxWindow::AddChild(wxWindowBase * kinder)
{
	wxWindowBase::AddChild(kinder);
	if(((wxWindow*)kinder)->m_view && m_view) 
		m_view->AddChild(((wxWindow*)kinder)->GetBView());
}

void wxWindow::RemoveChild(wxWindowBase * kinder)
{
	wxWindowBase::RemoveChild(kinder);
	if(((wxWindow*)kinder)->m_view && m_view)
		if(m_view->LockLooper())
		{
			m_view->RemoveChild(((wxWindow*)kinder)->m_view);
			m_view->UnlockLooper();
		}
}

void wxWindow::Draw()
{	
}
