/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextstylepage.cpp
// Purpose:
// Author:      Julian Smart
// Modified by:
// Created:     10/5/2006 11:34:55 AM
// RCS-ID:      $Id: richtextstylepage.cpp,v 1.5 2006/10/20 07:48:30 ABX Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if wxUSE_RICHTEXT

#include "wx/richtext/richtextstylepage.h"

/*!
 * wxRichTextStylePage type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxRichTextStylePage, wxPanel )

/*!
 * wxRichTextStylePage event table definition
 */

BEGIN_EVENT_TABLE( wxRichTextStylePage, wxPanel )

////@begin wxRichTextStylePage event table entries
    EVT_UPDATE_UI( ID_RICHTEXTSTYLEPAGE_NEXT_STYLE, wxRichTextStylePage::OnNextStyleUpdate )

////@end wxRichTextStylePage event table entries

END_EVENT_TABLE()

/*!
 * wxRichTextStylePage constructors
 */

wxRichTextStylePage::wxRichTextStylePage( )
{
    Init();
}

wxRichTextStylePage::wxRichTextStylePage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}

/*!
 * Initialise members
 */

void wxRichTextStylePage::Init()
{
////@begin wxRichTextStylePage member initialisation
    m_styleName = NULL;
    m_basedOn = NULL;
    m_nextStyle = NULL;
////@end wxRichTextStylePage member initialisation
}

/*!
 * wxRichTextStylePage creator
 */

bool wxRichTextStylePage::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin wxRichTextStylePage creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end wxRichTextStylePage creation
    return true;
}

/*!
 * Control creation for wxRichTextStylePage
 */

