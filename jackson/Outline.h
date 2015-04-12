/* 
 * File:   Outlining.h
 * Author: rpb
 *
 * Created on April 3, 2015, 10:30 PM
 */

#ifndef OUTLINING_H
#define	OUTLINING_H

#include <set>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <wx/wx.h>

/*
 * a mechanism for creating outlines, moving the box, the edges, or the verticies
 *   canvas:  always rectangular, resizeable, move box, or individual edge
 *   movie screen:  four sided polygon, move object, move any vertex, moving an edge changes length of attached edges
 *   texture:  same flexbility as movie screen
 * 
 * enter edit mode automatically when adding a rectangle
 * 
 * when creating rectangle, place at 1/3 and 2/3 positions
 * 
 * mode selection:
 *   ctrl:  all vertices
 *   line mode: when near line
 *   vertex mode: when near vertex, priority over line mode, so encountered first in list
 * mouse down enters move state for one of three modes
 * 
 * manages state and coordinates during edit mode
 * need to draw the rectangle, but is actually a four line segments, for now
 * may need to use inheritance if doing more complicated polygons
 */

class Outline {
public:
  
  typedef boost::shared_ptr<Outline> pOutline_t;
  typedef std::vector<wxPoint> vPoints_t;
  
  Outline( const wxRect& rect, bool bSelectByEdge = true, bool bSelectByVertex = true );  // top-left, bottom-right by default
  virtual ~Outline( void );
  
  void Erase( wxDC& dc ); // need to know when background has been erased
  void Render( wxDC& dc ); // handle the outline
  wxCursor TrackMouse( wxPoint, bool bDown, wxDC& dc );  // deals with cursor
  
  
  void GetCoords( vPoints_t& vPoints );
  wxRect GetBoundingBox( void );
  
private:
  
  enum Selection { bl = 0, br, tr, tl, count, top, right, bottom, left, all, none };
  enum MouseState { unknown, up, down };
  
  static const int bounding = 2;
  const float m_epsilon;
  
  bool m_bMouseDown;
  bool m_bUndoPrevious;
  bool m_bSelectByEdge;
  bool m_bSelectByVertex;
  bool m_bClean;
  std::set<Selection> m_setCorners;  // used for iterating corners
  std::set<Selection> m_setEdges;
  MouseState m_stateMouse;
  Selection m_selectionHit;
  wxPen m_penWhite;
  wxPen m_penBlack;
  wxBrush m_brush;
  wxPoint m_polyOriginal[ count ]; // top left, top right, bottom right, bottom left
  wxPoint m_polyNew[ count ];
  wxPoint m_pointMouseDown; // origin at mouse down event
  wxPoint m_pointMousePrevious;  
  wxRect m_rectBoundingBox[ 2 ]; // bounding box for each triangle, solves point to line problem when not within endpoints

  bool HitTest( wxPoint );

  void UpdateBoundingBoxes( void ); // performs inflation
  void GetBoundingBoxes( wxRect& box1, wxRect& box2 );  // no inflation
  void GetBoundingBox( wxPoint a, wxPoint b, wxPoint c, wxRect& rect ); 
  double PointToSegmentDistanceSquared( wxPoint point1, wxPoint point2, wxPoint point );
  bool PointInTriangle( wxPoint a, wxPoint b, wxPoint c, wxPoint p );
};

#endif	/* OUTLINING_H */

