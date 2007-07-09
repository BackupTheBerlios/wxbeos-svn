/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/dcclient.cpp
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id: dcclient.cpp,v 1.204 2005/08/17 21:40:44 MR Exp $
// Copyright:   (c) 1998 Robert Roebling, Chris Breeze
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "dcclient.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __VMS
#define XCopyPlane XCOPYPLANE
#endif

#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/image.h"
#include "wx/module.h"
#include "wx/log.h"
#include "wx/fontutil.h"

#include "wx/math.h" // for floating-point functions

//-----------------------------------------------------------------------------
// local defines
//-----------------------------------------------------------------------------

#define USE_PAINT_REGION 1

//-----------------------------------------------------------------------------
// local data
//-----------------------------------------------------------------------------

#include "bdiag.xbm"
#include "fdiag.xbm"
#include "cdiag.xbm"
#include "horiz.xbm"
#include "verti.xbm"
#include "cross.xbm"
#define  num_hatches 6

#define IS_15_PIX_HATCH(s) ((s)==wxCROSSDIAG_HATCH || (s)==wxHORIZONTAL_HATCH || (s)==wxVERTICAL_HATCH)
#define IS_16_PIX_HATCH(s) ((s)!=wxCROSSDIAG_HATCH && (s)!=wxHORIZONTAL_HATCH && (s)!=wxVERTICAL_HATCH)

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

const double RAD2DEG  = 180.0 / M_PI;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static inline double dmax(double a, double b) { return a > b ? a : b; }
static inline double dmin(double a, double b) { return a < b ? a : b; }

static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }

//-----------------------------------------------------------------------------
// Implement Pool of Graphic contexts. Creating them takes too much time.
//-----------------------------------------------------------------------------

enum wxPoolGCType
{
   wxGC_ERROR = 0,
   wxTEXT_MONO,
   wxBG_MONO,
   wxPEN_MONO,
   wxBRUSH_MONO,
   wxTEXT_COLOUR,
   wxBG_COLOUR,
   wxPEN_COLOUR,
   wxBRUSH_COLOUR,
   wxTEXT_SCREEN,
   wxBG_SCREEN,
   wxPEN_SCREEN,
   wxBRUSH_SCREEN
};

static int wxGCPoolSize = 0;

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)

wxWindowDC::wxWindowDC()
{
}

wxWindowDC::wxWindowDC( wxWindow *window )
{
    wxASSERT_MSG( window, wxT("DC needs a window") );
}

wxWindowDC::~wxWindowDC()
{
    Destroy();
}

void wxWindowDC::SetUpDC()
{
    m_ok = true;

    wxASSERT_MSG( !m_penGC, wxT("GCs already created") );

}

void wxWindowDC::DoGetSize( int* width, int* height ) const
{
    wxCHECK_RET( m_owner, _T("GetSize() doesn't work without window") );

    m_owner->GetSize(width, height);
}

extern bool wxDoFloodFill(wxDC *dc, wxCoord x, wxCoord y,
                          const wxColour & col, int style);

bool wxWindowDC::DoFloodFill(wxCoord x, wxCoord y,
                             const wxColour& col, int style)
{
    return wxDoFloodFill(this, x, y, col, style);
}

bool wxWindowDC::DoGetPixel( wxCoord x1, wxCoord y1, wxColour *col ) const
{
    // Generic (and therefore rather inefficient) method.
    // Could be improved.
    wxMemoryDC memdc;
    wxBitmap bitmap(1, 1);
    memdc.SelectObject(bitmap);
    memdc.Blit(0, 0, 1, 1, (wxDC*) this, x1, y1);
    memdc.SelectObject(wxNullBitmap);

    wxImage image = bitmap.ConvertToImage();
    col->Set(image.GetRed(0, 0), image.GetGreen(0, 0), image.GetBlue(0, 0));
    return true;
}

void wxWindowDC::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
    }
}

void wxWindowDC::DoCrossHair( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );
}