void wxRichTextStylePage::CreateControls()
{
////@begin wxRichTextStylePage content construction
    wxRichTextStylePage* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer3->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxGROW, 5);

    wxStaticText* itemStaticText6 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Style:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText6, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_styleName = new wxTextCtrl( itemPanel1, ID_RICHTEXTSTYLEPAGE_STYLE_NAME, wxEmptyString, wxDefaultPosition, wxSize(300, -1), wxTE_READONLY );
    m_styleName->SetHelpText(_("The style name."));
    if (ShowToolTips())
        m_styleName->SetToolTip(_("The style name."));
    itemBoxSizer5->Add(m_styleName, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText8 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Based on:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText8, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_basedOnStrings = NULL;
    m_basedOn = new wxComboBox( itemPanel1, ID_RICHTEXTSTYLEPAGE_BASED_ON, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, m_basedOnStrings, wxCB_DROPDOWN );
    m_basedOn->SetHelpText(_("The style on which this style is based."));
    if (ShowToolTips())
        m_basedOn->SetToolTip(_("The style on which this style is based."));
    itemBoxSizer5->Add(m_basedOn, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText10 = new wxStaticText( itemPanel1, wxID_STATIC, _("&Next style:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(itemStaticText10, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxString* m_nextStyleStrings = NULL;
    m_nextStyle = new wxComboBox( itemPanel1, ID_RICHTEXTSTYLEPAGE_NEXT_STYLE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, m_nextStyleStrings, wxCB_DROPDOWN );
    m_nextStyle->SetHelpText(_("The default style for the next paragraph."));
    if (ShowToolTips())
        m_nextStyle->SetToolTip(_("The default style for the next paragraph."));
    itemBoxSizer5->Add(m_nextStyle, 0, wxGROW|wxALL, 5);

    itemBoxSizer3->Add(5, 5, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end wxRichTextStylePage content construction
}

/// Transfer data from/to window
bool wxRichTextStylePage::TransferDataFromWindow()
{
    wxPanel::TransferDataFromWindow();

    wxRichTextStyleDefinition* def = wxRichTextFormattingDialog::GetDialogStyleDefinition(this);
    if (def)
    {
        wxRichTextParagraphStyleDefinition* paraDef = wxDynamicCast(def, wxRichTextParagraphStyleDefinition);
        if (paraDef)
            paraDef->SetNextStyle(m_nextStyle->GetValue());

        def->SetName(m_styleName->GetValue());
        def->SetBaseStyle(m_basedOn->GetValue());
    }

    return true;
}

bool wxRichTextStylePage::TransferDataToWindow()
{
    wxPanel::TransferDataToWindow();

    wxRichTextStyleDefinition* def = wxRichTextFormattingDialog::GetDialogStyleDefinition(this);
    if (def)
    {
        wxRichTextParagraphStyleDefinition* paraDef = wxDynamicCast(def, wxRichTextParagraphStyleDefinition);
        wxRichTextListStyleDefinition* listDef = wxDynamicCast(def, wxRichTextListStyleDefinition);
        // wxRichTextCharacterStyleDefinition* charDef = wxDynamicCast(def, wxRichTextCharacterStyleDefinition);
        wxRichTextStyleSheet* sheet = wxRichTextFormattingDialog::GetDialog(this)->GetStyleSheet();

        m_styleName->SetValue(def->GetName());

        if (listDef)
        {
            if (m_nextStyle->GetCount() == 0)
            {
                if (sheet)
                {
                    size_t i;
                    for (i = 0; i < sheet->GetListStyleCount(); i++)
                    {
                        wxRichTextListStyleDefinition* p = wxDynamicCast(sheet->GetListStyle(i), wxRichTextListStyleDefinition);
                        if (p)
                            m_nextStyle->Append(p->GetName());
                    }
                }
            }
            m_nextStyle->SetValue(listDef->GetNextStyle());
        }
        else if (paraDef)
        {
            if (m_nextStyle->GetCount() == 0)
            {
                if (sheet)
                {
                    size_t i;
                    for (i = 0; i < sheet->GetParagraphStyleCount(); i++)
                    {
                        wxRichTextParagraphStyleDefinition* p = wxDynamicCast(sheet->GetParagraphStyle(i), wxRichTextParagraphStyleDefinition);
                        if (p)
                            m_nextStyle->Append(p->GetName());
                    }
                }
            }
            m_nextStyle->SetValue(paraDef->GetNextStyle());
        }

        if (m_basedOn->GetCount() == 0)
        {
            if (sheet)
            {
                if (listDef)
                {
                    size_t i;
                    for (i = 0; i < sheet->GetListStyleCount(); i++)
                    {
                        wxRichTextListStyleDefinition* p = wxDynamicCast(sheet->GetListStyle(i), wxRichTextListStyleDefinition);
                        if (p)
                            m_basedOn->Append(p->GetName());
                    }
                }
                else if (paraDef)
                {
                    size_t i;
                    for (i = 0; i < sheet->GetParagraphStyleCount(); i++)
                    {
                        wxRichTextParagraphStyleDefinition* p = wxDynamicCast(sheet->GetParagraphStyle(i), wxRichTextParagraphStyleDefinition);
                        if (p)
                            m_basedOn->Append(p->GetName());
                    }
                }
                else
                {
                    size_t i;
                    for (i = 0; i < sheet->GetCharacterStyleCount(); i++)
                    {
                        wxRichTextCharacterStyleDefinition* p = wxDynamicCast(sheet->GetCharacterStyle(i), wxRichTextCharacterStyleDefinition);
                        if (p)
                            m_basedOn->Append(p->GetName());
                    }
                }
            }
        }

        m_basedOn->SetValue(def->GetBaseStyle());
    }

    return true;
}

wxTextAttrEx* wxRichTextStylePage::GetAttributes()
{
    return wxRichTextFormattingDialog::GetDialogAttributes(this);
}

/*!
 * Should we show tooltips?
 */

bool wxRichTextStylePage::ShowToolTips()
{
    return wxRichTextFormattingDialog::ShowToolTips();
}

/*!
 * Get bitmap resources
 */

wxBitmap wxRichTextStylePage::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxRichTextStylePage bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxRichTextStylePage bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxRichTextStylePage::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxRichTextStylePage icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxRichTextStylePage icon retrieval
}
/*!
 * wxEVT_UPDATE_UI event handler for ID_RICHTEXTSTYLEPAGE_NEXT_STYLE
 */

void wxRichTextStylePage::OnNextStyleUpdate( wxUpdateUIEvent& event )
{
    wxRichTextStyleDefinition* def = wxRichTextFormattingDialog::GetDialogStyleDefinition(this);
    event.Enable(def->IsKindOf(CLASSINFO(wxRichTextParagraphStyleDefinition)));
}

#endif // wxUSE_RICHTEXT
