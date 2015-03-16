/* 
 * File:   DndDropTarget.h
 * Author: rpb
 *
 * Created on March 1, 2015, 5:34 PM
 */

#ifndef DNDDROPTARGET_H
#define	DNDDROPTARGET_H

#include <wx/dnd.h>

class DndDropTarget: public wxDropTarget {
public:
  DndDropTarget( );
  //DndDropTarget( const DndDropTarget& orig );
  virtual ~DndDropTarget( );
  
  bool OnDropText (wxCoord x, wxCoord y, const wxString &data);
  
protected:  
  
  virtual bool GetData();  // callable from within OnData only
  virtual wxDragResult OnData (wxCoord x, wxCoord y, wxDragResult defResult);
  virtual wxDragResult OnDragOver (wxCoord x, wxCoord y, wxDragResult defResult);
  virtual bool 	OnDrop (wxCoord x, wxCoord y);
  virtual wxDragResult OnEnter (wxCoord x, wxCoord y, wxDragResult defResult);
  virtual void OnLeave ();
  
private:
  

};

#endif	/* DNDDROPTARGET_H */

