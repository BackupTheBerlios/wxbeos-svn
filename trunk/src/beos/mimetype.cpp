/////////////////////////////////////////////////////////////////////////////
// Name:        unix/mimetype.cpp
// Purpose:     classes and functions to manage MIME types
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.09.98
// RCS-ID:      $Id: mimetype.cpp,v 1.53.2.3 2006/03/17 15:05:03 RR Exp $
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

/*
    TODO: this file is a mess, we need to split it and reformet/review
          everything (VZ)
 */

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "mimetype.h"
#endif

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/defs.h"
#endif

#if wxUSE_MIMETYPE && wxUSE_FILE && wxUSE_TEXTFILE

#ifndef WX_PRECOMP
  #include "wx/string.h"
#endif //WX_PRECOMP


#include "wx/log.h"
#include "wx/file.h"
#include "wx/intl.h"
#include "wx/dynarray.h"
#include "wx/confbase.h"

#include "wx/ffile.h"
#include "wx/textfile.h"
#include "wx/dir.h"
#include "wx/utils.h"
#include "wx/tokenzr.h"
#include "wx/iconloc.h"
#include "wx/filename.h"

#include "wx/beos/mimetype.h"
#include <Mime.h>

// other standard headers
#include <ctype.h>

#ifdef __VMS
/* silence warnings for comparing unsigned int's <0 */
# pragma message disable unscomzer
#endif

// wxMimeTypeCommands stores the verbs defined for the given MIME type with
// their values
class wxMimeTypeCommands
{
public:
    wxMimeTypeCommands() { }

    wxMimeTypeCommands(const wxArrayString& verbs,
                       const wxArrayString& commands)
        : m_verbs(verbs),
          m_commands(commands)
    {
    }

    // add a new verb with the command or replace the old value
    void AddOrReplaceVerb(const wxString& verb, const wxString& cmd)
    {
        int n = m_verbs.Index(verb, false /* ignore case */);
        if ( n == wxNOT_FOUND )
        {
            m_verbs.Add(verb);
            m_commands.Add(cmd);
        }
        else
        {
            m_commands[n] = cmd;
        }
    }

    void Add(const wxString& s)
    {
        m_verbs.Add(s.BeforeFirst(_T('=')));
        m_commands.Add(s.AfterFirst(_T('=')));
    }

    // access the commands
    size_t GetCount() const { return m_verbs.GetCount(); }
    const wxString& GetVerb(size_t n) const { return m_verbs[n]; }
    const wxString& GetCmd(size_t n) const { return m_commands[n]; }

    bool HasVerb(const wxString& verb) const
        { return m_verbs.Index(verb) != wxNOT_FOUND; }

    wxString GetCommandForVerb(const wxString& verb, size_t *idx = NULL) const
    {
        wxString s;

        int n = m_verbs.Index(verb);
        if ( n != wxNOT_FOUND )
        {
            s = m_commands[(size_t)n];
            if ( idx )
                *idx = n;
        }
        else if ( idx )
        {
            // different from any valid index
            *idx = (size_t)-1;
        }

        return s;
    }

    // get a "verb=command" string
    wxString GetVerbCmd(size_t n) const
    {
        return m_verbs[n] + _T('=') + m_commands[n];
    }

private:
    wxArrayString m_verbs,
                  m_commands;
};

// this class extends wxTextFile
//
// VZ: ???
class wxMimeTextFile : public wxTextFile
{
public:
    // constructors
    wxMimeTextFile () : wxTextFile () {};
    wxMimeTextFile (const wxString& strFile) : wxTextFile (strFile)  {  };

    int pIndexOf(const wxString & sSearch, bool bIncludeComments = false, int iStart = 0)
    {
        size_t i = iStart;
        int nResult = wxNOT_FOUND;
        if (i>=GetLineCount()) return wxNOT_FOUND;

        wxString sTest = sSearch;
        sTest.MakeLower();
        wxString sLine;

        if (bIncludeComments)
        {
            while ( (i < GetLineCount())   )
            {
                sLine = GetLine (i);
                sLine.MakeLower();
                if (sLine.Contains(sTest)) nResult = (int) i;
                i++;
            }
        }
        else
        {
            while ( (i < GetLineCount()) )
            {
                sLine = GetLine (i);
                sLine.MakeLower();
                if ( ! sLine.StartsWith(wxT("#")))
                {
                    if (sLine.Contains(sTest)) nResult = (int) i;
                }
                i++;
            }
        }
        return  nResult;
    }

