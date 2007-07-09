/////////////////////////////////////////////////////////////////////////////
// Name:        src/x11/font.cpp
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: font.cpp,v 1.18 2005/08/22 21:58:32 MR Exp $
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
    #pragma implementation "font.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/font.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"       // for wxGetDisplay()
#include "wx/fontutil.h"    // for wxNativeFontInfo
#include "wx/tokenzr.h"
#include "wx/settings.h"

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the default size (in points) for the fonts
static const int wxDEFAULT_FONT_SIZE = 12;

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

void wxFont::Init()
{
}

wxFont::wxFont(const wxNativeFontInfo& info)
{
    Init();
    (void) Create(info.GetBFontName());
}

bool wxFont::Create(int pointSize,
                    int family,
                    int style,
                    int weight,
                    bool underlined,
                    const wxString& faceName,
                    wxFontEncoding encoding)
{
    UnRef();
	
//    m_refData = new wxFontRefData(pointSize, family, style, weight,
//                                  underlined, faceName, encoding);

    return FALSE;
}

bool wxFont::Create(const wxString& fontname, wxFontEncoding enc)
{
}

wxFont::~wxFont()
{
}

// ----------------------------------------------------------------------------
// change the font attributes
// ----------------------------------------------------------------------------

void wxFont::Unshare()
{
    // Don't change shared data
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

int wxFont::GetPointSize() const
{
	wxCHECK_MSG( Ok(), 0, wxT("invalid font") );
	return fnt->Size();
}

wxString wxFont::GetFaceName() const
{
	wxCHECK_MSG( Ok(), wxT(""), wxT("invalid font") );
//knorr?? face??
//	return fnt->Face();
}

int wxFont::GetFamily() const
{
	wxCHECK_MSG( Ok(), 0, wxT("invalid font") );
	return fnt->FamilyAndStyle();
}

int wxFont::GetStyle() const
{
	wxCHECK_MSG( Ok(), 0, wxT("invalid font") );	
	return fnt->FamilyAndStyle();
}

int wxFont::GetWeight() const
{
	 wxCHECK_MSG( Ok(), 0, wxT("invalid font") );	
//knorr?? weight??
//	 return fnt->;
}

bool wxFont::GetUnderlined() const
{
	wxCHECK_MSG( Ok(), FALSE, wxT("invalid font") );
//knorr??
//	return fnt->;
}

wxFontEncoding wxFont::GetEncoding() const
{
	wxCHECK_MSG( Ok(), wxFONTENCODING_DEFAULT, wxT("invalid font") );
	return (wxFontEncoding)fnt->Encoding();
}

bool wxFont::GetNoAntiAliasing() const
{
    wxCHECK_MSG( Ok(), wxFONTENCODING_DEFAULT, wxT("invalid font") );
//always antialiased
    return false;
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
//	wxCHECK_MSG( Ok(), (wxNativeFontInfo *)NULL, wxT("invalid font") );
//	BFont * res = new BFont(fnt);
//	return res;
}

bool wxFont::IsFixedWidth() const
{
	wxCHECK_MSG( Ok(), FALSE, wxT("invalid font") );
	return fnt->IsFixed();
}

// ----------------------------------------------------------------------------
// change font attributes
// ----------------------------------------------------------------------------

void wxFont::SetPointSize(int pointSize)
{
	Unshare();
	fnt->SetSize(pointSize);
}

void wxFont::SetFamily(int family)
{
	Unshare();
	fnt->SetFamilyAndStyle(family);
}

void wxFont::SetStyle(int style)
{
	Unshare();
	fnt->SetFamilyAndStyle(style);
}

void wxFont::SetWeight(int weight)
{
//    Unshare();
//
//    M_FONTDATA->SetWeight(weight);
}

void wxFont::SetFaceName(const wxString& faceName)
{
//	Unshare();	
//	M_FONTDATA->SetFaceName(faceName);
}

void wxFont::SetUnderlined(bool underlined)
{
//    Unshare();
//
//    M_FONTDATA->SetUnderlined(underlined);
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
	Unshare();	
	fnt->SetEncoding(encoding);
}

void wxFont::DoSetNativeFontInfo( const wxNativeFontInfo& info )
{
	Unshare();	
//	fnt = new BFont(info);
}

void wxFont::SetNoAntiAliasing( bool no )
{
}

//knorr!!
//wxNativeFontInfo
void wxNativeFontInfo::Init()
{
}

bool wxNativeFontInfo::FromString(const wxString &str)
{
	return false;
}

wxString wxNativeFontInfo::ToString() const
{	
}

wxString wxNativeFontInfo::ToUserString() const
{	
}

bool wxNativeFontInfo::FromUserString(const wxString &str)
{
	return false;
}


void wxNativeFontInfo::SetPointSize(int pointsize)
{
}

void wxNativeFontInfo::SetPixelSize(const wxSize& pixelSize)
{
}

void wxNativeFontInfo::SetStyle(wxFontStyle style)
{
}

void wxNativeFontInfo::SetWeight(wxFontWeight weight)
{
}

void wxNativeFontInfo::SetUnderlined(bool underlined)
{
}

void wxNativeFontInfo::SetFaceName(wxString facename)
{
}

void wxNativeFontInfo::SetFamily(wxFontFamily family)
{
}

void wxNativeFontInfo::SetEncoding(wxFontEncoding encoding)
{
}


wxString wxNativeFontInfo::GetBFontName() const
{
}