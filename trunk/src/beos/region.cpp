/////////////////////////////////////////////////////////////////////////////
// File:      region.cpp
// Purpose:   Region class
// Author:    Julian Smart, Robert Roebling
// Created:   Fri Oct 24 10:46:34 MET 1997
// RCS-ID:    $Id: region.cpp,v 1.12 2004/05/23 20:53:30 JS Exp $
// Copyright: (c) 1997 Julian Smart, Robert Roebling
// Licence:   wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "region.h"
#endif

#include "wx/gdicmn.h"
#include "wx/region.h"
#include "wx/log.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxRegion, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxRegionIterator, wxObject)

// ----------------------------------------------------------------------------
// wxRegion construction
// ----------------------------------------------------------------------------

void wxRegion::InitRect(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
//	BRect rect;
//    rect.x = x;
//    rect.y = y;
//    rect.width = w;
//    rect.height = h;
//    
//    m_refData = new wxRegionRefData();
//    
//    M_REGIONDATA->m_region = XCreateRegion();
//    XUnionRectWithRegion( &rect, M_REGIONDATA->m_region, M_REGIONDATA->m_region );
}

wxRegion::wxRegion( size_t n, const wxPoint *points, int fillStyle )
{
#if 0
    XPoint *xpoints = new XPoint[n];
    for ( size_t i = 0 ; i < n ; i++ )
    {
        xpoints[i].x = points[i].x;
        xpoints[i].y = points[i].y;
    }

    m_refData = new wxRegionRefData();

    Region* reg = gdk_region_polygon
                     (
                        gdkpoints,
                        n,
                        fillStyle == wxWINDING_RULE ? GDK_WINDING_RULE
                                                    : GDK_EVEN_ODD_RULE
                     );

    M_REGIONDATA->m_region = reg;

    delete [] xpoints;
#endif
}

wxRegion::~wxRegion()
{
    // m_refData unrefed in ~wxObject
}

wxObjectRefData *wxRegion::CreateRefData() const
{
#if 0
    return new wxRegionRefData;
#endif
}

wxObjectRefData *wxRegion::CloneRefData(const wxObjectRefData *data) const
{
#if 0
    return new wxRegionRefData(*(wxRegionRefData *)data);
#endif
}

// ----------------------------------------------------------------------------
// wxRegion comparison
// ----------------------------------------------------------------------------

bool wxRegion::operator==( const wxRegion& region )
{
#if 0
    if (m_refData == region.m_refData) return TRUE;

    if (!m_refData || !region.m_refData) return FALSE;
    
    // compare the regions themselves, not the pointers to ref data!
    return XEqualRegion( M_REGIONDATA->m_region,
                         M_REGIONDATA_OF(region)->m_region );
#endif
}

// ----------------------------------------------------------------------------
// wxRegion operations
// ----------------------------------------------------------------------------

void wxRegion::Clear()
{
    UnRef();
}

bool wxRegion::Union( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
#if 0
    // work around for XUnionRectWithRegion() bug: taking a union with an empty
    // rect results in an empty region (at least XFree 3.3.6 and 4.0 have this
    // problem)
    if ( !width || !height )
        return TRUE;

    XRectangle rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    
    if (!m_refData)
    {
        m_refData = new wxRegionRefData();
        M_REGIONDATA->m_region = XCreateRegion();
        XUnionRectWithRegion( &rect, M_REGIONDATA->m_region, M_REGIONDATA->m_region );
    }
    else
    {
        AllocExclusive();

        XUnionRectWithRegion( &rect, M_REGIONDATA->m_region, M_REGIONDATA->m_region );
    }
#endif
    return TRUE;
}

bool wxRegion::Union( const wxRect& rect )
{
    return Union( rect.x, rect.y, rect.width, rect.height );
}

bool wxRegion::Union( const wxRegion& region )
{
#if 0
    if (region.IsNull())
        return FALSE;

    if (!m_refData)
    {
        m_refData = new wxRegionRefData();
        M_REGIONDATA->m_region = XCreateRegion();
    }
    else
    {
        AllocExclusive();
    }
    
    XUnionRegion( M_REGIONDATA->m_region,
                  M_REGIONDATA_OF(region)->m_region,
                  M_REGIONDATA->m_region );
#endif
    return TRUE;
}

