/* 
 * File:   panelOpenGL.h
 * Author: rpb
 *
 * Created on March 15, 2015, 10:13 PM
 */

#pragma once

#include <wx/glcanvas.h>

class canvasOpenGL: public wxGLCanvas {
public:
  //canvasOpenGL();
  canvasOpenGL( wxFrame* parent, int* args );
  virtual ~canvasOpenGL( );
protected:
private:
  wxGLContext*	m_context;
  
  void Init();
  void Resized( wxSizeEvent& evt );
  void Render( wxPaintEvent& evt );
};

