///////////////////////////////////////////////////////////////////////////////
// Name:        msw/fontutil.cpp
// Purpose:     font-related helper functions for wxMSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.11.99
// RCS-ID:      $Id: fontutil.cpp,v 1.34 2005/06/16 14:16:50 DS Exp $
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "fontutil.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/encinfo.h"
#endif //WX_PRECOMP

#include "wx/fontutil.h"
#include "wx/fontmap.h"

#include "wx/tokenzr.h"

// for MSVC5 and old w32api
#ifndef HANGUL_CHARSET
#    define HANGUL_CHARSET  129
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNativeEncodingInfo
// ----------------------------------------------------------------------------

// convert to/from the string representation: format is
//      encodingid;facename[;charset]

bool wxNativeEncodingInfo::FromString(const wxString& s)
{
    return false;
}

wxString wxNativeEncodingInfo::ToString() const
{
    wxString s;
    return s;
}

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

bool wxGetNativeFontEncoding(wxFontEncoding encoding,
                             wxNativeEncodingInfo *info)
{
    wxCHECK_MSG( info, false, _T("bad pointer in wxGetNativeFontEncoding") );

    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        encoding = wxFont::GetDefaultEncoding();
    }

    extern WXDLLIMPEXP_BASE long wxEncodingToCharset(wxFontEncoding encoding);

    info->encoding = encoding;

    return true;
}

bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
{
    // try to create such font
    return false;
}

// ----------------------------------------------------------------------------
// wxFontEncoding <-> CHARSET_XXX
// ----------------------------------------------------------------------------

wxFontEncoding wxGetFontEncFromCharSet(int cs)
{
}

// ----------------------------------------------------------------------------
// wxFont <-> LOGFONT conversion
// ----------------------------------------------------------------------------

//void wxFillLogFont(LOGFONT *logFont, const wxFont *font)
//{
//    wxNativeFontInfo fi;
//
//    // maybe we already have LOGFONT for this font?
//    const wxNativeFontInfo *pFI = font->GetNativeFontInfo();
//    if ( !pFI )
//    {
//        // use wxNativeFontInfo methods to build a LOGFONT for this font
//        fi.InitFromFont(*font);
//
//        pFI = &fi;
//    }
//
//    // transfer all the data to LOGFONT
//    *logFont = pFI->lf;
//}

//wxFont wxCreateFontFromLogFont(const LOGFONT *logFont)
//{
//    wxNativeFontInfo info;
//
//    info.lf = *logFont;
//
//    return wxFont(info);
//}

