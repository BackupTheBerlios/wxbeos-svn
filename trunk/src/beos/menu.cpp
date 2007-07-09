/////////////////////////////////////////////////////////////////////////////
// Name:        src/beos/menu.cpp
// Purpose:     wxMenu, wxMenuBar, wxMenuItem
// Author:      Anton Sokolov
// Modified by:
// Created:     06.01.07
// Copyleft
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "menu.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MENUS

#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/utils.h"
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#if wxUSE_OWNER_DRAWN
    #include "wx/ownerdrw.h"
#endif

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

extern wxMenu *wxCurrentPopupMenu;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the (popup) menu title has this special id
static const int idMenuTitle = -3;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// ============================================================================
// implementation
// ============================================================================

#if wxUSE_EXTENDED_RTTI

WX_DEFINE_FLAGS( wxMenuStyle )

wxBEGIN_FLAGS( wxMenuStyle )
    wxFLAGS_MEMBER(wxMENU_TEAROFF)
wxEND_FLAGS( wxMenuStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxMenu, wxEvtHandler,"wx/menu.h")

wxCOLLECTION_TYPE_INFO( wxMenuItem * , wxMenuItemList ) ;

template<> void wxCollectionToVariantArray( wxMenuItemList const &theList, wxxVariantArray &value)
{
    wxListCollectionToVariantArray<wxMenuItemList::compatibility_iterator>( theList , value ) ;
}