    bool CommentLine(int nIndex)
    {
        if (nIndex <0) return false;
        if (nIndex >= (int)GetLineCount() ) return false;
        GetLine(nIndex) = GetLine(nIndex).Prepend(wxT("#"));
        return true;
    }

    bool CommentLine(const wxString & sTest)
    {
        int nIndex = pIndexOf(sTest);
        if (nIndex <0) return false;
        if (nIndex >= (int)GetLineCount() ) return false;
        GetLine(nIndex) = GetLine(nIndex).Prepend(wxT("#"));
        return true;
    }

    wxString GetVerb (size_t i)
    {
        if (i > GetLineCount() ) return wxEmptyString;
        wxString sTmp = GetLine(i).BeforeFirst(wxT('='));
        return sTmp;
    }

    wxString GetCmd (size_t i)
    {
        if (i > GetLineCount() ) return wxEmptyString;
        wxString sTmp = GetLine(i).AfterFirst(wxT('='));
        return sTmp;
    }
};

// in case we're compiling in non-GUI mode
class WXDLLEXPORT wxIcon;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// MIME code tracing mask
#define TRACE_MIME _T("mime")

// give trace messages about the results of mailcap tests
#define TRACE_MIME_TEST _T("mimetest")

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// there are some fields which we don't understand but for which we don't give
// warnings as we know that they're not important - this function is used to
// test for them
static bool IsKnownUnimportantField(const wxString& field);

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------


// This class uses both mailcap and mime.types to gather information about file
// types.
//
// The information about mailcap file was extracted from metamail(1) sources
// and documentation and subsequently revised when I found the RFC 1524
// describing it.
//
// Format of mailcap file: spaces are ignored, each line is either a comment
// (starts with '#') or a line of the form <field1>;<field2>;...;<fieldN>.
// A backslash can be used to quote semicolons and newlines (and, in fact,
// anything else including itself).
//
// The first field is always the MIME type in the form of type/subtype (see RFC
// 822) where subtype may be '*' meaning "any". Following metamail, we accept
// "type" which means the same as "type/*", although I'm not sure whether this
// is standard.
//
// The second field is always the command to run. It is subject to
// parameter/filename expansion described below.
//
// All the following fields are optional and may not be present at all. If
// they're present they may appear in any order, although each of them should
// appear only once. The optional fields are the following:
//  * notes=xxx is an uninterpreted string which is silently ignored
//  * test=xxx is the command to be used to determine whether this mailcap line
//    applies to our data or not. The RHS of this field goes through the
//    parameter/filename expansion (as the 2nd field) and the resulting string
//    is executed. The line applies only if the command succeeds, i.e. returns 0
//    exit code.
//  * print=xxx is the command to be used to print (and not view) the data of
//    this type (parameter/filename expansion is done here too)
//  * edit=xxx is the command to open/edit the data of this type
//  * needsterminal means that a new interactive console must be created for
//    the viewer
//  * copiousoutput means that the viewer doesn't interact with the user but
//    produces (possibly) a lof of lines of output on stdout (i.e. "cat" is a
//    good example), thus it might be a good idea to use some kind of paging
//    mechanism.
//  * textualnewlines means not to perform CR/LF translation (not honored)
//  * compose and composetyped fields are used to determine the program to be
//    called to create a new message pert in the specified format (unused).
//
// Parameter/filename expansion:
//  * %s is replaced with the (full) file name
//  * %t is replaced with MIME type/subtype of the entry
//  * for multipart type only %n is replaced with the nnumber of parts and %F is
//    replaced by an array of (content-type, temporary file name) pairs for all
//    message parts (TODO)
//  * %{parameter} is replaced with the value of parameter taken from
//    Content-type header line of the message.
//
//
// There are 2 possible formats for mime.types file, one entry per line (used
// for global mime.types and called Mosaic format) and "expanded" format where
// an entry takes multiple lines (used for users mime.types and called
// Netscape format).
//
// For both formats spaces are ignored and lines starting with a '#' are
// comments. Each record has one of two following forms:
//  a) for "brief" format:
//      <mime type>  <space separated list of extensions>
//  b) for "expanded" format:
//      type=<mime type> BACKSLASH
//      desc="<description>" BACKSLASH
//      exts="<comma separated list of extensions>"
//
// (where BACKSLASH is a literal '\\' which we can't put here because cpp
// misinterprets it)
//
// We try to autodetect the format of mime.types: if a non-comment line starts
// with "type=" we assume the second format, otherwise the first one.

