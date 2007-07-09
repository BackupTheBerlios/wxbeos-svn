///////////////////////////////////////////////////////////////////////////////
// Name:        src/beos/menuitem.cpp
// Purpose:     wxMenuItem implementation
// Author:      Anton Sokolov
// Modified by:
// Created:     06.01.07
// Copyleft
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "menuitem.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MENUS

#ifndef WX_PRECOMP
    #include "wx/font.h"
    #include "wx/bitmap.h"
    #include "wx/settings.h"
    #include "wx/font.h"
    #include "wx/window.h"
    #include "wx/accel.h"
    #include "wx/menu.h"
    #include "wx/string.h"
#endif

#include "wx/menuitem.h"
#include "wx/log.h"

#include <MenuItem.h>

#if wxUSE_ACCEL
    #include "wx/accel.h"
#endif // wxUSE_ACCEL

// ---------------------------------------------------------------------------
// macro
// ---------------------------------------------------------------------------

// conditional compilation
#if wxUSE_OWNER_DRAWN
    #define OWNER_DRAWN_ONLY( code ) if ( IsOwnerDrawn() ) code
#else // !wxUSE_OWNER_DRAWN
    #define OWNER_DRAWN_ONLY( code )
#endif // wxUSE_OWNER_DRAWN/!wxUSE_OWNER_DRAWN

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// dynamic classes implementation
// ----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI

bool wxMenuItemStreamingCallback( const wxObject *object, wxWriter * , wxPersister * , wxxVariantArray & )
{
    const wxMenuItem * mitem = dynamic_cast<const wxMenuItem*>(object) ;
    if ( mitem->GetMenu() && !mitem->GetMenu()->GetTitle().empty() )
    {
        // we don't stream out the first two items for menus with a title, they will be reconstructed
        if ( mitem->GetMenu()->FindItemByPosition(0) == mitem || mitem->GetMenu()->FindItemByPosition(1) == mitem )
            return false ;
    }
    return true ;
}

wxBEGIN_ENUM( wxItemKind )
    wxENUM_MEMBER( wxITEM_SEPARATOR )
    wxENUM_MEMBER( wxITEM_NORMAL )
    wxENUM_MEMBER( wxITEM_CHECK )
    wxENUM_MEMBER( wxITEM_RADIO )
wxEND_ENUM( wxItemKind )

IMPLEMENT_DYNAMIC_CLASS_XTI_CALLBACK(wxMenuItem, wxObject,"wx/menuitem.h",wxMenuItemStreamingCallback)

