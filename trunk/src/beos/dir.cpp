/////////////////////////////////////////////////////////////////////////////
// Name:        msw/dir.cpp
// Purpose:     wxDir implementation for Win32
// Author:      Vadim Zeitlin
// Modified by:
// Created:     08.12.99
// RCS-ID:      $Id: dir.cpp,v 1.18 2005/06/10 17:53:16 ABX Exp $
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "dir.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif // PCH

#include "wx/dir.h"
#include "wx/filefn.h"          // for wxDirExists()

//#ifdef __WINDOWS__
//    #include "wx/msw/private.h"
//#endif

#ifdef __BEOS__
#include <Entry.h>
#include <Directory.h>
#endif

// ----------------------------------------------------------------------------
// define the types and functions used for file searching
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define M_DIR       ((wxDirData *)m_data)

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// this class stores everything we need to enumerate the files
class wxDirData
{
public:
	wxDirData(const wxString& dirname);
	~wxDirData();
	
	void SetFileSpec(const wxString& filespec) { m_filespec = filespec; }
	void SetFlags(int flags) { m_flags = flags; }
	
	void Close();
	void Rewind();
	bool Read(wxString *filename);
	
	const wxString& GetName() const { return m_dirname; }

private:
	BDirectory * bDirectory;
	BEntry * bEntry;

	wxString m_dirname;
	wxString m_filespec;

	int      m_flags;

	DECLARE_NO_COPY_CLASS(wxDirData)
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDirData
// ----------------------------------------------------------------------------

wxDirData::wxDirData(const wxString& dirname)
         : m_dirname(dirname)
{
	m_dirname = dirname;
}

wxDirData::~wxDirData()
{
	Close();
}

void wxDirData::Close()
{
	if(bDirectory) delete bDirectory;
	if(bEntry) delete bEntry;
}

void wxDirData::Rewind()
{
	Close();
}

bool wxDirData::Read(wxString *filename)
{
	status_t err;
	if(!bDirectory)
	{
		bDirectory = new BDirectory(m_dirname.c_str());
		if(!bEntry) bEntry = new BEntry();
		if((err=bDirectory->FindEntry("*", bEntry))!=B_OK)
		{
			wxLogSysError(err, _("Can not enumerate files in directory '%s'"), m_dirname.c_str());		
			return false;
		}
	} else
	{
		err = bDirectory->GetNextEntry(bEntry);
		if(err!=B_OK)
		{
			wxLogLastError(_T("FindNext "));
			return false;
		}	
	}

	char name[B_FILE_NAME_LENGTH];
	bEntry->GetName(name);
	*filename=name;
	return true;
}

// ----------------------------------------------------------------------------
// wxDir helpers
// ----------------------------------------------------------------------------

/* static */
bool wxDir::Exists(const wxString& dir)
{
    return wxDirExists(dir);
}

// ----------------------------------------------------------------------------
// wxDir construction/destruction
// ----------------------------------------------------------------------------

wxDir::wxDir(const wxString& dirname)
{
    m_data = NULL;

    (void)Open(dirname);
}

bool wxDir::Open(const wxString& dirname)
{
    delete M_DIR;
    m_data = new wxDirData(dirname);

    return true;
}

bool wxDir::IsOpened() const
{
    return m_data != NULL;
}

wxString wxDir::GetName() const
{
	wxString name;
	if ( m_data )
		name = M_DIR->GetName();
	return name;
}

wxDir::~wxDir()
{
    delete M_DIR;
}

// ----------------------------------------------------------------------------
// wxDir enumerating
// ----------------------------------------------------------------------------

bool wxDir::GetFirst(wxString *filename,
                     const wxString& filespec,
                     int flags) const
{
	wxCHECK_MSG( IsOpened(), false, _T("must wxDir::Open() first") );

	M_DIR->Rewind();

	M_DIR->SetFileSpec(filespec);
	M_DIR->SetFlags(flags);
	
	return GetNext(filename);
}

bool wxDir::GetNext(wxString *filename) const
{
    wxCHECK_MSG( IsOpened(), false, _T("must wxDir::Open() first") );

    wxCHECK_MSG( filename, false, _T("bad pointer in wxDir::GetNext()") );

    return M_DIR->Read(filename);
}

// ----------------------------------------------------------------------------
// wxGetDirectoryTimes: used by wxFileName::GetTimes()
// ----------------------------------------------------------------------------