bool wxRegion::Union(const wxBitmap& bmp)
{
	return false;
}

bool wxRegion::Union(const wxBitmap& bmp,
               const wxColour& transColour, int tolerance = 0)
{
	return false;
}

bool wxRegion::Intersect( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxRegion reg( x, y, width, height );

    return Intersect( reg );
}

bool wxRegion::Intersect( const wxRect& rect )
{
    wxRegion reg( rect );
    
    return Intersect( reg );
}

bool wxRegion::Intersect( const wxRegion& region )
{
#if 0
    if (region.IsNull())
        return FALSE;

    if (!m_refData)
    {
        m_refData = new wxRegionRefData();
        M_REGIONDATA->m_region = XCreateRegion();
        
        // leave here 
        return TRUE;
    }
    else
    {
        AllocExclusive();
    }

    XIntersectRegion( M_REGIONDATA->m_region,
                      M_REGIONDATA_OF(region)->m_region,
                      M_REGIONDATA->m_region );
#endif
    return TRUE;
}

bool wxRegion::Subtract( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxRegion reg( x, y, width, height );
    return Subtract( reg );
}

bool wxRegion::Subtract( const wxRect& rect )
{
    wxRegion reg( rect );
    return Subtract( reg );
}

bool wxRegion::Subtract( const wxRegion& region )
{
#if 0
    if (region.IsNull())
        return FALSE;

    if (!m_refData)
    {
        m_refData = new wxRegionRefData();
        M_REGIONDATA->m_region = XCreateRegion();
    }
    else
    {
        AllocExclusive();
    }

    XSubtractRegion( M_REGIONDATA->m_region,
                     M_REGIONDATA_OF(region)->m_region,
                     M_REGIONDATA->m_region );
#endif
    return TRUE;
}

bool wxRegion::Xor( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
#if 0
    wxRegion reg( x, y, width, height );
    return Xor( reg );
#endif
}

bool wxRegion::Xor( const wxRect& rect )
{
    wxRegion reg( rect );
    return Xor( reg );
}

bool wxRegion::Xor( const wxRegion& region )
{
#if 0
    if (region.IsNull())
        return FALSE;

    if (!m_refData)
    {
        m_refData = new wxRegionRefData();
        M_REGIONDATA->m_region = XCreateRegion();
    }
    else
    {
        AllocExclusive();
    }

    XXorRegion( M_REGIONDATA->m_region,
                M_REGIONDATA_OF(region)->m_region,
                M_REGIONDATA->m_region );
#endif                
    return TRUE;
}

// ----------------------------------------------------------------------------
// wxRegion tests
// ----------------------------------------------------------------------------

void wxRegion::GetBox( wxCoord &x, wxCoord &y, wxCoord &w, wxCoord &h ) const
{
#if 0
    if (m_refData)
    {
        XRectangle rect;
        XClipBox( M_REGIONDATA->m_region, &rect );
        x = rect.x;
        y = rect.y;
        w = rect.width;
        h = rect.height;
    }
    else
    {
        x = 0;
        y = 0;
        w = -1;
        h = -1;
    }
#endif
}

wxRect wxRegion::GetBox() const
{
    wxCoord x, y, w, h;
    GetBox( x, y, w, h );
    return wxRect( x, y, w, h );
}

bool wxRegion::Offset( wxCoord x, wxCoord y )
{
#if 0
    if (!m_refData)
        return FALSE;

    AllocExclusive();

    XOffsetRegion( M_REGIONDATA->m_region, x, y );
#endif
    return TRUE;
}

bool wxRegion::Empty() const
{
    if (!m_refData)
        return TRUE;
#if 0
    return XEmptyRegion( M_REGIONDATA->m_region );
#endif
}

wxRegionContain wxRegion::Contains( wxCoord x, wxCoord y ) const
{
    if (!m_refData)
        return wxOutRegion;

#if 0
    if (XPointInRegion( M_REGIONDATA->m_region, x, y ))
        return wxInRegion;
    else
        return wxOutRegion;
#endif
}