wxBEGIN_PROPERTIES_TABLE(wxMenuItem)
    wxPROPERTY( Parent,wxMenu*, SetMenu, GetMenu, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( Id,int, SetId, GetId, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( Text, wxString , SetText, GetText, wxString(), 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( Help, wxString , SetHelp, GetHelp, wxString(), 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxREADONLY_PROPERTY( Kind, wxItemKind , GetKind , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( SubMenu,wxMenu*, SetSubMenu, GetSubMenu, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( Enabled , bool , Enable , IsEnabled , wxxVariant((bool)true) , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Checked , bool , Check , IsChecked , wxxVariant((bool)false) , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Checkable , bool , SetCheckable , IsCheckable , wxxVariant((bool)false) , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxMenuItem)
wxEND_HANDLERS_TABLE()

wxDIRECT_CONSTRUCTOR_6( wxMenuItem , wxMenu* , Parent , int , Id , wxString , Text , wxString , Help , wxItemKind , Kind , wxMenu* , SubMenu  )
#else
IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)
#endif

// ----------------------------------------------------------------------------
// wxMenuItem
// ----------------------------------------------------------------------------

// ctor & dtor
// -----------

wxMenuItem::wxMenuItem(wxMenu *pParentMenu,
                       int id,
                       const wxString& text,
                       const wxString& strHelp,
                       wxItemKind kind,
                       wxMenu *pSubMenu)
          : wxMenuItemBase(pParentMenu, id, text, strHelp, kind, pSubMenu)
#if wxUSE_OWNER_DRAWN
            , wxOwnerDrawn(text, kind == wxITEM_CHECK, true)
#endif // owner drawn
	, m_mnu(NULL)
{
	if(pSubMenu)
		m_mnu = pSubMenu->GetBMenu();
	Init();
}

wxMenuItem::wxMenuItem(wxMenu *parentMenu,
                       int id,
                       const wxString& text,
                       const wxString& help,
                       bool isCheckable,
                       wxMenu *subMenu)
          : wxMenuItemBase(parentMenu, id, text, help,
                           isCheckable ? wxITEM_CHECK : wxITEM_NORMAL, subMenu)
#if wxUSE_OWNER_DRAWN
           , wxOwnerDrawn(text, isCheckable, true)
#endif // owner drawn
	, m_mnu(NULL)
{
	if(subMenu)
		m_mnu = subMenu->GetBMenu();
	Init();
}

void wxMenuItem::Init()
{
 	if(m_id==wxID_SEPARATOR)
		m_mnuitem = new BSeparatorItem();
	else
		m_mnuitem = new BMenuItem(m_text.c_str(), new BMessage(m_id));
	SetText(m_text);
}

wxMenuItem::~wxMenuItem()
{
}

// misc
// ----

// get item state
// --------------

bool wxMenuItem::IsChecked() const
{
	return m_isChecked;
}

/* static */
wxString wxMenuItemBase::GetLabelFromText(const wxString& text)
{
	return wxStripMenuCodes(text);
}

// radio group stuff
// -----------------

void wxMenuItem::SetAsRadioGroupStart()
{
}

void wxMenuItem::SetRadioGroupStart(int start)
{
}

void wxMenuItem::SetRadioGroupEnd(int end)
{
}

// change item state
// -----------------

void wxMenuItem::Enable(bool enable)
{
	m_isEnabled = enable;
	m_mnuitem->SetEnabled(m_isEnabled);
}

void wxMenuItem::Check(bool check)
{
	if(m_kind != wxITEM_CHECK)
		return;
	m_mnuitem->SetMarked(check);
	m_isChecked = check;		
}

void wxMenuItem::SetText(const wxString& text)
{
	wxString label(text);
#if wxUSE_ACCEL
	wxAcceleratorEntry * accel = GetAccel();
#endif
	label.Replace("&", "");
	int pos = label.Find("\t");
	if(pos) label = label(0, pos);
	m_mnuitem->SetLabel(label.c_str());
	if(m_mnu)
		m_mnu->SetName(label.c_str());
}

void wxMenuItem::SetCheckable(bool checkable)
{
	if(checkable)
		m_kind = wxITEM_CHECK;
}

// ----------------------------------------------------------------------------
// wxMenuItemBase
// ----------------------------------------------------------------------------

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu,
                                int id,
                                const wxString& name,
                                const wxString& help,
                                wxItemKind kind,
                                wxMenu *subMenu)
{
	return new wxMenuItem(parentMenu, id, name, help, kind, subMenu);
}

#if wxUSE_ACCEL

static wxString GetGtkHotKey( const wxMenuItem& item )
{
    wxString hotkey;

    wxAcceleratorEntry *accel = item.GetAccel();
    if ( accel )
    {
        int flags = accel->GetFlags();
        if ( flags & wxACCEL_ALT )
            hotkey += wxT("<alt>");
        if ( flags & wxACCEL_CTRL )
            hotkey += wxT("<control>");
        if ( flags & wxACCEL_SHIFT )
            hotkey += wxT("<shift>");

        int code = accel->GetKeyCode();
        switch ( code )
        {
            case WXK_F1:
            case WXK_F2:
            case WXK_F3:
            case WXK_F4:
            case WXK_F5:
            case WXK_F6:
            case WXK_F7:
            case WXK_F8:
            case WXK_F9:
            case WXK_F10:
            case WXK_F11:
            case WXK_F12:
            case WXK_F13:
            case WXK_F14:
            case WXK_F15:
            case WXK_F16:
            case WXK_F17:
            case WXK_F18:
            case WXK_F19:
            case WXK_F20:
            case WXK_F21:
            case WXK_F22:
            case WXK_F23:
            case WXK_F24:
                hotkey += wxString::Format(wxT("F%d"), code - WXK_F1 + 1);
                break;

                // TODO: we should use gdk_keyval_name() (a.k.a.
                //       XKeysymToString) here as well as hardcoding the keysym
                //       names this might be not portable
           case WXK_INSERT:
                hotkey << wxT("Insert" );
                break;
            case WXK_DELETE:
                hotkey << wxT("Delete" );
                break;
            case WXK_UP:
                hotkey << wxT("Up" );
                break;
            case WXK_DOWN:
                hotkey << wxT("Down" );
                break;
            case WXK_PAGEUP:
            case WXK_PRIOR:
                hotkey << wxT("Prior" );
                break;
            case WXK_PAGEDOWN:
            case WXK_NEXT:
                hotkey << wxT("Next" );
                break;
            case WXK_LEFT:
                hotkey << wxT("Left" );
                break;
            case WXK_RIGHT:
                hotkey << wxT("Right" );
                break;
            case WXK_HOME:
                hotkey << wxT("Home" );
                break;
            case WXK_END:
                hotkey << wxT("End" );
                break;
            case WXK_RETURN:
                hotkey << wxT("Return" );
                break;
            case WXK_BACK:
                hotkey << wxT("BackSpace" );
                break;
            case WXK_TAB:
                hotkey << wxT("Tab" );
                break;
            case WXK_ESCAPE:
                hotkey << wxT("Esc" );
                break;
            case WXK_SPACE:
                hotkey << wxT("space" );
                break;
            case WXK_MULTIPLY:
                hotkey << wxT("Multiply" );
                break;
            case WXK_ADD:
                hotkey << wxT("Add" );
                break;
            case WXK_SEPARATOR:
                hotkey << wxT("Separator" );
                break;
            case WXK_SUBTRACT:
                hotkey << wxT("Subtract" );
                break;
            case WXK_DECIMAL:
                hotkey << wxT("Decimal" );
                break;
            case WXK_DIVIDE:
                hotkey << wxT("Divide" );
                break;
            case WXK_CANCEL:
                hotkey << wxT("Cancel" );
                break;
            case WXK_CLEAR:
                hotkey << wxT("Clear" );
                break;
            case WXK_MENU:
                hotkey << wxT("Menu" );
                break;
            case WXK_PAUSE:
                hotkey << wxT("Pause" );
                break;
            case WXK_CAPITAL:
                hotkey << wxT("Capital" );
                break;
            case WXK_SELECT:
                hotkey << wxT("Select" );
                break;
            case WXK_PRINT:
                hotkey << wxT("Print" );
                break;
            case WXK_EXECUTE:
                hotkey << wxT("Execute" );
                break;
            case WXK_SNAPSHOT:
                hotkey << wxT("Snapshot" );
                break;
            case WXK_HELP:
                hotkey << wxT("Help" );
                break;
            case WXK_NUMLOCK:
                hotkey << wxT("Num_Lock" );
                break;
            case WXK_SCROLL:
                hotkey << wxT("Scroll_Lock" );
                break;
            case WXK_NUMPAD_INSERT:
                hotkey << wxT("KP_Insert" );
                break;
            case WXK_NUMPAD_DELETE:
                hotkey << wxT("KP_Delete" );
                break;
             case WXK_NUMPAD_SPACE:
                hotkey << wxT("KP_Space" );
                break;
            case WXK_NUMPAD_TAB:
                hotkey << wxT("KP_Tab" );
                break;
            case WXK_NUMPAD_ENTER:
                hotkey << wxT("KP_Enter" );
                break;
            case WXK_NUMPAD_F1: case WXK_NUMPAD_F2: case WXK_NUMPAD_F3:
            case WXK_NUMPAD_F4:
                hotkey += wxString::Format(wxT("KP_F%d"), code - WXK_NUMPAD_F1 + 1);
                break;
            case WXK_NUMPAD_HOME:
                hotkey << wxT("KP_Home" );
                break;
            case WXK_NUMPAD_LEFT:
                hotkey << wxT("KP_Left" );
                break;
             case WXK_NUMPAD_UP:
                hotkey << wxT("KP_Up" );
                break;
            case WXK_NUMPAD_RIGHT:
                hotkey << wxT("KP_Right" );
                break;
            case WXK_NUMPAD_DOWN:
                hotkey << wxT("KP_Down" );
                break;
            case WXK_NUMPAD_PRIOR: case WXK_NUMPAD_PAGEUP:
                hotkey << wxT("KP_Prior" );
                break;
            case WXK_NUMPAD_NEXT:  case WXK_NUMPAD_PAGEDOWN:
                hotkey << wxT("KP_Next" );
                break;
            case WXK_NUMPAD_END:
                hotkey << wxT("KP_End" );
                break;
            case WXK_NUMPAD_BEGIN:
                hotkey << wxT("KP_Begin" );
                break;
            case WXK_NUMPAD_EQUAL:
                hotkey << wxT("KP_Equal" );
                break;
            case WXK_NUMPAD_MULTIPLY:
                hotkey << wxT("KP_Multiply" );
                break;
            case WXK_NUMPAD_ADD:
                hotkey << wxT("KP_Add" );
                break;
            case WXK_NUMPAD_SEPARATOR:
                hotkey << wxT("KP_Separator" );
                break;
            case WXK_NUMPAD_SUBTRACT:
                hotkey << wxT("KP_Subtract" );
                break;
            case WXK_NUMPAD_DECIMAL:
                hotkey << wxT("KP_Decimal" );
                break;
            case WXK_NUMPAD_DIVIDE:
                hotkey << wxT("KP_Divide" );
                break;
           case WXK_NUMPAD0: case WXK_NUMPAD1: case WXK_NUMPAD2:
           case WXK_NUMPAD3: case WXK_NUMPAD4: case WXK_NUMPAD5:
           case WXK_NUMPAD6: case WXK_NUMPAD7: case WXK_NUMPAD8: case WXK_NUMPAD9:
                hotkey += wxString::Format(wxT("KP_%d"), code - WXK_NUMPAD0);
                break;
            case WXK_WINDOWS_LEFT:
                hotkey << wxT("Super_L" );
                break;
            case WXK_WINDOWS_RIGHT:
                hotkey << wxT("Super_R" );
                break;
            case WXK_WINDOWS_MENU:
                hotkey << wxT("Menu" );
                break;
            case WXK_COMMAND:
                hotkey << wxT("Command" );
                break;
          /* These probably wouldn't work as there is no SpecialX in gdk/keynames.txt
            case WXK_SPECIAL1: case WXK_SPECIAL2: case WXK_SPECIAL3: case WXK_SPECIAL4:
            case WXK_SPECIAL5: case WXK_SPECIAL6: case WXK_SPECIAL7: case WXK_SPECIAL8:
            case WXK_SPECIAL9:  case WXK_SPECIAL10:  case WXK_SPECIAL11: case WXK_SPECIAL12:
            case WXK_SPECIAL13: case WXK_SPECIAL14: case WXK_SPECIAL15: case WXK_SPECIAL16:
            case WXK_SPECIAL17: case WXK_SPECIAL18: case WXK_SPECIAL19:  case WXK_SPECIAL20:
                hotkey += wxString::Format(wxT("Special%d"), code - WXK_SPECIAL1 + 1);
                break;
          */
                // if there are any other keys wxGetAccelFromString() may
                // return, we should process them here

            default:
                if ( code < 127 )
                {
                    wxString name = wxGTK_CONV_BACK( gdk_keyval_name((guint)code) );
                    if ( name )
                    {
                        hotkey << name;
                        break;
                    }
                }

                wxFAIL_MSG( wxT("unknown keyboard accel") );
        }

        delete accel;
    }

    return hotkey;
}
#endif // wxUSE_ACCELS
#endif // wxUSE_MENUS
