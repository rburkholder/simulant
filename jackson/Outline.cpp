/* 
 * File:   Outline.cpp
 * Author: rpb
 * 
 * Created on April 3, 2015, 10:30 PM
 */

// todo:  setup vertex lists for triangles, will allow generalized polygon hit test
// todo:  use doubles for a bunch of stuff?

#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/assign/std/set.hpp>

#include "Outline.h"

// http://stackoverflow.com/questions/2049582/how-to-determine-a-point-in-a-triangle  -- some background

Outline::Outline( const wxRect& rect, bool bSelectByEdge, bool bSelectByVertex )
: m_bUndoPrevious( false ), m_bMouseDown( false ), m_bClean( true ),
  m_selectionHit( none ), m_stateMouse( unknown ),
  m_epsilon( 0.001 ),
  m_bSelectByEdge( bSelectByEdge ), m_bSelectByVertex( bSelectByVertex ),
  m_penWhite( wxColor( 255, 255,255 ), 1, wxPENSTYLE_SOLID ),  // white
  m_penBlack( wxColor( 0, 0, 0 ), 1, wxPENSTYLE_SOLID ),  // white
  m_brush( wxColor( 0, 0, 0 ), wxBRUSHSTYLE_TRANSPARENT )  // black
{
  m_polyNew[tl] = m_polyOriginal[tl] = rect.GetTopLeft();
  m_polyNew[tr] = m_polyOriginal[tr] = rect.GetTopRight();
  m_polyNew[br] = m_polyOriginal[br] = rect.GetBottomRight();
  m_polyNew[bl] = m_polyOriginal[bl] = rect.GetBottomLeft();
  
  UpdateBoundingBoxes();
  
  using namespace boost::assign;
  m_setCorners += tl, tr, br, bl;
  m_setEdges += top, right, bottom, left;
}

Outline::~Outline( void ) {
}

wxCursor Outline::TrackMouse( wxPoint point, bool bDown, wxDC& dc ) {
  
  wxCursor cursor( wxNullCursor );


  switch ( m_stateMouse ) {
    case unknown:
      if ( bDown ) { // wait for mouse up to clear state
      }
      else {
        // transition up
        m_stateMouse = up;
      }
      break;
    case up:
      if ( HitTest( point ) ) {
        switch ( m_selectionHit ) {
          case tl:
          case br:
            cursor = wxCursor( wxCURSOR_SIZENWSE );
            std::cout << "tl, br" << std::endl;
            break;
          case bl:
          case tr:
            cursor = wxCursor( wxCURSOR_SIZENESW );
            std::cout << "bl tr" << std::endl;
            break;
          case top:
          case bottom:
            cursor = wxCursor( wxCURSOR_SIZENS );
            std::cout << "top, bottom" << std::endl;
            break;
          case left:
          case right:
            cursor = wxCursor( wxCURSOR_SIZEWE );
            std::cout << "left right" << std::endl;
            break;
          case all:
            cursor = wxCursor( wxCURSOR_SIZING );
            std::cout << "all" << std::endl;
            break;
          case none:
            break;
        }
      }
      if ( bDown && ( none != m_selectionHit ) ) { 
        cursor = wxCursor( wxCURSOR_LEFT_BUTTON );
        m_pointMouseDown = point;
        m_pointMousePrevious = point;
        m_stateMouse = down;
      }
      break;
    case down:
      if ( bDown ) {
        cursor = wxCursor( wxCURSOR_LEFT_BUTTON );
        if ( point != m_pointMousePrevious ) {
          switch ( m_selectionHit ) {
            case tl:
              m_polyNew[tl] = point;
              break;
            case tr:
              m_polyNew[tr] = point;
              break;
            case br:
              m_polyNew[br] = point;
              break;
            case bl:
              m_polyNew[bl] = point;
              break;
            case top:
              if ( m_bSelectByVertex ) {
                m_polyNew[tl] += ( point - m_pointMousePrevious );
                m_polyNew[tr] += ( point - m_pointMousePrevious );
              }
              else {
                m_polyNew[tl].y = point.y;
                m_polyNew[tr].y = point.y;
              }
              break;
            case right:
              if ( m_bSelectByVertex ) {
                m_polyNew[tr] += ( point - m_pointMousePrevious );
                m_polyNew[br] += ( point - m_pointMousePrevious );
              }
              else {
                m_polyNew[tr].x = point.x;
                m_polyNew[br].x = point.x;
              }
              break;
            case bottom:
              if ( m_bSelectByVertex ) {
                m_polyNew[bl] += ( point - m_pointMousePrevious );
                m_polyNew[br] += ( point - m_pointMousePrevious );
              }
              else {
                m_polyNew[bl].y = point.y;
                m_polyNew[br].y = point.y;
              }
              break;
            case left:
              if ( m_bSelectByVertex ) {
                m_polyNew[tl] += ( point - m_pointMousePrevious );
                m_polyNew[bl] += ( point - m_pointMousePrevious );
              }
              else {
                m_polyNew[tl].x = point.x;
                m_polyNew[bl].x = point.x;
              }
              break;
            case all:
              BOOST_FOREACH( Selection ix, m_setCorners ) {
                m_polyNew[ ix ] += ( point - m_pointMousePrevious );
              }
              break;
          }
          m_bUndoPrevious = true;
          m_pointMousePrevious = point;
        }
        Render( dc );
      }
      else {
        // release and update
        UpdateBoundingBoxes();
        m_stateMouse = up;
      }
      break;
  }
  return cursor;
}

