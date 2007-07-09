///////////////////////////////////////////////////////////////////////////////
// Name:        src/beos/statbrbeos.cpp
// Purpose:     Implementation of wxStatusBar for BeOS
// Author:      Anton Sokolov
// Created:     14.01.07
// Copyleft
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "statusbr.h"
#endif

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/setup.h"
  #include "wx/frame.h"
  #include "wx/settings.h"
  #include "wx/dcclient.h"
#endif

#include "wx/intl.h"
#include "wx/log.h"
#include "wx/statusbr.h"
#include "wx/beos/backview.h"
#include "wx/beos/utilsbeos.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStatusBarBeOS class
// ----------------------------------------------------------------------------

wxStatusBarBeOS::wxStatusBarBeOS()
{
    SetParent(NULL);
}

bool wxStatusBarBeOS::Create(wxWindow *parent,
                             wxWindowID id,
                             long style,
                             const wxString& name)
{
	wxCHECK_MSG( parent, false, wxT("status bar must have a parent") );
	
	wxPoint pos(0, parent->GetSize().y-14);
	wxSize size(parent->GetSize().x, 14);
	
	if(!m_view)
		m_view = new BBackView(this, pos, size, B_FOLLOW_BOTTOM | B_FOLLOW_LEFT_RIGHT, name);
	StatusTextBuffer = NULL;
	
	SetName(name);
	SetParent(parent);
	SetId( id == wxID_ANY ? NewControlId() : id );
	
	parent->AddChild(this);
	
	SetFieldsCount(1);
	
	return true;
}

wxStatusBarBeOS::~wxStatusBarBeOS()
{
	Show();	
	DeleteStatusBuffer();
}

bool wxStatusBarBeOS::IsShown() const
{
//knorr!! 
	return true;
}

bool wxStatusBarBeOS::Show( bool show )
{
	if(show)
	{
		// show it if hidden
		if(IsShown())
			return false;
	}
	else
	{
		// hide it if shown
		if(!IsShown())
			return false;
	}
	return true;
}

void wxStatusBarBeOS::SetFieldsCount(int nFields, const int *widths)
{
    // this is a Windows limitation
    wxASSERT_MSG( (nFields > 0) && (nFields < 255), _T("too many fields") );

    wxStatusBarBase::SetFieldsCount(nFields, widths);

    SetFieldsWidth();
}

void wxStatusBarBeOS::SetStatusWidths(int n, const int widths[])
{
    wxStatusBarBase::SetStatusWidths(n, widths);

    SetFieldsWidth();
}

void wxStatusBarBeOS::SetFieldsWidth()
{
    // clear the status bar
    DeleteStatusBuffer();
}

void wxStatusBarBeOS::SetStatusText(const wxString& strText, int nField)
{
    wxCHECK_RET( (nField >= 0) && (nField < m_nFields),
                 _T("invalid statusbar field index") );

    SetStatusBufferText(strText,nField);
    DrawStatusBar();
}

wxString wxStatusBarBeOS::GetStatusText(int nField) const
{
    wxCHECK_MSG( (nField >= 0) && (nField < m_nFields), wxEmptyString,
                 _T("invalid statusbar field index") );

    wxString text;
    return text;
}

void wxStatusBarBeOS::DrawStatusBar()
{

	int i=0;
	int leftPos=0;
	wxArrayInt widthsAbs;
	wxString text;

	m_view->Clear();
	BRect bounds(m_view->bounds());
	BView * drawview = m_view->GetBack();
	if(drawview->LockLooper())
	{	
		rgb_color clr;
		drawview->PushState();
		clr = drawview->ViewColor();
		clr.red-=50; clr.green-=50; clr.blue-=50;
		drawview->SetHighColor(clr);
		drawview->StrokeLine(BPoint(bounds.left, bounds.top), BPoint(bounds.right, bounds.top));
		clr.red+=100; clr.green+=100; clr.blue+=100;
		drawview->SetHighColor(clr);
		drawview->StrokeLine(BPoint(bounds.left, bounds.top+1), BPoint(bounds.right, bounds.top+1));
		drawview->PopState();
		
		if(m_nFields>0)
			widthsAbs = CalculateAbsWidths(bounds.IntegerWidth() - 2*(m_nFields - 1));
		
		drawview->SetDrawingMode(B_OP_OVER);
		for(i=0;i<m_nFields;i++)
		{
			text = GetStatusBufferText(i);
			drawview->DrawString(text, BPoint(leftPos, bounds.bottom-2));
			leftPos+=widthsAbs[i]+2;
		}
		
		drawview->UnlockLooper();
	}
	m_view->flush();
}

void wxStatusBarBeOS::SetStatusBufferText(const wxString& text, int number)
{
    wxListString* st = GetOrCreateStatusBuffer(number);

    wxString tmp1(text);
    wxString* tmp = new wxString(tmp1);
    st->Insert(tmp);
}

wxString wxStatusBarBeOS::GetStatusBufferText(int number)
{
    wxListString *st = GetStatusBufferStack(number);
    if(st==0)
        return wxEmptyString;

    wxListString::compatibility_iterator top = st->GetFirst();
    return(*top->GetData());
}

wxListString *wxStatusBarBeOS::GetOrCreateStatusBuffer(int i)
{
    if(!StatusTextBuffer)
    {
        StatusTextBuffer = new wxListString*[m_nFields];

        size_t j;
        for(j = 0; j < (size_t)m_nFields; ++j) StatusTextBuffer[j] = 0;
    }

    if(!StatusTextBuffer[i])
    {
        StatusTextBuffer[i] = new wxListString();
    }
    else
    {
        wxListString *st=StatusTextBuffer[i];
        wxListString::compatibility_iterator top = st->GetFirst();
        delete top->GetData();
        st->Erase(top);
        delete st;

        StatusTextBuffer[i] = new wxListString();
    }

    return StatusTextBuffer[i];
}

wxListString *wxStatusBarBeOS::GetStatusBufferStack(int i) const
{
    if(!StatusTextBuffer)
        return 0;
    return StatusTextBuffer[i];
}

void wxStatusBarBeOS::DeleteStatusBuffer()
{
    if(!StatusTextBuffer)
    {
        return;
    }

    for(int i=0;i<m_nFields;i++)
    {
        if(StatusTextBuffer[i])
        {
            wxListString *st=StatusTextBuffer[i];
            wxListString::compatibility_iterator top = st->GetFirst();
            delete top->GetData();
            st->Erase(top);
            delete st;
            StatusTextBuffer[i]=0;
        }
    }
    delete[] m_statusTextStacks;
}

int wxStatusBarBeOS::GetBorderX() const
{
    return 0;
}

int wxStatusBarBeOS::GetBorderY() const
{
    return 0;
}

void wxStatusBarBeOS::SetMinHeight(int height)
{
}

bool wxStatusBarBeOS::GetFieldRect(int i, wxRect& rect) const
{
	return false;
}

void wxStatusBarBeOS::DoMoveWindow(int x, int y, int width, int height)
{
}

