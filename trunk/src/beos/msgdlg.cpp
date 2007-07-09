/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/msgdlg.cpp
// Purpose:     wxMessageDialog for GTK+2
// Author:      Vaclav Slavik
// Modified by:
// Created:     2003/02/28
// RCS-ID:      $Id: msgdlg.cpp,v 1.22 2005/03/11 15:34:12 ABX Exp $
// Copyright:   (c) Vaclav Slavik, 2003
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "msgdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/msgdlg.h"
#include "wx/intl.h"

IMPLEMENT_CLASS(wxMessageDialog, wxDialog)

wxMessageDialog::wxMessageDialog(wxWindow *parent,
                                 const wxString& message,
                                 const wxString& caption,
                                 long style,
                                 const wxPoint& WXUNUSED(pos))
{
	m_caption = caption;
	m_message = message;
	
	SetMessageDialogStyle(style);
	m_parent = wxGetTopLevelParent(parent);
	    
	alert_type a_type;
	wxString btn0 = "Ok",  btn1, btn2;


	if (style & wxYES_NO)
	{
		btn0 = "yes";
		btn1 = "no";
	}

	if (style & wxOK)
	{
		if (style & wxCANCEL)
			btn1 = "cancel";
		btn0 = "Ok";
	}

	if (style & wxICON_EXCLAMATION)
		a_type = B_WARNING_ALERT;
	else if (style & wxICON_ERROR)
		a_type = B_STOP_ALERT;
	else if (style & wxICON_INFORMATION)
		a_type = B_INFO_ALERT;
	else if (style & wxICON_QUESTION)
		a_type = B_IDEA_ALERT;
	else
		a_type = B_EMPTY_ALERT;
	
	m_alert = new BAlert(m_caption.c_str(), m_message.c_str(), btn0.c_str(), btn1==wxEmptyString?NULL:btn1.c_str(), btn2==wxEmptyString?NULL:btn2.c_str(), B_WIDTH_AS_USUAL, a_type);
//	m_alert->MoveTo(BPoint(m_parent->GetPosition().x, m_parent->GetPosition().y));
}

//wxMessageDialog::~wxMessageDialog()
//{
//}

int wxMessageDialog::ShowModal()
{
	if(!m_alert) return -1;
	int result = m_alert->Go();

	int style = GetMessageDialogStyle();
	int answer = result + (style & wxOK) + (style & wxYES_NO);
	
	switch(answer)
	{
		case wxOK :
			return wxID_OK;
		case wxOK+1 :
			return wxID_CANCEL;
		case wxYES_NO :
			return wxID_YES;
		case wxYES_NO+1 :
			return wxID_NO;
		default:
			wxFAIL_MSG(_T("unexpected GtkMessageDialog return code"));
	}
	
	return wxID_CANCEL;
}


