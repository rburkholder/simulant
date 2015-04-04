/* 
 * File:   Outlining.h
 * Author: rpb
 *
 * Created on April 3, 2015, 10:30 PM
 */

#ifndef OUTLINING_H
#define	OUTLINING_H

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
 */

class Outlining {
public:
  Outlining( );
  virtual ~Outlining( );
private:

};

#endif	/* OUTLINING_H */