// there may be more than one entry for one and the same mime type, to
// choose the right one we have to run the command specified in the test
// field on our data.

// ----------------------------------------------------------------------------
// wxFileTypeImpl (Unix)
// ----------------------------------------------------------------------------

wxString wxFileTypeImpl::GetExpandedCommand(const wxString & verb, const wxFileType::MessageParameters& params) const
{
    wxString sTmp;
    size_t i = 0;
    while ( (i < m_index.GetCount() ) && sTmp.empty() )
    {
            sTmp = m_manager->GetCommand ( verb, m_index[i] );
            i ++;
    }

    return wxFileType::ExpandCommand(sTmp, params);
}

bool wxFileTypeImpl::GetIcon(wxIconLocation *iconLoc) const
{
//get icon for this file type
    return true;
}


bool
wxFileTypeImpl::GetMimeTypes(wxArrayString& mimeTypes) const
{
//get mime types for this file type
    mimeTypes.Clear();
//    for (size_t i = 0; i < m_index.GetCount(); i++)
//        mimeTypes.Add(m_manager->m_aTypes[m_index[i]]);
    return true;
}


size_t wxFileTypeImpl::GetAllCommands(wxArrayString *verbs,
                                  wxArrayString *commands,
                                  const wxFileType::MessageParameters& params) const
{

    wxString vrb, cmd, sTmp;
    size_t count = 0;
    wxMimeTypeCommands * sPairs;

    // verbs and commands have been cleared already in mimecmn.cpp...
    // if we find no entries in the exact match, try the inexact match
    for (size_t n = 0; ((count ==0) && (n < m_index.GetCount())); n++)
    {
        // list of verb = command pairs for this mimetype
        sPairs = m_manager->m_aEntries [m_index[n]];
        size_t i;
        for ( i = 0; i < sPairs->GetCount (); i++ )
            {
                vrb = sPairs->GetVerb(i);
                // some gnome entries have . inside
                vrb = vrb.AfterLast(wxT('.'));
                cmd = sPairs->GetCmd (i);
                if (! cmd.empty() )
                     {
                     cmd = wxFileType::ExpandCommand(cmd, params);
                     count ++;
                     if ( vrb.IsSameAs (wxT("open")))
                         {
                         verbs->Insert(vrb,0u);
                         commands ->Insert(cmd,0u);
                         }
                     else
                         {
                         verbs->Add (vrb);
                         commands->Add (cmd);
                         }
                     }

        }

    }
    return count;

}

bool wxFileTypeImpl::GetExtensions(wxArrayString& extensions)
{
    wxString strExtensions = m_manager->GetExtension(m_index[0]);
    extensions.Empty();

    // one extension in the space or comma delimitid list
    wxString strExt;
    for ( const wxChar *p = strExtensions;; p++ ) {
        if ( *p == wxT(' ') || *p == wxT(',') || *p == wxT('\0') ) {
            if ( !strExt.empty() ) {
                extensions.Add(strExt);
                strExt.Empty();
            }
            //else: repeated spaces (shouldn't happen, but it's not that
            //      important if it does happen)

            if ( *p == wxT('\0') )
                break;
        }
        else if ( *p == wxT('.') ) {
            // remove the dot from extension (but only if it's the first char)
            if ( !strExt.empty() ) {
                strExt += wxT('.');
            }
            //else: no, don't append it
        }
        else {
            strExt += *p;
        }
    }

    return true;
}