wxBEGIN_PROPERTIES_TABLE(wxMenu)
    wxEVENT_PROPERTY( Select , wxEVT_COMMAND_MENU_SELECTED , wxCommandEvent)
    wxPROPERTY( Title, wxString , SetTitle, GetTitle, wxString(), 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxREADONLY_PROPERTY_FLAGS( MenuStyle , wxMenuStyle , long , GetStyle , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
    wxPROPERTY_COLLECTION( MenuItems , wxMenuItemList , wxMenuItem* , Append , GetMenuItems , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxMenu)
wxEND_HANDLERS_TABLE()

wxDIRECT_CONSTRUCTOR_2( wxMenu , wxString , Title , long , MenuStyle  )

WX_DEFINE_FLAGS( wxMenuBarStyle )

wxBEGIN_FLAGS( wxMenuBarStyle )
    wxFLAGS_MEMBER(wxMB_DOCKABLE)
wxEND_FLAGS( wxMenuBarStyle )

// the negative id would lead the window (its superclass !) to vetoe streaming out otherwise
bool wxMenuBarStreamingCallback( const wxObject *WXUNUSED(object), wxWriter * , wxPersister * , wxxVariantArray & )
{
    return true ;
}

IMPLEMENT_DYNAMIC_CLASS_XTI_CALLBACK(wxMenuBar, wxWindow ,"wx/menu.h",wxMenuBarStreamingCallback)

IMPLEMENT_DYNAMIC_CLASS_XTI(wxMenuInfo, wxObject , "wx/menu.h" )

wxBEGIN_PROPERTIES_TABLE(wxMenuInfo)
    wxREADONLY_PROPERTY( Menu , wxMenu* , GetMenu , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxREADONLY_PROPERTY( Title , wxString , GetTitle , wxString() , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxMenuInfo)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_2( wxMenuInfo , wxMenu* , Menu , wxString , Title )

wxCOLLECTION_TYPE_INFO( wxMenuInfo * , wxMenuInfoList ) ;

template<> void wxCollectionToVariantArray( wxMenuInfoList const &theList, wxxVariantArray &value)
{
    wxListCollectionToVariantArray<wxMenuInfoList::compatibility_iterator>( theList , value ) ;
}

wxBEGIN_PROPERTIES_TABLE(wxMenuBar)
    wxPROPERTY_COLLECTION( MenuInfos , wxMenuInfoList , wxMenuInfo* , Append , GetMenuInfos , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxMenuBar)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_DUMMY( wxMenuBar )

#else
IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxEvtHandler)
IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxWindow)
#endif

// ---------------------------------------------------------------------------
// wxMenu construction, adding and removing menu items
// ---------------------------------------------------------------------------

// Construct a menu with optional title (then use append)
void wxMenu::Init()
{
	m_mnu = new BMenu("A menu");
}

// The wxWindow destructor will take care of deleting the submenus.
wxMenu::~wxMenu()
{
}

void wxMenu::Break()
{
}

void wxMenu::Attach(wxMenuBarBase *menubar)
{
    wxMenuBase::Attach(menubar);
}

#if wxUSE_ACCEL

int wxMenu::FindAccel(int id) const
{
    return wxNOT_FOUND;
}

void wxMenu::UpdateAccel(wxMenuItem *item)
{
}

#endif // wxUSE_ACCEL

// append a new item or submenu to the menu
bool wxMenu::DoInsertOrAppend(wxMenuItem *pItem, size_t pos)
{
    if ( IsAttached() && GetMenuBar()->IsAttached() )
    {
        // Regenerate the menu resource
        GetMenuBar()->Refresh();
    }

    return true;
}

void wxMenu::EndRadioGroup()
{
}

wxMenuItem* wxMenu::DoAppend(wxMenuItem *item)
{
	wxCHECK_MSG( item, NULL, _T("NULL item in wxMenu::DoAppend") );

	if(!wxMenuBase::DoAppend(item) || !DoInsertOrAppend(item))
	{
		return NULL;
	}
	else if(IsAttached() && GetMenuBar()->IsAttached())
	{
		// Regenerate the menu resource
		GetMenuBar()->Refresh();
	}
	
	if(item->GetBMenu())
		m_mnu->AddItem(item->GetBMenu());
	else
		m_mnu->AddItem(item->GetBMenuItem());
	
	return item;
}

wxMenuItem* wxMenu::DoInsert(size_t pos, wxMenuItem *item)
{
    if (wxMenuBase::DoInsert(pos, item) && DoInsertOrAppend(item, pos))
        return item;
    else
        return NULL;
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    // we need to find the items position in the child list
    size_t pos;
    wxMenuItemList::compatibility_iterator node = GetMenuItems().GetFirst();
    for ( pos = 0; node; pos++ )
    {
        if ( node->GetData() == item )
            break;

        node = node->GetNext();
    }

    // DoRemove() (unlike Remove) can only be called for existing item!
    wxCHECK_MSG( node, NULL, wxT("bug in wxMenu::Remove logic") );

    // remove the item from the menu
    wxMenuItem *ret=wxMenuBase::DoRemove(item);

    if ( IsAttached() && GetMenuBar()->IsAttached() )
    {
        // Regenerate the menu resource
        GetMenuBar()->Refresh();
    }

    return ret;
}

// ---------------------------------------------------------------------------
// accelerator helpers
// ---------------------------------------------------------------------------

#if wxUSE_ACCEL

// create the wxAcceleratorEntries for our accels and put them into provided
// array - return the number of accels we have
size_t wxMenu::CopyAccels(wxAcceleratorEntry *accels) const
{
    size_t count = GetAccelCount();
    for ( size_t n = 0; n < count; n++ )
    {
        *accels++ = *m_accels[n];
    }

    return count;
}

#endif // wxUSE_ACCEL

// ---------------------------------------------------------------------------
// set wxMenu title
// ---------------------------------------------------------------------------

void wxMenu::SetTitle(const wxString& label)
{
	m_title = label;
	
	if ( IsAttached() && GetMenuBar()->IsAttached() )
	{
	    // Regenerate the menu resource
	    GetMenuBar()->Refresh();
	}
	
	m_mnu->SetName(wxStripMenuCodes(label).c_str());
}

// ---------------------------------------------------------------------------
// event processing
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// other
// ---------------------------------------------------------------------------

wxWindow *wxMenu::GetWindow() const
{
	return NULL;
}


// ---------------------------------------------------------------------------
// Menu Bar
// ---------------------------------------------------------------------------

void wxMenuBar::Init()
{
	m_mnubar = new BMenuBar(BRect(0, 0, 0, 0), "A MenuBar");
	m_view = (BBackView*)m_mnubar;
}

wxMenuBar::wxMenuBar()
{
	Init();
}

wxMenuBar::wxMenuBar( long WXUNUSED(style) )
{
	Init();
}

wxMenuBar::wxMenuBar(size_t count, wxMenu *menus[], const wxString titles[], long WXUNUSED(style))
{
	Init();
}

wxMenuBar::~wxMenuBar()
{
}

// ---------------------------------------------------------------------------
// wxMenuBar helpers
// ---------------------------------------------------------------------------

void wxMenuBar::Refresh()
{
	wxCHECK_RET( IsAttached(), wxT("can't refresh unattached menubar") );

	// Regenerate the menu resource
	LoadMenu();
}

// ---------------------------------------------------------------------------
// wxMenuBar functions to work with the top level submenus
// ---------------------------------------------------------------------------

void wxMenuBar::EnableTop(size_t pos, bool enable)
{
    // Palm OS does not have support for grayed or disabled items
}

void wxMenuBar::SetLabelTop(size_t pos, const wxString& label)
{
    wxCHECK_RET( pos < GetMenuCount(), wxT("invalid menu index") );

    m_titles[pos]=wxStripMenuCodes(label);

    if ( !IsAttached() )
    {
        return;
    }

       // Regenerate the menu resource
    Refresh();
}

wxString wxMenuBar::GetLabelTop(size_t pos) const
{
    wxCHECK_MSG( pos < GetMenuCount(), wxEmptyString,
                 wxT("invalid menu index in wxMenuBar::GetLabelTop") );

    return wxMenuItem::GetLabelFromText(m_titles[pos]);
}

// ---------------------------------------------------------------------------
// wxMenuBar construction
// ---------------------------------------------------------------------------

wxMenu *wxMenuBar::Replace(size_t pos, wxMenu *menu, const wxString& title)
{
   wxMenu *menuOld = wxMenuBarBase::Replace(pos, menu, title);
    if ( !menuOld )
        return NULL;

    m_titles[pos]=wxStripMenuCodes(title);

    if ( IsAttached() )
    {
        // Regenerate the menu resource
        Refresh();
    }

    return menuOld;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
	if ( !wxMenuBarBase::Insert(pos, menu, title) )
		return false;
	
	m_titles.Insert(wxStripMenuCodes(title), pos);
	
	if ( IsAttached() )
	{
		// Regenerate the menu resource
		Refresh();
	}

	menu->SetTitle(title);
	m_mnubar->AddItem(menu->GetBMenu(), pos);
	
	return true;
}

bool wxMenuBar::Append(wxMenu *menu, const wxString& title)
{
	if ( !wxMenuBarBase::Append(menu, title) )
		return false;
	
	m_titles.Add(wxStripMenuCodes(title));
	
	if(IsAttached())
	{
	   // Regenerate the menu resource
		Refresh();
	}
	
	menu->SetTitle(title);
	m_mnubar->AddItem(menu->GetBMenu());
	
	return true;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if ( !menu )
        return NULL;

    m_titles.RemoveAt(pos);

    if (IsAttached())
    {
        // Regenerate the menu resource
        Refresh();
    }

    return menu;
}

#if wxUSE_ACCEL

void wxMenuBar::RebuildAccelTable()
{
}

#endif // wxUSE_ACCEL

//int wxMenuBar::ProcessCommand(int ItemID)
//{
//    if(!IsAttached())
//        return -1;
//
//    int MenuNum=(ItemID/1000)-1;
//    int ItemNum=(ItemID-(1000*(MenuNum+1)));
//
//    // Should never happen, but it doesn't hurt to check anyway.
//    if(MenuNum>GetMenuCount())
//        return -1;
//
//    // Get the menu
//    wxMenu *ActiveMenu=GetMenu(MenuNum);
//
//    // Make sure this is a valid item.
//    if(ItemNum>ActiveMenu->GetMenuItemCount())
//        return -1;
//
//    // Get the item
//    wxMenuItem *ActiveItem=ActiveMenu->FindItemByPosition(ItemNum);
//    int ActiveID=ActiveItem->GetId();
//
//    return ActiveID;
//}

/* Palm OS does not have good dynamic menu support.  About all you can do with
 * the standard API calls is to add new items to an existing drop-down menu and
 * hide/show items in a drop-down menu.  It is impossible to add, hide, or
 * change the label on a drop-down menu.
 *
 * The easiest and simplest way around this limitation is to modify the Palm OS
 * MenuBarType structure directly.  This gives limited ability to change the
 * label on a drop-down menu.  I have not been able to find a safe way to add,
 * delete, or resize drop-down menus in OS 6.
 *
 * The following routine attempt to work around these limitations present in the
 * Palm OS API to provide limited dynamic menu support.  This solution is far
 * from perfect, but the only other option is to wait for PalmSource to add full
 * dynamic menu support, or to recreate the Palm OS menu system from scratch.
 *
 * This system is limited in that no more than 4 drop-down menus are allowed per
 * menu bar, and the label for each drop-down menu is limited to 8 characters of
 * text.  However, this menu system should work for most applications.
 *
 * Basically the menu routines select one of four menu bars, depending on
 * whether or not the requested menu bar has one, two, three, or four drop-down
 * menus.
 *
 * These four "template" menu bars contain one, two, three, or four drop-down
 * menus.  Each menu has a dummy menu item attached to it to allow the Palm OS
 * MenuAddItem function to add the real items.
 *
 * The labels on the drop-down menus are then replaced with the labels of the
 * real menus.
 *
 * The menu is then attached to the active window and the MenuAddItem API
 * function is called to add the items to each drop-down menu.  Finally,
 * MenuHideItem is called to remove the dummy items from each drop-down menu.
 */
void wxMenuBar::LoadMenu()
{
    int i=0;
    int j=0;

    // Handle to the currently running application database
    // Get app database reference - needed for some Palm OS Menu API calls.

    // Get the number of menus
    int NumMenus=GetMenuCount();

    // Set up the pointers and handles
    // Load the menu template and set up the menu pointers
}

void wxMenuBar::Attach(wxFrame *frame)
{
    // before attaching preprocess menus to not include wxID_EXIT item
    // as PalmOS guidelines suggest

    wxMenuItem *item;
    wxMenu *menu;
    int i;

    // check if we received any empty menu!
    i = 0;
    while(i < GetMenuCount())
    {
        menu = GetMenu(i);

        if( menu && (menu->GetMenuItemCount()==0) )
        {
            menu = Remove( i );
            delete menu;
        }
        else
            i++;
    }

    wxMenuBarBase::Attach(frame);

    LoadMenu();
}

void wxMenuBar::Detach()
{
    wxMenuBarBase::Detach();
}

wxMenuItem* wxMenuBar::FindItem(int nId, wxMenu** ppMenu = NULL) const
{
	return wxMenuBarBase::FindItem(nId, ppMenu);
}

int wxMenuBar::FindMenuItem(const wxString& rMenuString, const wxString& rItemString) const
{
	return 0;
}
#endif // wxUSE_MENUS
