///////////////////////////////////////////////////////////////////////////////
// Name:        unix/stdpaths.cpp
// Purpose:     wxStandardPaths implementation for Unix & OpenVMS systems
// Author:      Vadim Zeitlin
// Modified by:
// Created:     2004-10-19
// RCS-ID:      $Id: stdpaths.cpp,v 1.4 2005/03/11 09:33:29 JJ Exp $
// Copyright:   (c) 2004 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STDPATHS

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif //WX_PRECOMP

#include "wx/filename.h"

#include "wx/stdpaths.h"

#if defined( __LINUX__ ) || defined( __VMS )
    #include <unistd.h>
#endif

// ============================================================================
// wxStandardPaths implementation
// ============================================================================

// ----------------------------------------------------------------------------
// prefix management
// ----------------------------------------------------------------------------

void wxStandardPaths::SetInstallPrefix(const wxString& prefix)
{
    m_prefix = prefix;
}

wxString wxStandardPaths::GetInstallPrefix() const
{
	if ( m_prefix.empty() )
    	{
		wxStandardPaths *self = wx_const_cast(wxStandardPaths *, this);
		self->m_prefix = _T("/boot/home/config");
	}
	return m_prefix;	
}

// ----------------------------------------------------------------------------
// public functions
// ----------------------------------------------------------------------------

wxString wxStandardPaths::GetConfigDir() const
{
   return _T("/boot/home/config/settings");
}

wxString wxStandardPaths::GetUserConfigDir() const
{
    return wxFileName::GetHomeDir();
}

wxString wxStandardPaths::GetDataDir() const
{
   return wxString("");
}

wxString wxStandardPaths::GetLocalDataDir() const
{
	return AppendAppName(GetInstallPrefix() + _T("/settings"));
}

wxString wxStandardPaths::GetUserDataDir() const
{
	return GetDataDir();
}

wxString wxStandardPaths::GetPluginsDir() const
{
	return AppendAppName(GetInstallPrefix() + _T("/add-ons"));
}

#endif // wxUSE_STDPATHS