void wxWindowDC::DoDrawArc( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
                            wxCoord xc, wxCoord yc )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord xx1 = XLOG2DEV(x1);
    wxCoord yy1 = YLOG2DEV(y1);
    wxCoord xx2 = XLOG2DEV(x2);
    wxCoord yy2 = YLOG2DEV(y2);
    wxCoord xxc = XLOG2DEV(xc);
    wxCoord yyc = YLOG2DEV(yc);
    double dx = xx1 - xxc;
    double dy = yy1 - yyc;
    double radius = sqrt((double)(dx*dx+dy*dy));
    wxCoord   r      = (wxCoord)radius;
    double radius1, radius2;

    if (xx1 == xx2 && yy1 == yy2)
    {
        radius1 = 0.0;
        radius2 = 360.0;
    }
    else
    if (radius == 0.0)
    {
        radius1 = radius2 = 0.0;
    }
    else
    {
        radius1 = (xx1 - xxc == 0) ?
            (yy1 - yyc < 0) ? 90.0 : -90.0 :
            -atan2(double(yy1-yyc), double(xx1-xxc)) * RAD2DEG;
        radius2 = (xx2 - xxc == 0) ?
            (yy2 - yyc < 0) ? 90.0 : -90.0 :
            -atan2(double(yy2-yyc), double(xx2-xxc)) * RAD2DEG;
    }
    wxCoord alpha1 = wxCoord(radius1 * 64.0);
    wxCoord alpha2 = wxCoord((radius2 - radius1) * 64.0);
    while (alpha2 <= 0) alpha2 += 360*64;
    while (alpha1 > 360*64) alpha1 -= 360*64;

    if (m_window)
    {
    }

    CalcBoundingBox (x1, y1);
    CalcBoundingBox (x2, y2);
}

void wxWindowDC::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double sa, double ea )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_window)
    {
        wxCoord start = wxCoord(sa * 64.0);
        wxCoord end = wxCoord((ea-sa) * 64.0);

        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
            }
            else
            {
            }
        }
    }

    CalcBoundingBox (x, y);
    CalcBoundingBox (x + width, y + height);
}

void wxWindowDC::DoDrawPoint( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );
    CalcBoundingBox (x, y);
}

void wxWindowDC::DoDrawLines( int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_pen.GetStyle() == wxTRANSPARENT) return;
    if (n <= 0) return;
}

void wxWindowDC::DoDrawPolygon( int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int WXUNUSED(fillStyle) )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (n <= 0) return;
}

void wxWindowDC::DoDrawRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0) return;

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
            }
            else
            {
            }
        }

    }

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DoDrawRoundedRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (radius < 0.0) radius = - radius * ((width < height) ? width : height);

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);
    wxCoord rr = XLOG2DEVREL((wxCoord)radius);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    // CMB: if radius is zero use DrawRectangle() instead to avoid
    // X drawing errors with small radii
    if (rr == 0)
    {
        DrawRectangle( x, y, width, height );
        return;
    }

    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0) return;

    // CMB: adjust size if outline is drawn otherwise the result is
    // 1 pixel too wide and high
    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        ww--;
        hh--;
    }

    if (m_window)
    {
        // CMB: ensure dd is not larger than rectangle otherwise we
        // get an hour glass shape
        wxCoord dd = 2 * rr;
        if (dd > ww) dd = ww;
        if (dd > hh) dd = hh;
        rr = dd / 2;

        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
            }
            else
            {
            }
        }

        if (m_pen.GetStyle() != wxTRANSPARENT)
        {
        }
    }

    // this ignores the radius
    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DoDrawEllipse( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
            }
            else
            {
            }
        }

    }

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y )
{
    // VZ: egcs 1.0.3 refuses to compile this without cast, no idea why
    DoDrawBitmap( (const wxBitmap&)icon, x, y, true );
}