void Outline::Erase( wxDC& dc ) {
  m_bClean = true;  // may not be needed
  m_bUndoPrevious = false;  // may not be needed
}
void Outline::Render( wxDC& dc ) {
  
  dc.SetBrush( m_brush );
  if ( m_bUndoPrevious && !m_bClean ) {  // this is going to be a problem if we have a clean slate
    //dc.SetLogicalFunction( wxXOR );
    //dc.SetLogicalFunction( wxINVERT );
    dc.SetPen( m_penBlack );
    dc.DrawPolygon( count, m_polyOriginal );
    BOOST_FOREACH( Selection ix, m_setCorners ) {
      m_polyOriginal[ ix ] = m_polyNew[ ix ];
    }
    m_bUndoPrevious = false;
  }
  //dc.SetLogicalFunction( wxCOPY );
  dc.SetPen( m_penWhite );
  dc.DrawPolygon( count, m_polyOriginal );
  m_bClean = false;
}

bool Outline::HitTest( wxPoint point ) {
  m_selectionHit = none;  
  // overall bounding box test
  if ( m_rectBoundingBox[0].Contains( point ) || m_rectBoundingBox[1].Contains( point ) ) {
    // test on vertices first
    if ( m_bSelectByVertex ) {
      wxPoint pointBounding( bounding, bounding );
      BOOST_FOREACH( Selection ix, m_setCorners ) {
        wxRect rect( m_polyOriginal[ ix ] - pointBounding, m_polyOriginal[ ix ] + pointBounding );
        if ( rect.Contains( point ) ) {
          m_selectionHit = ix;
          break;
        }
      }
    }
    if ( ( none == m_selectionHit ) && m_bSelectByEdge ) { // try selecting edges
      double dif;
      dif = PointToSegmentDistanceSquared( m_polyOriginal[bl], m_polyOriginal[tl], point );
      if ( m_epsilon > dif ) m_selectionHit = left;
      else {
        dif = PointToSegmentDistanceSquared( m_polyOriginal[tl], m_polyOriginal[tr], point );
        if ( m_epsilon > dif ) m_selectionHit = top;
        else {
          dif = PointToSegmentDistanceSquared( m_polyOriginal[tr], m_polyOriginal[br], point );
          if ( m_epsilon > dif ) m_selectionHit = right;
          else {
            dif = PointToSegmentDistanceSquared( m_polyOriginal[br], m_polyOriginal[bl], point );
            if ( m_epsilon > dif ) m_selectionHit = bottom;
          }
        }
      }
    }
    if ( ( none == m_selectionHit ) && !m_bSelectByVertex ) {  // check bounding box, only if rectangular
      if ( m_rectBoundingBox[0].Contains( point ) ) {
        m_selectionHit = all;
      }
    }
    if ( none == m_selectionHit ) { // check interior of quad
      if ( PointInTriangle( m_polyOriginal[bl], m_polyOriginal[tl], m_polyOriginal[tr], point )
        || PointInTriangle( m_polyOriginal[bl], m_polyOriginal[tr], m_polyOriginal[br], point )
        || ( m_epsilon > PointToSegmentDistanceSquared( m_polyOriginal[bl], m_polyOriginal[tr], point ) ) // common edge of triangles
        ) {
        m_selectionHit = all;
      }
    }
  }
  return ( none != m_selectionHit );
}

