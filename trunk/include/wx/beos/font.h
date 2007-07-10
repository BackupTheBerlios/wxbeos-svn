/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: font.h,v 1.11 2005/08/02 18:16:49 MW Exp $
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONT_H_
#define _WX_FONT_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "font.h"
#endif

// Font
class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { Init(); }
    wxFont(const wxFont& font) { Init(); Ref(font); }

    wxFont(int size,
        int family,
        int style,
        int weight,
        bool underlined = FALSE,
        const wxString& face = wxEmptyString,
        wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        Init();

        (void)Create(size, family, style, weight, underlined, face, encoding);
    }

    wxFont(const wxNativeFontInfo& info);

    bool Create(int size,
        int family,
        int style,
        int weight,
        bool underlined = FALSE,
        const wxString& face = wxEmptyString,
        wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    // FIXME: I added the ! to make it compile;
    // is this right? - JACS
#if !wxUSE_UNICODE
    bool Create(const wxString& fontname,
        wxFontEncoding fontenc = wxFONTENCODING_DEFAULT);
#endif
    // DELETEME: no longer seems to be implemented.
    // bool Create(const wxNativeFontInfo& fontinfo);

    virtual ~wxFont();

    // assignment
    wxFont& operator=(const wxFont& font);

    // implement base class pure virtuals
    virtual int GetPointSize() const;
    virtual int GetFamily() const;
    virtual int GetStyle() const;
    virtual int GetWeight() const;
    virtual bool GetUnderlined() const;
    virtual wxString GetFaceName() const;
    virtual wxFontEncoding GetEncoding() const;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const;

    virtual bool IsFixedWidth() const;

    virtual void SetPointSize(int pointSize);
    virtual void SetFamily(int family);
    virtual void SetStyle(int style);
    virtual void SetWeight(int weight);
    virtual void SetFaceName(const wxString& faceName);
    virtual void SetUnderlined(bool underlined);
    virtual void SetEncoding(wxFontEncoding encoding);

    virtual void SetNoAntiAliasing( bool no = TRUE );
    virtual bool GetNoAntiAliasing() const ;

    // Implementation

protected:
    virtual void DoSetNativeFontInfo( const wxNativeFontInfo& info );

    // common part of all ctors
    void Init();

    void Unshare();
	BFont * fnt;

private:
    DECLARE_DYNAMIC_CLASS(wxFont)
};

#endif
// _WX_FONT_H_
