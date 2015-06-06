/* 
 * File:   DndDropTarget.cpp
 * Author: rpb
 * 
 * Created on March 1, 2015, 5:34 PM
 */

#include <iostream>

#include "DndDropTarget.h"

DndDropTarget::DndDropTarget( ) {
}

//DndDropTarget::DndDropTarget( const DndDropTarget& orig ) {
//}

DndDropTarget::~DndDropTarget( ) {
}

bool DndDropTarget::OnDropText (wxCoord x, wxCoord y, const wxString &data) {
  std::cout << "got text " << data << std::endl;
  // do nothing with the text for now, inheritor should deal with it
  return true;
}

bool DndDropTarget::GetData() {
  std::cout << "got GetData " << std::endl;
  return true;
}

wxDragResult DndDropTarget::OnData (wxCoord x, wxCoord y, wxDragResult defResult) {
  std::cout << "got OnData " << defResult << std::endl;
  return defResult;
}

wxDragResult DndDropTarget::OnDragOver (wxCoord x, wxCoord y, wxDragResult defResult) {
  std::cout << "got OnDragOver " << defResult << std::endl;
  return defResult;
}

bool DndDropTarget::OnDrop (wxCoord x, wxCoord y) {
  std::cout << "got OnDrop " << x << "," << y << std::endl;
  return true;
}

wxDragResult DndDropTarget::OnEnter (wxCoord x, wxCoord y, wxDragResult defResult) {
  std::cout << "got OnEnter " << defResult << std::endl;
  return defResult;
}

void DndDropTarget::OnLeave () {
  std::cout << "got OnLeave " << std::endl;
}
