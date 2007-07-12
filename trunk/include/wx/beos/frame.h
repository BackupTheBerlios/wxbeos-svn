/////////////////////////////////////////////////////////////////////////////
// Name:        wx/beos/frame.h
// Purpose:     wxFrame class
// Author:      Anton Sokolov
// Modified by:
// Created:     05.01.07
// Copyleft
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FRAME_H_
#define _WX_FRAME_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "frame.h"
#endif

class WXDLLEXPORT wxFrame : public wxFrameBase
{
public:
    // construction
    wxFrame() { Init(); }
    wxFrame(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE,
               const wxString& name = wxFrameNameStr)
    {
        Init();

        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual ~wxFrame();

    // implement base class pure virtuals
    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);
    virtual void Raise();

    // implementation only from now on
    // -------------------------------

    // event handlers
    void OnPaint(wxPaintEvent& event);

    // Toolbar
#if wxUSE_TOOLBAR
    virtual wxToolBar* CreateToolBar(long style = -1,
                                     wxWindowID id = wxID_ANY,
                                     const wxString& name = wxToolBarNameStr);

    virtual void PositionToolBar();
#endif // wxUSE_TOOLBAR

#if wxUSE_MENUS
	virtual void SetMenuBar(wxMenuBar *menubar);
//	virtual bool DoPopupMenu(wxMenu *menu, int x, int y);
#endif
	
    // event handlers
    bool HandleMenuLoop(const wxEventType& evtType, WXWORD isPopup);

    virtual wxPoint GetClientAreaOrigin() const;

#if wxUSE_MENUS_NATIVE
    bool HandleMenuOpen();
    bool HandleMenuSelect(WXEVENTPTR event);
#endif // wxUSE_MENUS_NATIVE

protected:
    // common part of all ctors
    void Init();

    // override base class virtuals
    virtual void DoGetClientSize(int *width, int *height) const;
    virtual void DoSetClientSize(int width, int height);

#if wxUSE_MENUS_NATIVE
    // a plug in for MDI frame classes which need to do something special when
    // the menubar is set
    virtual void InternalSetMenuBar();
#endif // wxUSE_MENUS_NATIVE

    // propagate our state change to all child frames
    void IconizeChildFrames(bool bIconize);

    virtual bool IsMDIChild() const { return false; }

    // Data to save/restore when calling ShowFullScreen
    int                   m_fsStatusBarFields; // 0 for no status bar
    int                   m_fsStatusBarHeight;
    int                   m_fsToolBarHeight;

private:
    // used by IconizeChildFrames(), see comments there
    bool m_wasMinimized;

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxFrame)
};

#endif
    // _WX_FRAME_H_