void wxWindowDC::DoDrawBitmap( const wxBitmap &bitmap,
                               wxCoord x, wxCoord y,
                               bool useMask )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCHECK_RET( bitmap.Ok(), wxT("invalid bitmap") );

    bool is_mono = (bitmap.GetBitmap() != NULL);

    // scale/translate size and position
    int xx = XLOG2DEV(x);
    int yy = YLOG2DEV(y);

    int w = bitmap.GetWidth();
    int h = bitmap.GetHeight();

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + w, y + h );

    if (!m_window) return;

    int ww = XLOG2DEVREL(w);
    int hh = YLOG2DEVREL(h);

    // compare to current clipping region
    if (!m_currentClippingRegion.IsNull())
    {
        wxRegion tmp( xx,yy,ww,hh );
        tmp.Intersect( m_currentClippingRegion );
        if (tmp.IsEmpty())
            return;
    }

    // scale bitmap if required
    wxBitmap use_bitmap = bitmap;
    // apply mask if any
}

bool wxWindowDC::DoBlit( wxCoord xdest, wxCoord ydest,
                         wxCoord width, wxCoord height,
                         wxDC *source,
                         wxCoord xsrc, wxCoord ysrc,
                         int logical_func,
                         bool useMask,
                         wxCoord xsrcMask, wxCoord ysrcMask )
{
    wxCHECK_MSG( Ok(), false, wxT("invalid window dc") );

    wxCHECK_MSG( source, false, wxT("invalid source dc") );

    if (!m_window) return false;
    return true;
}

void wxWindowDC::DoDrawText( const wxString &text, wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (!m_window) return;

    if (text.empty()) return;
}


// TODO: There is an example of rotating text with GTK2 that would probably be
// a better approach here:
//           http://www.daa.com.au/pipermail/pygtk/2003-April/005052.html

void wxWindowDC::DoDrawRotatedText( const wxString &text, wxCoord x, wxCoord y, double angle )
{
    if (angle == 0.0)
    {
        DrawText(text, x, y);
        return;
    }

    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (!m_window) return;

    wxCoord w;
    wxCoord h;

    // draw the string normally
    wxBitmap src(w, h);
    wxMemoryDC dc;
    dc.SelectObject(src);
    dc.SetFont(GetFont());
    dc.SetBackground(*wxBLACK_BRUSH);
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.Clear();
    dc.SetTextForeground( *wxWHITE );
    dc.DrawText(text, 0, 0);
    dc.SelectObject(wxNullBitmap);

    // Calculate the size of the rotated bounding box.
    double rad = DegToRad(angle);
    double dx = cos(rad),
           dy = sin(rad);

    // the rectngle vertices are counted clockwise with the first one being at
    // (0, 0) (or, rather, at (x, y))
    double x2 = w*dx,
           y2 = -w*dy;      // y axis points to the bottom, hence minus
    double x4 = h*dy,
           y4 = h*dx;
    double x3 = x4 + x2,
           y3 = y4 + y2;

    // calc max and min
    wxCoord maxX = (wxCoord)(dmax(x2, dmax(x3, x4)) + 0.5),
            maxY = (wxCoord)(dmax(y2, dmax(y3, y4)) + 0.5),
            minX = (wxCoord)(dmin(x2, dmin(x3, x4)) - 0.5),
            minY = (wxCoord)(dmin(y2, dmin(y3, y4)) - 0.5);


    wxImage image = src.ConvertToImage();

    image.ConvertColourToAlpha( m_textForegroundColour.Red(),
                                m_textForegroundColour.Green(),
                                m_textForegroundColour.Blue() );
    image = image.Rotate( rad, wxPoint(0,0) );

    int i_angle = (int) angle;
    i_angle = i_angle % 360;
    if (i_angle < 0)
        i_angle += 360;
    int xoffset = 0;
    if ((i_angle >= 90.0) && (i_angle < 270.0))
        xoffset = image.GetWidth();
    int yoffset = 0;
    if ((i_angle >= 0.0) && (i_angle < 180.0))
        yoffset = image.GetHeight();

    if ((i_angle >= 0) && (i_angle < 90))
        yoffset -= (int)( cos(rad)*h );
    if ((i_angle >= 90) && (i_angle < 180))
        xoffset -= (int)( sin(rad)*h );
    if ((i_angle >= 180) && (i_angle < 270))
        yoffset -= (int)( cos(rad)*h );
    if ((i_angle >= 270) && (i_angle < 360))
        xoffset -= (int)( sin(rad)*h );

    int i_x = x - xoffset;
    int i_y = y - yoffset;

    src = image;
    DoDrawBitmap( src, i_x, i_y, true );


    // it would be better to draw with non underlined font and draw the line
    // manually here (it would be more straight...)
#if 0
    if ( m_font.GetUnderlined() )
    {
        gdk_draw_line( m_window, m_textGC,
                       XLOG2DEV(x + x4), YLOG2DEV(y + y4 + font->descent),
                       XLOG2DEV(x + x3), YLOG2DEV(y + y3 + font->descent));
    }
#endif // 0

    // update the bounding box
    CalcBoundingBox(x + minX, y + minY);
    CalcBoundingBox(x + maxX, y + maxY);
}