void Outline::UpdateBoundingBox( wxPoint a, wxPoint b, wxPoint c, wxRect& rect ) {
  rect.SetLeft(   std::min<int>( a.x, std::min<int>( b.x, c.x ) ) );
  rect.SetRight(  std::max<int>( a.x, std::max<int>( b.x, c.x ) ) );
  rect.SetTop(    std::min<int>( a.y, std::min<int>( b.y, c.y ) ) );
  rect.SetBottom( std::max<int>( a.y, std::max<int>( b.y, c.y ) ) );
}


void Outline::UpdateBoundingBoxes( void ) {
  // optimization: if bSelectByVertex is false, then this does not need to be called
  UpdateBoundingBox( m_polyOriginal[bl], m_polyOriginal[tl], m_polyOriginal[tr], m_rectBoundingBox[ 0 ] );
  UpdateBoundingBox( m_polyOriginal[bl], m_polyOriginal[tr], m_polyOriginal[br], m_rectBoundingBox[ 1 ] );
}

void Outline::SetTopLeft( wxPoint point ) {
  m_polyNew[tl] = point;
  m_bUndoPrevious = true;
}

void Outline::SetTopRight( wxPoint point ) {
  m_polyNew[tr] = point;
  m_bUndoPrevious = true;
}

void Outline::SetBottomLeft( wxPoint point ) {
  m_polyNew[bl] = point;
  m_bUndoPrevious = true;
}

void Outline::SetBottomRight( wxPoint point ) {
  m_polyNew[br] = point;
  m_bUndoPrevious = true;
}

// http://totologic.blogspot.fr/2014/01/accurate-point-in-triangle-test.html
double Outline::PointToSegmentDistanceSquared( wxPoint point1, wxPoint point2, wxPoint point ) {
  
  double x1( point1.x );
  double y1( point1.y );
  double x2( point2.x );
  double y2( point2.y );
  double  x( point.x );
  double  y( point.y );
  
  double dblSegmentLengthSquared = (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1);
  double dblDotProduct = ((x - x1)*(x2 - x1) + (y - y1)*(y2 - y1)) / dblSegmentLengthSquared;
  if ( 0.0 > dblDotProduct ) {
    return (x - x1)*(x - x1) + (y - y1)*(y - y1);
  }
  if ( 1.0 >= dblDotProduct ) {
    double dblSegmentSquared = (x1 - x)*(x1 - x) + (y1 - y)*(y1 - y);
    return dblSegmentSquared - dblDotProduct * dblDotProduct * dblSegmentLengthSquared;
  }
  return (x - x2)*(x - x2) + (y - y2)*(y - y2);
}

// http://totologic.blogspot.fr/2014/01/accurate-point-in-triangle-test.html
bool Outline::PointInTriangle( wxPoint a, wxPoint b, wxPoint c, wxPoint p ) {
  // optimization: if bSelectByVertex is false, then this does not need to be called
  // could optimize this by calculating certain values only when points change
  double x1( a.x );
  double y1( a.y );
  double x2( b.x );
  double y2( b.y );
  double x3( c.x );
  double y3( c.y );
  double  x( p.x );
  double  y( p.y );
  
  double denominator = ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3));
  double u = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / denominator;
  double v = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / denominator;
  double w = 1.0 - u - v;

  return ( 0 <= u ) && ( u <= 1 ) && ( 0 <= v ) && ( v <= 1 ) && ( 0 <= w ) && ( w <= 1 );  
  
}