// set an arbitrary command,
// could adjust the code to ask confirmation if it already exists and
// overwriteprompt is true, but this is currently ignored as *Associate* has
// no overwrite prompt
bool wxFileTypeImpl::SetCommand(const wxString& cmd, const wxString& verb, bool overwriteprompt /*= true*/)
{
    wxArrayString strExtensions;
    wxString strDesc, strIcon;

    wxMimeTypeCommands *entry = new wxMimeTypeCommands ();
    entry->Add(verb + wxT("=")  + cmd + wxT(" %s "));

    wxArrayString strTypes;
    GetMimeTypes (strTypes);
    if (strTypes.GetCount() < 1) return false;

    size_t i;
    bool Ok = true;
    for (i = 0; i < strTypes.GetCount(); i++)
    {
        if (!m_manager->DoAssociation (strTypes[i], strIcon, entry, strExtensions, strDesc))
            Ok = false;
    }

    return Ok;
}

// ignore index on the grouds that we only have one icon in a Unix file
bool wxFileTypeImpl::SetDefaultIcon(const wxString& strIcon /*= wxEmptyString*/, int /*index = 0*/)
{
    if (strIcon.empty()) return false;
    wxArrayString strExtensions;
    wxString strDesc;

    wxMimeTypeCommands *entry = new wxMimeTypeCommands ();

    wxArrayString strTypes;
    GetMimeTypes (strTypes);
    if (strTypes.GetCount() < 1) return false;

    size_t i;
    bool Ok = true;
    for (i = 0; i < strTypes.GetCount(); i++)
    {
        if (!m_manager->DoAssociation (strTypes[i], strIcon, entry, strExtensions, strDesc))
            Ok = false;
    }

    return Ok;
}

// ----------------------------------------------------------------------------
// wxMimeTypesManagerImpl (Unix)
// ----------------------------------------------------------------------------


wxMimeTypesManagerImpl::wxMimeTypesManagerImpl()
{
    m_initialized = false;
    m_mailcapStylesInited = 0;
}

void wxMimeTypesManagerImpl::InitIfNeeded()
{
	if ( !m_initialized )
	{
		// set the flag first to prevent recursion
		m_initialized = true;
		bMime = new BMimeType();			
	}
}


// clear data so you can read another group of WM files
void wxMimeTypesManagerImpl::ClearData()
{
	if(bMime) delete bMime;
}

wxMimeTypesManagerImpl::~wxMimeTypesManagerImpl()
{
    ClearData();
}

bool wxMimeTypesManagerImpl::WriteToMimeTypes (int index, bool delete_index)
{
//update mime type??	
	return false;
}

wxFileType * wxMimeTypesManagerImpl::Associate(const wxFileTypeInfo& ftInfo)
{
//associate this file type with ftinfo
	InitIfNeeded();
//	return GetFileTypeFromMimeType(strType);
}


bool wxMimeTypesManagerImpl::DoAssociation(const wxString& strType,
                                           const wxString& strIcon,
                                           wxMimeTypeCommands *entry,
                                           const wxArrayString& strExtensions,
                                           const wxString& strDesc)
{
//what about a icon??
    int nIndex = AddToMimeData(strType, strIcon, entry, strExtensions, strDesc, true);
    return WriteMimeInfo (nIndex, false);
}

bool wxMimeTypesManagerImpl::WriteMimeInfo(int nIndex, bool delete_mime )
{
    bool ok = false;
    return ok;
}

int wxMimeTypesManagerImpl::AddToMimeData(const wxString& strType,
                                          const wxString& strIcon,
                                          wxMimeTypeCommands *entry,
                                          const wxArrayString& strExtensions,
                                          const wxString& strDesc,
                                          bool replaceExisting)
{
	InitIfNeeded();
	return 0;
}


wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromExtension(const wxString& ext)
{
	if (ext.empty() )
		return NULL;
	return NULL;
}

wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromMimeType(const wxString& mimeType)
{
    InitIfNeeded();

    wxFileType * fileType = NULL;
    // mime types are not case-sensitive
    wxString mimetype(mimeType);
    mimetype.MakeLower();
    return fileType;
}


wxString wxMimeTypesManagerImpl::GetCommand(const wxString & verb, size_t nIndex) const
{
    wxString command, testcmd, sV, sTmp;
    sV = verb + wxT("=");
    // list of verb = command pairs for this mimetype
    wxMimeTypeCommands * sPairs = m_aEntries [nIndex];

    size_t i;
    for ( i = 0; i < sPairs->GetCount (); i++ )
    {
        sTmp = sPairs->GetVerbCmd (i);
        if ( sTmp.Contains(sV) )
            command = sTmp.AfterFirst(wxT('='));
    }
    return command;
}

void wxMimeTypesManagerImpl::AddFallback(const wxFileTypeInfo& filetype)
{
    InitIfNeeded();

    wxString extensions;
    const wxArrayString& exts = filetype.GetExtensions();
    size_t nExts = exts.GetCount();
    for ( size_t nExt = 0; nExt < nExts; nExt++ ) {
        if ( nExt > 0 ) {
            extensions += wxT(' ');
        }
        extensions += exts[nExt];
    }

    AddMimeTypeInfo(filetype.GetMimeType(),
                    extensions,
                    filetype.GetDescription());

    AddMailcapInfo(filetype.GetMimeType(),
                   filetype.GetOpenCommand(),
                   filetype.GetPrintCommand(),
                   wxT(""),
                   filetype.GetDescription());
}

void wxMimeTypesManagerImpl::AddMimeTypeInfo(const wxString& strMimeType,
                                             const wxString& strExtensions,
                                             const wxString& strDesc)
{
    // reading mailcap may find image/* , while
    // reading mime.types finds image/gif and no match is made
    // this means all the get functions don't work  fix this
    wxString strIcon;
    wxString sTmp = strExtensions;

    wxArrayString sExts;
    sTmp.Trim().Trim(false);

    while (!sTmp.empty())
    {
        sExts.Add (sTmp.AfterLast(wxT(' ')));
        sTmp = sTmp.BeforeLast(wxT(' '));
    }

    AddToMimeData (strMimeType, strIcon, NULL, sExts, strDesc, true);
}

void wxMimeTypesManagerImpl::AddMailcapInfo(const wxString& strType,
                                            const wxString& strOpenCmd,
                                            const wxString& strPrintCmd,
                                            const wxString& strTest,
                                            const wxString& strDesc)
{
    InitIfNeeded();

    wxMimeTypeCommands *entry = new wxMimeTypeCommands;
    entry->Add(wxT("open=")  + strOpenCmd);
    entry->Add(wxT("print=") + strPrintCmd);
    entry->Add(wxT("test=")  + strTest);

    wxString strIcon;
    wxArrayString strExtensions;

    AddToMimeData (strType, strIcon, entry, strExtensions, strDesc, true);

}