void wxWindowDC::DoGetTextExtent(const wxString &string,
                                 wxCoord *width, wxCoord *height,
                                 wxCoord *descent, wxCoord *externalLeading,
                                 wxFont *theFont) const
{
    if ( width )
        *width = 0;
    if ( height )
        *height = 0;
    if ( descent )
        *descent = 0;
    if ( externalLeading )
        *externalLeading = 0;

    if (string.empty())
    {
        return;
    }

#ifdef __WXGTK20__
    // Set new font description
    if (theFont)
        pango_layout_set_font_description( m_layout, theFont->GetNativeFontInfo()->description );

    // Set layout's text
#if wxUSE_UNICODE
    const wxCharBuffer data = wxConvUTF8.cWC2MB( string );
    const char *dataUTF8 = (const char *)data;
#else
    const wxWCharBuffer wdata = wxConvLocal.cMB2WC( string );
    if ( !wdata )
        return;

    const wxCharBuffer data = wxConvUTF8.cWC2MB( wdata );
    const char *dataUTF8 = (const char *)data;
#endif

    if ( !dataUTF8 )
    {
        // hardly ideal, but what else can we do if conversion failed?
        return;
    }

    pango_layout_set_text( m_layout, dataUTF8, strlen(dataUTF8) );

    if (descent)
    {
        int h;
        pango_layout_get_pixel_size( m_layout, width, &h );
        PangoLayoutIter *iter = pango_layout_get_iter(m_layout);
        int baseline = pango_layout_iter_get_baseline(iter);
        pango_layout_iter_free(iter);
        *descent = h - PANGO_PIXELS(baseline);

        if (height)
            *height = (wxCoord) h;
    }
    else
    {
        pango_layout_get_pixel_size( m_layout, width, height );
    }

    // Reset old font description
    if (theFont)
        pango_layout_set_font_description( m_layout, m_fontdesc );
#else // GTK+ 1.x
    wxFont fontToUse = m_font;
    if (theFont)
        fontToUse = *theFont;
#endif // GTK+ 2/1
}

wxCoord wxWindowDC::GetCharWidth() const
{
#ifdef __WXGTK20__
    pango_layout_set_text( m_layout, "H", 1 );
    int w;
    pango_layout_get_pixel_size( m_layout, &w, NULL );
    return w;
#else
#endif
}

wxCoord wxWindowDC::GetCharHeight() const
{
#ifdef __WXGTK20__
    pango_layout_set_text( m_layout, "H", 1 );
    int h;
    pango_layout_get_pixel_size( m_layout, NULL, &h );
    return h;
#else
#endif
}

void wxWindowDC::Clear()
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (!m_window) return;

    // VZ: the code below results in infinite recursion and crashes when
    //     dc.Clear() is done from OnPaint() so I disable it for now.
    //     I don't know what the correct fix is but Clear() surely should not
    //     reenter OnPaint()!