wxRegionContain wxRegion::Contains( wxCoord x, wxCoord y, wxCoord w, wxCoord h ) const
{
    if (!m_refData)
        return wxOutRegion;

#if 0
    int res = XRectInRegion( M_REGIONDATA->m_region, x, y, w, h );
    switch (res)
    {
        case RectangleIn:   return wxInRegion;
        case RectangleOut:  return wxOutRegion;
        case RectanglePart: return wxPartRegion;
    }
    return wxOutRegion;
#endif
}

wxRegionContain wxRegion::Contains(const wxPoint& pt) const
{
    return Contains( pt.x, pt.y );
}

wxRegionContain wxRegion::Contains(const wxRect& rect) const
{
    return Contains( rect.x, rect.y, rect.width, rect.height );
}

#if 0
WXRegion *wxRegion::GetX11Region() const
{
    if (!m_refData)
        return (WXRegion*) NULL;

    return (WXRegion*) M_REGIONDATA->m_region;
}
#endif

// ----------------------------------------------------------------------------
// wxRegionIterator
// ----------------------------------------------------------------------------

// the following structures must match the private structures
// in X11 region code ( xc/lib/X11/region.h )

// this makes the Region type transparent
// and we have access to the region rectangles

struct _XBox {
    short x1, x2, y1, y2;
};

struct _XRegion {
    long   size , numRects;
    _XBox *rects, extents;
};

class wxRIRefData: public wxObjectRefData
{
public:

    wxRIRefData() : m_rects(0), m_numRects(0){}
   ~wxRIRefData();

    wxRect *m_rects;
    size_t  m_numRects;

    void CreateRects( const wxRegion& r );
};

wxRIRefData::~wxRIRefData()
{
    delete [] m_rects;
}

void wxRIRefData::CreateRects( const wxRegion& region )
{
    if (m_rects)
      delete [] m_rects;

    m_rects = 0;
    m_numRects = 0;
    
    if (region.IsEmpty()) return;
    
#if 0
    Region r = (Region) region.GetX11Region();
    if (r)
    {
        m_numRects = r->numRects;
        if (m_numRects)
        {
            m_rects = new wxRect[m_numRects];
            for (size_t i=0; i < m_numRects; ++i)
            {
                _XBox &xr = r->rects[i];
                wxRect &wr = m_rects[i];
                wr.x = xr.x1;
                wr.y = xr.y1;
                wr.width = xr.x2-xr.x1;
                wr.height = xr.y2-xr.y1;
            }
        }
    }
#endif    
}

wxRegionIterator::wxRegionIterator()
{
    m_refData = new wxRIRefData();
    Reset();
}

wxRegionIterator::wxRegionIterator( const wxRegion& region )
{
    m_refData = new wxRIRefData();
    Reset(region);
}

void wxRegionIterator::Reset( const wxRegion& region )
{
    m_region = region;
    ((wxRIRefData*)m_refData)->CreateRects(region);
    Reset();
}

bool wxRegionIterator::HaveRects() const
{
    return m_current < ((wxRIRefData*)m_refData)->m_numRects;
}

wxRegionIterator::operator bool () const
{
    return HaveRects();
}

void wxRegionIterator::operator ++ ()
{
    if (HaveRects()) ++m_current;
}

void wxRegionIterator::operator ++ (int)
{
    if (HaveRects()) ++m_current;
}

wxCoord wxRegionIterator::GetX() const
{
    if( !HaveRects() ) return 0;
    return ((wxRIRefData*)m_refData)->m_rects[m_current].x;
}

wxCoord wxRegionIterator::GetY() const
{
    if( !HaveRects() ) return 0;
    return ((wxRIRefData*)m_refData)->m_rects[m_current].y;
}

wxCoord wxRegionIterator::GetW() const
{
    if( !HaveRects() ) return -1;
    return ((wxRIRefData*)m_refData)->m_rects[m_current].width;
}

wxCoord wxRegionIterator::GetH() const
{
    if( !HaveRects() ) return -1;
    return ((wxRIRefData*)m_refData)->m_rects[m_current].height;
}

wxRect wxRegionIterator::GetRect() const
{
    wxRect r;
    if( HaveRects() )
        r = ((wxRIRefData*)m_refData)->m_rects[m_current];

    return r;
}