bool wxMimeTypesManagerImpl::ReadMimeTypes(const wxString& strFileName)
{
    wxLogTrace(TRACE_MIME, wxT("--- Parsing mime.types file '%s' ---"),
               strFileName.c_str());

    wxTextFile file(strFileName);
#if defined(__WXGTK20__) && wxUSE_UNICODE
    if ( !file.Open( wxConvUTF8) )
#else
    if ( !file.Open() )
#endif
        return false;

    // the information we extract
    wxString strMimeType, strDesc, strExtensions;

    size_t nLineCount = file.GetLineCount();
    const wxChar *pc = NULL;
    for ( size_t nLine = 0; nLine < nLineCount; nLine++ )
    {
        if ( pc == NULL ) {
            // now we're at the start of the line
            pc = file[nLine].c_str();
        }
        else {
            // we didn't finish with the previous line yet
            nLine--;
        }

        // skip whitespace
        while ( wxIsspace(*pc) )
            pc++;

        // comment or blank line?
        if ( *pc == wxT('#') || !*pc ) {
            // skip the whole line
            pc = NULL;
            continue;
        }

        // detect file format
        const wxChar *pEqualSign = wxStrchr(pc, wxT('='));
        if ( pEqualSign == NULL ) {
            // brief format
            // ------------

            // first field is mime type
            for ( strMimeType.Empty(); !wxIsspace(*pc) && *pc != wxT('\0'); pc++ ) {
                strMimeType += *pc;
            }

            // skip whitespace
            while ( wxIsspace(*pc) )
                pc++;

            // take all the rest of the string
            strExtensions = pc;

            // no description...
            strDesc.Empty();
        }
        else {
            // expanded format
            // ---------------

            // the string on the left of '=' is the field name
            wxString strLHS(pc, pEqualSign - pc);

            // eat whitespace
            for ( pc = pEqualSign + 1; wxIsspace(*pc); pc++ )
              ;

            const wxChar *pEnd;
            if ( *pc == wxT('"') ) {
                // the string is quoted and ends at the matching quote
                pEnd = wxStrchr(++pc, wxT('"'));
                if ( pEnd == NULL ) {
                    wxLogWarning(_("Mime.types file %s, line %d: unterminated quoted string."),
                                 strFileName.c_str(), nLine + 1);
                }
            }
            else {
                // unquoted string ends at the first space or at the end of
                // line
                for ( pEnd = pc; *pEnd && !wxIsspace(*pEnd); pEnd++ )
                  ;
            }

            // now we have the RHS (field value)
            wxString strRHS(pc, pEnd - pc);

            // check what follows this entry
            if ( *pEnd == wxT('"') ) {
                // skip this quote
                pEnd++;
            }

            for ( pc = pEnd; wxIsspace(*pc); pc++ )
              ;

            // if there is something left, it may be either a '\\' to continue
            // the line or the next field of the same entry
            bool entryEnded = *pc == wxT('\0'),
                 nextFieldOnSameLine = false;
            if ( !entryEnded ) {
                nextFieldOnSameLine = ((*pc != wxT('\\')) || (pc[1] != wxT('\0')));
            }

            // now see what we got
            if ( strLHS == wxT("type") ) {
                strMimeType = strRHS;
            }
            else if ( strLHS.StartsWith(wxT("desc")) ) {
                strDesc = strRHS;
            }
            else if ( strLHS == wxT("exts") ) {
                strExtensions = strRHS;
            }
            else if ( strLHS == _T("icon") )
            {
                // this one is simply ignored: it usually refers to Netscape
                // built in icons which are useless for us anyhow
            }
            else if ( !strLHS.StartsWith(_T("x-")) )
            {
                // we suppose that all fields starting with "X-" are
                // unregistered extensions according to the standard practice,
                // but it may be worth telling the user about other junk in
                // his mime.types file
                wxLogWarning(_("Unknown field in file %s, line %d: '%s'."),
                             strFileName.c_str(), nLine + 1, strLHS.c_str());
            }

            if ( !entryEnded ) {
                if ( !nextFieldOnSameLine )
                    pc = NULL;
                //else: don't reset it

                // as we don't reset strMimeType, the next field in this entry
                // will be interpreted correctly.

                continue;
            }
        }

        // depending on the format (Mosaic or Netscape) either space or comma
        // is used to separate the extensions
        strExtensions.Replace(wxT(","), wxT(" "));

        // also deal with the leading dot
        if ( !strExtensions.empty() && strExtensions[0u] == wxT('.') )
        {
            strExtensions.erase(0, 1);
        }

        wxLogTrace(TRACE_MIME, wxT("mime.types: '%s' => '%s' (%s)"),
                   strExtensions.c_str(),
                   strMimeType.c_str(),
                   strDesc.c_str());

        AddMimeTypeInfo(strMimeType, strExtensions, strDesc);

        // finished with this line
        pc = NULL;
    }

    return true;
}

// ----------------------------------------------------------------------------
// UNIX mailcap files parsing
// ----------------------------------------------------------------------------