#if 0
    /* - we either are a memory dc or have a window as the
       owner. anything else shouldn't happen.
       - we don't use gdk_window_clear() as we don't set
       the window's background colour anymore. it is too
       much pain to keep the DC's and the window's back-
       ground colour in synch. */

    if (m_owner)
    {
        m_owner->Clear();
        return;
    }

    if (m_isMemDC)
    {
        int width,height;
        GetSize( &width, &height );
        gdk_draw_rectangle( m_window, m_bgGC, TRUE, 0, 0, width, height );
        return;
    }
#else // 1
    int width,height;
    GetSize( &width, &height );
#endif // 0/1
}

void wxWindowDC::SetFont( const wxFont &font )
{
    m_font = font;

#ifdef __WXGTK20__
    if (m_font.Ok())
    {
        if (m_fontdesc)
            pango_font_description_free( m_fontdesc );

        m_fontdesc = pango_font_description_copy( m_font.GetNativeFontInfo()->description );


        if (m_owner)
        {
            PangoContext *oldContext = m_context;

            // We might want to use the X11 context for faster
            // rendering on screen.
            // MR: Lets not want to do this, as this introduces libpangox dependancy.
#if 0
            if (m_font.GetNoAntiAliasing())
                m_context = m_owner->GtkGetPangoX11Context();
            else
#endif
            m_context = m_owner->GtkGetPangoDefaultContext();

            // If we switch back/forth between different contexts
            // we also have to create a new layout. I think so,
            // at least, and it doesn't hurt to do it.
            if (oldContext != m_context)
            {
                if (m_layout)
                    g_object_unref( G_OBJECT( m_layout ) );

                m_layout = pango_layout_new( m_context );
            }
        }

        pango_layout_set_font_description( m_layout, m_fontdesc );
    }
#endif
}

void wxWindowDC::SetPen( const wxPen &pen )
{
}

void wxWindowDC::SetBrush( const wxBrush &brush )
{
}

void wxWindowDC::SetBackground( const wxBrush &brush )
{
   /* CMB 21/7/98: Added SetBackground. Sets background brush
    * for Clear() and bg colour for shapes filled with cross-hatch brush */

    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_backgroundBrush == brush) return;

    m_backgroundBrush = brush;

    if (!m_backgroundBrush.Ok()) return;

    if (!m_window) return;


}

void wxWindowDC::SetLogicalFunction( int function )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_logicalFunction == function)
        return;

    // VZ: shouldn't this be a CHECK?
    if (!m_window)
        return;

}

void wxWindowDC::SetTextForeground( const wxColour &col )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    // don't set m_textForegroundColour to an invalid colour as we'd crash
    // later then (we use m_textForegroundColour.GetColor() without checking
    // in a few places)
    if ( !col.Ok() || (m_textForegroundColour == col) )
        return;

    m_textForegroundColour = col;

}

void wxWindowDC::SetTextBackground( const wxColour &col )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    // same as above
    if ( !col.Ok() || (m_textBackgroundColour == col) )
        return;

    m_textBackgroundColour = col;

    if ( m_window )
    {
    }
}

void wxWindowDC::SetBackgroundMode( int mode )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    m_backgroundMode = mode;

    if (!m_window) return;

    // CMB 21/7/98: fill style of cross-hatch brushes is affected by
    // transparent/solid background mode

    if (m_brush.GetStyle() != wxSOLID && m_brush.GetStyle() != wxTRANSPARENT)
    {
    }
}

void wxWindowDC::SetPalette( const wxPalette& WXUNUSED(palette) )
{
    wxFAIL_MSG( wxT("wxWindowDC::SetPalette not implemented") );
}

void wxWindowDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (!m_window) return;

    wxRect rect;
    rect.x = XLOG2DEV(x);
    rect.y = YLOG2DEV(y);
    rect.width = XLOG2DEVREL(width);
    rect.height = YLOG2DEVREL(height);

    if (!m_currentClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( rect );
    else
        m_currentClippingRegion.Union( rect );

#if USE_PAINT_REGION
    if (!m_paintClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( m_paintClippingRegion );
#endif

    wxCoord xx, yy, ww, hh;
    m_currentClippingRegion.GetBox( xx, yy, ww, hh );
    wxDC::DoSetClippingRegion( xx, yy, ww, hh );

}

void wxWindowDC::DoSetClippingRegionAsRegion( const wxRegion &region  )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (region.Empty())
    {
        DestroyClippingRegion();
        return;
    }

    if (!m_window) return;

    if (!m_currentClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( region );
    else
        m_currentClippingRegion.Union( region );

#if USE_PAINT_REGION
    if (!m_paintClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( m_paintClippingRegion );
#endif

    wxCoord xx, yy, ww, hh;
    m_currentClippingRegion.GetBox( xx, yy, ww, hh );
    wxDC::DoSetClippingRegion( xx, yy, ww, hh );

}

void wxWindowDC::DestroyClippingRegion()
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxDC::DestroyClippingRegion();

    m_currentClippingRegion.Clear();

}

void wxWindowDC::Destroy()
{
}

void wxWindowDC::ComputeScaleAndOrigin()
{
    /* CMB: copy scale to see if it changes */
    double origScaleX = m_scaleX;
    double origScaleY = m_scaleY;

    wxDC::ComputeScaleAndOrigin();

    /* CMB: if scale has changed call SetPen to recalulate the line width */
    if ((m_scaleX != origScaleX || m_scaleY != origScaleY) &&
        (m_pen.Ok()))
    {
        /* this is a bit artificial, but we need to force wxDC to think
           the pen has changed */
        wxPen pen = m_pen;
        m_pen = wxNullPen;
        SetPen( pen );
    }
}

// Resolution in pixels per logical inch
wxSize wxWindowDC::GetPPI() const
{
    return wxSize( (int) (m_mm_to_pix_x * 25.4 + 0.5), (int) (m_mm_to_pix_y * 25.4 + 0.5));
}

int wxWindowDC::GetDepth() const
{
#ifdef __WXGTK20__
    return gdk_drawable_get_depth(m_window);
#else
    wxFAIL_MSG(wxT("not implemented"));

    return -1;
#endif
}


//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxClientDC)

// Limit the paint region to the window size. Sometimes
// the paint region is too big, and this risks X11 errors
static void wxLimitRegionToSize(wxRegion& region, const wxSize& sz)
{
    wxRect originalRect = region.GetBox();
    wxRect rect(originalRect);
    if (rect.width + rect.x > sz.x)
        rect.width = sz.x - rect.x;
    if (rect.height + rect.y > sz.y)
        rect.height = sz.y - rect.y;
    if (rect != originalRect)
    {
        region = wxRegion(rect);
        wxLogTrace(wxT("painting"), wxT("Limiting region from %d, %d, %d, %d to %d, %d, %d, %d\n"),
                   originalRect.x, originalRect.y, originalRect.width, originalRect.height,
                   rect.x, rect.y, rect.width, rect.height);
    }
}

wxPaintDC::wxPaintDC( wxWindow *win )
         : wxClientDC( win )
{
#if USE_PAINT_REGION
#endif // USE_PAINT_REGION
}

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)

wxClientDC::wxClientDC( wxWindow *win )
          : wxWindowDC( win )
{
    wxCHECK_RET( win, _T("NULL window in wxClientDC::wxClientDC") );

#ifdef __WXUNIVERSAL__
    wxPoint ptOrigin = win->GetClientAreaOrigin();
    SetDeviceOrigin(ptOrigin.x, ptOrigin.y);
    wxSize size = win->GetClientSize();
    SetClippingRegion(wxPoint(0, 0), size);
#endif // __WXUNIVERSAL__
}

void wxClientDC::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_owner, _T("GetSize() doesn't work without window") );

    m_owner->GetClientSize( width, height );
}

// ----------------------------------------------------------------------------
// wxDCModule
// ----------------------------------------------------------------------------

class wxDCModule : public wxModule
{
public:
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxDCModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxDCModule, wxModule)

bool wxDCModule::OnInit()
{
    return true;
}

void wxDCModule::OnExit()
{
}
