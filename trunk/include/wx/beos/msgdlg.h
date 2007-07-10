/////////////////////////////////////////////////////////////////////////////
// Name:        wx/beos/msgdlg.h
// Purpose:     wxMessageDialog class
// Author:      Anton Sokolov
// Modified by:
// Created:     08.01.07
// Copyleft
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSGBOXDLG_H_
#define _WX_MSGBOXDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "msgdlg.h"
#endif

#include "wx/setup.h"
#include "wx/dialog.h"

class BAlert;
/*
 * Message box dialog
 */

extern WXDLLEXPORT_DATA(const wxChar*) wxMessageBoxCaptionStr;

class WXDLLEXPORT wxMessageDialog: public wxDialog, public wxMessageDialogBase
{
DECLARE_DYNAMIC_CLASS(wxMessageDialog)
protected:
	wxString    m_caption;
	wxString    m_message;
	wxWindow *  m_parent;
	
	BAlert * m_alert;
public:
	wxMessageDialog(wxWindow *parent, const wxString& message, const wxString& caption = wxMessageBoxCaptionStr,
		long style = wxOK|wxCENTRE, const wxPoint& pos = wxDefaultPosition);
	virtual ~wxMessageDialog() {};

	int ShowModal(void);

DECLARE_NO_COPY_CLASS(wxMessageDialog)
};


#endif
    // _WX_MSGBOXDLG_H_