// the data for a single MIME type
struct MailcapLineData
{
    // field values
    wxString type,
             cmdOpen,
             test,
             icon,
             desc;

    wxArrayString verbs,
                  commands;

    // flags
    bool testfailed,
         needsterminal,
         copiousoutput;

    MailcapLineData() { testfailed = needsterminal = copiousoutput = false; }
};

// process a non-standard (i.e. not the first or second one) mailcap field
bool
wxMimeTypesManagerImpl::ProcessOtherMailcapField(MailcapLineData& data,
                                                 const wxString& curField)
{
    if ( curField.empty() )
    {
        // we don't care
        return true;
    }

    // is this something of the form foo=bar?
    const wxChar *pEq = wxStrchr(curField, wxT('='));
    if ( pEq != NULL )
    {
        // split "LHS = RHS" in 2
        wxString lhs = curField.BeforeFirst(wxT('=')),
                 rhs = curField.AfterFirst(wxT('='));

        lhs.Trim(true);     // from right
        rhs.Trim(false);    // from left

        // it might be quoted
        if ( !rhs.empty() && rhs[0u] == wxT('"') && rhs.Last() == wxT('"') )
        {
            rhs = rhs.Mid(1, rhs.length() - 2);
        }

        // is it a command verb or something else?
        if ( lhs == wxT("test") )
        {
            if ( wxSystem(rhs) == 0 )
            {
                // ok, test passed
                wxLogTrace(TRACE_MIME_TEST,
                           wxT("Test '%s' for mime type '%s' succeeded."),
                           rhs.c_str(), data.type.c_str());

            }
            else
            {
                wxLogTrace(TRACE_MIME_TEST,
                           wxT("Test '%s' for mime type '%s' failed, skipping."),
                           rhs.c_str(), data.type.c_str());

                data.testfailed = true;
            }
        }
        else if ( lhs == wxT("desc") )
        {
            data.desc = rhs;
        }
        else if ( lhs == wxT("x11-bitmap") )
        {
            data.icon = rhs;
        }
        else if ( lhs == wxT("notes") )
        {
            // ignore
        }
        else // not a (recognized) special case, must be a verb (e.g. "print")
        {
            data.verbs.Add(lhs);
            data.commands.Add(rhs);
        }
    }
    else // '=' not found
    {
        // so it must be a simple flag
        if ( curField == wxT("needsterminal") )
        {
            data.needsterminal = true;
        }
        else if ( curField == wxT("copiousoutput"))
        {
            // copiousoutput impies that the viewer is a console program
            data.needsterminal =
            data.copiousoutput = true;
        }
        else if ( !IsKnownUnimportantField(curField) )
        {
            return false;
        }
    }

    return true;
}

size_t wxMimeTypesManagerImpl::EnumAllFileTypes(wxArrayString& mimetypes)
{
    InitIfNeeded();
    mimetypes.Empty();

    wxString type;
    size_t count = 0;
    for ( size_t n = 0; n < count; n++ )
    {
        // don't return template types from here (i.e. anything containg '*')
        if ( type.Find(_T('*')) == wxNOT_FOUND )
        {
            mimetypes.Add(type);
        }
    }

    return mimetypes.GetCount();
}

// ----------------------------------------------------------------------------
// writing to MIME type files
// ----------------------------------------------------------------------------

bool wxMimeTypesManagerImpl::Unassociate(wxFileType *ft)
{
    return false;
}

wxString wxMimeTypesManagerImpl::GetExtension(size_t)
{
	return wxString("");
}
// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static bool IsKnownUnimportantField(const wxString& fieldAll)
{
    static const wxChar *knownFields[] =
    {
        _T("x-mozilla-flags"),
        _T("nametemplate"),
        _T("textualnewlines"),
    };

    wxString field = fieldAll.BeforeFirst(_T('='));
    for ( size_t n = 0; n < WXSIZEOF(knownFields); n++ )
    {
        if ( field.CmpNoCase(knownFields[n]) == 0 )
            return true;
    }

    return false;
}

#endif
  // wxUSE_MIMETYPE && wxUSE_FILE && wxUSE_TEXTFILE

